#include "XeroSimDisplay.h"
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QProcess>
#include <QMessageBox>
#include <QTextCodec>
#include <QProcessEnvironment>
#include <QTemporaryFile>
#include <QMessageBox>

XeroSimDisplay::XeroSimDisplay(GameFieldManager &fields, QWidget *parent) : QMainWindow(parent), fields_(fields)
{
	std::string nttable_name = "/";
	fields_.initialize();

	createWindows();
	createMenus();

	if (settings_.contains(GeometrySettings))
		restoreGeometry(settings_.value(GeometrySettings).toByteArray());

	if (settings_.contains(WindowStateSettings))
		restoreState(settings_.value(WindowStateSettings).toByteArray());

	if (settings_.contains(SplitterSettings))
	{
		QList<QVariant> stored = settings_.value(SplitterSettings).toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		left_right_->setSizes(sizes);
	}

	std::string name = "Empty Field";
	if (settings_.contains("FIELD"))
	{
		QString field = settings_.value("FIELD").toString();
		name = field.toStdString();
	}
	setField(name);
	field_view_->setUnits("in");

	timer_ = new QTimer(this);
	(void)connect(timer_, &QTimer::timeout, this, &XeroSimDisplay::timerProc);
	timer_->start(1);

	nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
	inst.StartClient("127.0.0.1");
	table_ = inst.GetTable(nttable_name.c_str());
}

XeroSimDisplay::~XeroSimDisplay()
{
}

void XeroSimDisplay::closeEvent(QCloseEvent* ev)
{
	settings_.setValue(GeometrySettings, saveGeometry());
	settings_.setValue(WindowStateSettings, saveState());

	QList<QVariant> stored;
	for (int size : left_right_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue(SplitterSettings, stored);

	QMainWindow::closeEvent(ev);
}

void XeroSimDisplay::setField(const std::string& name)
{
	//
	// Set the current field
	//
	if (current_field_ != name.c_str())
	{
		auto field = fields_.getFieldByName(name);
		if (field != nullptr)
		{
			field->convert("in");
			field_view_->setField(field);
			current_field_ = name.c_str();
		}
	}
}

void XeroSimDisplay::createWindows()
{
	left_right_ = new QSplitter(Qt::Horizontal);
	setCentralWidget(left_right_);

	field_view_ = new PathFieldView();
	left_right_->addWidget(field_view_);

	subsystem_view_ = new SubsystemWidget(left_right_);
	left_right_->addWidget(subsystem_view_);

	if (settings_.contains("GEOMETRY"))
		restoreGeometry(settings_.value("GEOMETRY").toByteArray());

	if (settings_.contains("STATE"))
		restoreState(settings_.value("STATE").toByteArray());
}

void XeroSimDisplay::createMenus()
{
	QAction *action;

	field_ = this->menuBar()->addMenu("Fields");
	field_group_ = new QActionGroup(this);
	field_group_->setExclusive(true);
	for (auto& field : fields_.getNames())
	{
		QAction* fieldact = field_->addAction(field.c_str());
		field_group_->addAction(fieldact);
		(void)connect(fieldact, &QAction::triggered, this, [this, field] { newFieldSelected(field); });
	}
}

void XeroSimDisplay::newFieldSelected(const std::string &fieldname)
{
	if (fieldname.c_str() != current_field_)
	{
		setField(fieldname);
		settings_.setValue("field", fieldname.c_str());
	}
}

void XeroSimDisplay::timerProc()
{
	static std::string nameend = "-subsystem";

	double txpos, typos, tangle, tt;
	double pxpos, pypos, pangle, pt;
	std::string txt;
	bool ready = false;

	auto smart = table_->GetSubTable("SmartDashboard");

	tt = smart->GetNumber("db-trk-t", RobotTracking::TimeToken);
	txpos = smart->GetNumber("db-trk-x", 0.0);
	typos = smart->GetNumber("db-trk-y", 0.0);
	tangle = smart->GetNumber("db-trk-a", 0.0);
	field_view_->addTrackPosition(tt, xero::paths::Pose2d(txpos, typos, xero::paths::Rotation2d::fromDegrees(tangle)));

	pt = smart->GetNumber("db-path-t", RobotTracking::TimeToken);
	pxpos = smart->GetNumber("db-path-x", 0.0);
	pypos = smart->GetNumber("db-path-y", 0.0);
	pangle = smart->GetNumber("db-path-a", 0.0);
	field_view_->addPathPosition(pt, xero::paths::Pose2d(pxpos, pypos, xero::paths::Rotation2d::fromDegrees(pangle)));

	std::vector<std::string> keys = smart->GetKeys();
	for (const std::string& key : keys) {
		if (key.length() > nameend.length() && key.substr(key.length() - nameend.length()) == nameend)
		{
			std::string name = key.substr(0, key.length() - nameend.length());
			if (smart->ContainsKey(name + "-power"))
			{
				double v = smart->GetNumber(name + "-power", 0.0);
				subsystem_view_->updateSubsystem(name, v);
			}
		}
	}

	field_view_->update();
}
