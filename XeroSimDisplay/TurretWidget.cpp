#include "TurretWidget.h"
#include <QPainter>
#include <QCoreApplication>

TurretWidget::TurretWidget(QWidget *parent) : QWidget(parent)
{
	QString filename = QCoreApplication::applicationDirPath() + "/turret.png";
	image_ = new QImage(filename);
}

TurretWidget::~TurretWidget()
{
}

void TurretWidget::paintEvent(QPaintEvent* ev)
{
	QPainter p(this);
	double dim = width();
	if (dim > height())
		dim = height();
	QRectF r(0, 0, dim, dim);
	QTransform t;
	t.translate(width() / 2.0, height() / 2.0);
	t.rotate(-angle_);
	t.translate(-r.width() / 2.0, -r.height() / 2.0);

	p.setTransform(t);
	p.drawImage(r, *image_);

	t = QTransform();
	p.setTransform(t);

	QFont f("Times", 14, QFont::Bold);
	p.setFont(f);
	QString angstr = QString::number(angle_);
	QFontMetricsF fm(f);
	int sw = fm.horizontalAdvance(angstr);
	p.drawText(width() / 2 - sw / 2, height() / 2 + fm.height() / 2, angstr);
}

void TurretWidget::resizeEvent(QResizeEvent* ev)
{
}