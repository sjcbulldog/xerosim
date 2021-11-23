#include "XeroSimDisplay.h"
#include "ConfigDialog.h"
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
	settings_.setValue("GEOMETERY", saveGeometry());
	settings_.setValue("STATE", saveState());
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
	field_view_ = new PathFieldView();
	setCentralWidget(field_view_);

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
	double xpos, ypos, angle, t;
	std::string txt;
	bool ready = false;

	auto tankdrive = table_->GetSubTable("SmartDashboard");

	t = tankdrive->GetNumber("db-trk-t", RobotTracking::TimeToken);
	xpos = tankdrive->GetNumber("db-trk-x", 0.0);
	ypos = tankdrive->GetNumber("db-trk-y", 0.0);
	angle = tankdrive->GetNumber("db-trk-a", 0.0);
	field_view_->addTrackPosition(t, xero::paths::Pose2d(xpos, ypos, xero::paths::Rotation2d::fromDegrees(angle)));

	t = tankdrive->GetNumber("db-path-t", RobotTracking::TimeToken);
	xpos = tankdrive->GetNumber("db-path-x", 0.0);
	ypos = tankdrive->GetNumber("db-path-y", 0.0);
	angle = tankdrive->GetNumber("db-path-a", 0.0);
	field_view_->addPathPosition(t, xero::paths::Pose2d(xpos, ypos, xero::paths::Rotation2d::fromDegrees(angle)));

	field_view_->update();
}
