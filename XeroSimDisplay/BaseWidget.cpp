#include "BaseWidget.h"
#include <QPainter>

BaseWidget::BaseWidget(QWidget *parent) : QWidget(parent)
{
}

BaseWidget::~BaseWidget()
{
}

void BaseWidget::drawBase(QPainter& p, const QString& text)
{
	p.save();

	QPen pen(QColor(0, 0, 0));
	pen.setWidth(4);
	p.setPen(pen);

	QBrush brush(QColor(212, 212, 212));
	p.setBrush(brush);
	p.drawRect(0, 0, width(), height());

	p.save();
	QFont ftitle = p.font();
	ftitle.setPointSize(16);
	p.setFont(ftitle);
	QFontMetrics fmtitle(ftitle);
	QPoint pt(width() / 2 - fmtitle.horizontalAdvance(text) / 2, fmtitle.height());
	p.drawText(pt, text);

	p.restore();
}