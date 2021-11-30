#include "SubsystemWidget.h"
#include <QtCore/QDebug>

SubsystemWidget::SubsystemWidget(QWidget *parent) : QWidget(parent)
{
	layout_ = new QVBoxLayout();
	layout_->setAlignment(Qt::AlignTop);
	setLayout(layout_);
}

SubsystemWidget::~SubsystemWidget()
{
}

void SubsystemWidget::updateSubsystem(const std::string& name, double v)
{
	SubsystemDisplayWidget* display = nullptr;
	QString qname = QString::fromStdString(name);

	if (!display_.contains(qname))
	{
		display = new SubsystemDisplayWidget(qname);
		display_.insert(qname, display);
		layout_->addWidget(display);
	}
	else
	{
		display = display_.value(qname);
	}

	assert(display != nullptr);
	display->setValue(v);
}
