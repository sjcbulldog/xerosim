#pragma once

#include <QtCore/QMap>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QBoxLayout>
#include "SubsystemDisplayWidget.h"


class SubsystemWidget : public QWidget
{
	Q_OBJECT

public:
	SubsystemWidget(QWidget *parent = Q_NULLPTR);
	virtual ~SubsystemWidget();

	void updateSubsystem(const std::string& name, double v);

private:
	QVBoxLayout* layout_;
	QMap<QString, SubsystemDisplayWidget*> display_;
};
