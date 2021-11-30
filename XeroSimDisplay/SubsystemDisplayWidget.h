#pragma once

#include <QtCore/QString>
#include <QtWidgets/QWidget>

class SubsystemDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	SubsystemDisplayWidget(const QString &name,QWidget *parent = Q_NULLPTR);
	~SubsystemDisplayWidget();

	void setValue(double v) {
		value_ = v;
		update();
	}

	QSize sizeHint() const override;


protected:
	void paintEvent(QPaintEvent* ev) override;

private:
	static const int minWidth = 60;
	static const int barHeight = 16;
	static const int barLabelHeight = 16;
	static const int sideMargin = 4;
	static const int labelBarSpacing = 4;
	static const int barUnitsSpacing = 4;

private:
	QString name_;
	double value_;
};
