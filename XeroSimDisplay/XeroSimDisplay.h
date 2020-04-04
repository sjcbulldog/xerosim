#pragma once

#include <QtWidgets/QMainWindow>
#include "GameFieldManager.h"
#include "PathFieldView.h"
#include "TurretWidget.h"
#include "ConveyorWidget.h"
#include "SubsystemData.h"
#include <QSplitter>
#include <QTextEdit>
#include <QMenu>
#include <QSettings>
#include <QProcess>
#include <QTimer>
#include <QTreeWidget>
#include <memory>
#include <thread>
#include <map>
#include <mutex>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4100 4458)
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif

#include <networktables/NetworkTableInstance.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSVC
#pragma warning(pop)
#endif


class XeroSimDisplay : public QMainWindow
{
	Q_OBJECT

public:
	XeroSimDisplay(GameFieldManager &field, QWidget *parent = Q_NULLPTR);
	virtual ~XeroSimDisplay();

protected:
	void setField(const std::string& field);
	void closeEvent(QCloseEvent* ev);

private:
	void newFieldSelected(const std::string& fieldname);

private:
	void createWindows();
	void createMenus();

	void timerProc();

private:
	GameFieldManager& fields_;
	QSettings settings_;
	QString current_field_;

	double robot_x_;
	double robot_y_;
	double robot_angle_;
	QString robot_text_;

	QSplitter* top_bottom_spliter_;
	QSplitter* left_right_splitter_;
	PathFieldView* field_view_;
	TurretWidget* turret_view_;
	ConveyorWidget* conveyor_view_;

	QMenu* field_;
	QActionGroup* field_group_;

	QTimer *timer_;

	std::shared_ptr<nt::NetworkTable> table_;
};

