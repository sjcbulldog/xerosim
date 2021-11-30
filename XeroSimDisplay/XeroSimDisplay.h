#pragma once

#include "SubsystemWidget.h"
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
#include <QtWidgets/QMainWindow>
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
	void closeEvent(QCloseEvent* ev) override;

private:
	void setField(const std::string& field);
	void newFieldSelected(const std::string& fieldname);

private:
	void createWindows();
	void createMenus();

	void timerProc();

private:
	static constexpr const char* GeometrySettings = "geometry";
	static constexpr const char* WindowStateSettings = "windowstate";
	static constexpr const char* SplitterSettings = "splitter";

private:
	GameFieldManager& fields_;
	QSettings settings_;
	QString current_field_;

	QSplitter* left_right_;
	PathFieldView* field_view_;
	SubsystemWidget* subsystem_view_;

	QMenu* field_;
	QActionGroup* field_group_;

	QTimer *timer_;

	std::shared_ptr<nt::NetworkTable> table_;
};

