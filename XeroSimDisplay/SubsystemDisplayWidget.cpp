#include "SubsystemDisplayWidget.h"
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

SubsystemDisplayWidget::SubsystemDisplayWidget(const QString& name, QWidget *parent) : QWidget(parent)
{
	name_ = name;
	value_ = 0.0;

	setMinimumHeight(20);
	setMinimumHeight(20);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

SubsystemDisplayWidget::~SubsystemDisplayWidget()
{
}

void SubsystemDisplayWidget::paintEvent(QPaintEvent* ev)
{
	QString label;
	QPainter p(this);
	p.setRenderHint(QPainter::HighQualityAntialiasing);

	p.setBrush(QColor(192, 192, 192));
	p.drawRect(0, 0, width(), height());

	p.setBrush(QColor(0, 0, 0));
	p.setFont(font());

	QFontMetricsF fm(font());
	int w = fm.horizontalAdvance(name_) ;
	int xpos = width() / 2 - w / 2;
	int ypos = fm.height() - fm.descent();
	p.drawText(QPoint(xpos, ypos), name_);

	QPainterPath path;
	QPen pen(QColor(0, 0, 0));
	p.setPen(pen);
	p.setBrush(Qt::BrushStyle::NoBrush);
	QRectF r(sideMargin, fm.height() + labelBarSpacing, width() - 2 * sideMargin, barHeight);
	path.addRoundedRect(r, 4.0, 4.0);
	p.drawPath(path);

	double pcnt = (value_ / 2.0) + 0.5;
	w = width();
	int clipx = sideMargin + (w - sideMargin * 2) * pcnt;
	p.setClipRect(0, 0, clipx, height(), Qt::ClipOperation::ReplaceClip);

	QColor c = QColor(255, 255, 0);
	if (value_ < 0.0)
		c = QColor(255, 0, 0);
	else if (value_ > 0.0)
		c = QColor(0, 255, 0);
	p.fillPath(path, c);
	p.setClipRect(0, 0, 0, 0, Qt::ClipOperation::NoClip);

	label = QString::number(value_, 'f', 2);
	w = fm.horizontalAdvance(label);
	xpos = width() / 2 - w / 2;
	ypos = fm.height() + labelBarSpacing + barHeight / 2 + fm.descent() + 2;
	p.drawText(xpos, ypos, label);

	label = "-1.0";
	xpos = sideMargin;
	ypos = fm.height() + labelBarSpacing + barHeight + barUnitsSpacing + fm.height() - fm.descent();
	p.drawText(QPoint(xpos, ypos), label);

	label = "0.0";
	xpos = width() / 2 - fm.horizontalAdvance(label) / 2;
	p.drawText(QPoint(xpos, ypos), label);

	label = "1.0";
	xpos = width() - fm.horizontalAdvance(label);
	p.drawText(QPoint(xpos, ypos), label);
}

QSize SubsystemDisplayWidget::sizeHint() const {
	QFontMetricsF fm(font());

	int height = 2 * fm.height() + barHeight + labelBarSpacing + barUnitsSpacing;
	int width = fm.horizontalAdvance(name_) * 2.0 + sideMargin + sideMargin;
	if (width < minWidth)
		width = minWidth;

	return QSize(width, height);
}
