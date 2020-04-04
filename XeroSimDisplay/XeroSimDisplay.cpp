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
	std::string nttable_name = "XeroSim";
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

	if (settings_.contains("leftright"))
	{
		QList<QVariant> stored = settings_.value("leftright").toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		left_right_splitter_->setSizes(sizes);
	}

	if (settings_.contains("topbottom"))
	{
		QList<QVariant> stored = settings_.value("topbottom").toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		top_bottom_spliter_->setSizes(sizes);
	}
}

XeroSimDisplay::~XeroSimDisplay()
{
}

void XeroSimDisplay::closeEvent(QCloseEvent* ev)
{
	settings_.setValue("GEOMETERY", saveGeometry());
	settings_.setValue("STATE", saveState());
	QMainWindow::closeEvent(ev);

	QList<QVariant> stored;
	for (int size : left_right_splitter_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue("leftright", stored);

	stored.clear();
	for (int size : top_bottom_spliter_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue("topbottom", stored);
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
	left_right_splitter_ = new QSplitter();
	left_right_splitter_->setOrientation(Qt::Orientation::Vertical);
	setCentralWidget(left_right_splitter_);

	field_view_ = new PathFieldView(left_right_splitter_);
	left_right_splitter_->addWidget(field_view_);

	top_bottom_spliter_ = new QSplitter(top_bottom_spliter_);
	top_bottom_spliter_->setOrientation(Qt::Orientation::Horizontal);
	left_right_splitter_->addWidget(top_bottom_spliter_);

	turret_view_ = new TurretWidget(top_bottom_spliter_);
	top_bottom_spliter_->addWidget(turret_view_);

	conveyor_view_ = new ConveyorWidget(top_bottom_spliter_);
	top_bottom_spliter_->addWidget(conveyor_view_);

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
	double xpos, ypos, angle;
	std::string txt;
	bool ready = false;

	auto tankdrive = table_->GetSubTable("tankdrive");
	xpos = tankdrive->GetNumber("xpos", 0.0);
	ypos = tankdrive->GetNumber("ypos", 0.0);
	angle = tankdrive->GetNumber("angle", 0.0);
	txt = tankdrive->GetString("text", "#");

	field_view_->setRobotPosition(xpos, ypos, angle);
	field_view_->setRobotText(txt.c_str());

	auto turret = table_->GetSubTable("turret");
	angle = turret->GetNumber("angle", 0.0);
	turret_view_->setAngle(angle);

	auto conveyor = table_->GetSubTable("conveyor");

	double minpos, maxpos;

	minpos = conveyor->GetNumber("MinConveyorPosition", 0.0);
	maxpos = conveyor->GetNumber("MaxConveyorPosition", 0.0);
	conveyor_view_->setDimensions(minpos, maxpos);

	char ch = 'A';

	while (true) {
		std::string str = "Sensor";
		str += ch;
		auto senstab = conveyor->GetSubTable(str);

		if (!senstab->ContainsKey("position"))
			break;

		double pos = senstab->GetNumber("position", 0.0);
		conveyor_view_->setSensorPosition(ch - 'A', pos);

		bool b = senstab->GetBoolean("state", false);
		conveyor_view_->setSensorState(ch - 'A', b);

		ch++;
	}

	int index = 1;
	while (true) {
		std::string str = std::to_string(index);
		auto balltab = conveyor->GetSubTable(str);
	
		if (!balltab->ContainsKey("position"))
			break;

		double pos = balltab->GetNumber("position", 0.0);
		conveyor_view_->setBallPosition(index, pos);

		bool b = balltab->GetBoolean("present", false);
		conveyor_view_->setBallPresent(index, b);

		ready = true;

		index++;
	}

	if (ready)
		conveyor_view_->setReady();
}
