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

	last_robot_update_ = std::chrono::high_resolution_clock::now();
	last_subsystem_update_ = std::chrono::high_resolution_clock::now();

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
	left_right_splitter_ = new QSplitter();
	left_right_splitter_->setOrientation(Qt::Orientation::Horizontal);
	setCentralWidget(left_right_splitter_);

	top_bottom_spliter_ = new QSplitter();
	top_bottom_spliter_->setOrientation(Qt::Orientation::Vertical);

	field_view_ = new PathFieldView(top_bottom_spliter_);
	top_bottom_spliter_->addWidget(field_view_);

	logger_ = new QTextEdit(top_bottom_spliter_);
	logger_->setReadOnly(true);
	top_bottom_spliter_->addWidget(logger_);

	left_right_splitter_->addWidget(top_bottom_spliter_);
	subsystems_ = new QTreeWidget();
	subsystems_->setColumnCount(2);
	QStringList header;
	header.push_back("Param");
	header.push_back("Value");
	subsystems_->setHeaderLabels(header);
	left_right_splitter_->addWidget(subsystems_);

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

	auto tankdrive = table_->GetSubTable("tankdrive");
	xpos = tankdrive->GetNumber("xpos", 0.0);
	ypos = tankdrive->GetNumber("ypos", 0.0);
	angle = tankdrive->GetNumber("angle", 0.0);
	txt = tankdrive->GetString("text", "#");

	field_view_->setRobotPosition(xpos, ypos, angle);
	field_view_->setRobotText(txt.c_str());
}
