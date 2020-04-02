#pragma once

#include <QtWidgets/QMainWindow>
#include "GameFieldManager.h"
#include "PathFieldView.h"
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
	QTextEdit* logger_;
	QTreeWidget* subsystems_;

	QMenu* run_;
	QMenu* field_;
	QActionGroup* field_group_;

	QTimer *timer_;

	QMetaObject::Connection rdstdout;
	QMetaObject::Connection rdstderr;
	QMetaObject::Connection procstarted;
	QMetaObject::Connection procfinished;
	QMetaObject::Connection erroroccurred;

	std::map<std::string, std::shared_ptr<SubsystemData>> subsystem_data_;

	std::mutex data_lock_;

	std::chrono::high_resolution_clock::time_point last_robot_update_;
	std::chrono::high_resolution_clock::time_point last_subsystem_update_;

	std::shared_ptr<nt::NetworkTable> table_;
};

