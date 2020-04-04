#pragma once

#include <QWidget>
#include <QImage>

class TurretWidget : public QWidget
{
	Q_OBJECT

public:
	TurretWidget(QWidget *parent = Q_NULLPTR);
	~TurretWidget();

	void setAngle(double angle) {
		angle_ = angle;
		update();
	}

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;

private:
	double angle_;
	QImage *image_;
};
