#include "ConveyorWidget.h"
#include <QPainter>

ConveyorWidget::ConveyorWidget(QWidget *parent) : QWidget(parent)
{
	left_margin_ = 12.0;
	right_margin_ = 12.0;
	top_margin_ = 4.0 + WallThickness / 2.0;
	bottom_margin_ = 4.0 + WallThickness / 2.0;

	ready_ = false;
	need_scale_ = true;

	xoffset_ = 0.0;
	yoffset_ = 0.0;

	minpos_ = 0.0;
	maxpos_ = 0.0;

	fired_ = 0;
}

ConveyorWidget::~ConveyorWidget()
{
}

void ConveyorWidget::calcScale()
{
	double wheight = BallDiameter + 2 * WallThickness + SensorHeight + SensorTextHeight;
	double wwidth = maxpos_ - minpos_;

	double w = width() - left_margin_ - right_margin_;
	double h = height() - top_margin_ - bottom_margin_;

	double scaley = h / wheight;
	double scalex = w / wwidth;

	if (scalex > scaley) {
		scale_ = scaley;
		xoffset_ = (w - wwidth * scale_) / 2.0;
		yoffset_ = 0.0;
	}
	else {
		scale_ = scalex;
		xoffset_ = 0.0;
		yoffset_ = (h - wheight * scale_) / 2.0;
	}
}

QPointF ConveyorWidget::worldToWindow(const QPointF& p) 
{
	return QPointF(p.x() * scale_ + left_margin_ + xoffset_, p.y() * scale_ + right_margin_ + yoffset_);
}

void ConveyorWidget::paintEvent(QPaintEvent* ev)
{
	QPointF p1, p2;
	double total = maxpos_ - minpos_;

	if (!ready_)
		return;

	if (need_scale_) {
		calcScale();
		need_scale_ = false;
	}

	QPainter p(this);
	QPen pen(QColor(0xFF, 0xA5, 0x00, 0xFF));
	pen.setCapStyle(Qt::RoundCap);
	pen.setWidth(WallThickness * scale_);
	p.setPen(pen);

	p1 = worldToWindow(QPointF(0.0, 0.0));
	p2 = worldToWindow(QPointF(total, 0.0));
	p.drawLine(p1, p2);

	p1 = worldToWindow(QPointF(0.0, BallDiameter + WallThickness));
	p2 = worldToWindow(QPointF(total, BallDiameter + WallThickness));
	p.drawLine(p1, p2);

	QFontMetricsF fm(p.font());
	if (sensor_pos_.size() == sensor_state_.size()) {
		for (int i = 0; i < sensor_pos_.size(); i++) {
			p1 = QPointF(sensor_pos_[i] - SensorWidth / 2.0, BallDiameter + WallThickness * 3 / 2.0);
			p2 = QPointF(sensor_pos_[i] + SensorWidth / 2.0, BallDiameter + WallThickness * 3 / 2.0 + SensorHeight);

			p1 = worldToWindow(p1);
			p2 = worldToWindow(p2);

			p.setPen(Qt::PenStyle::NoPen);
			if (sensor_state_[i])
				p.setBrush(QColor(0, 255, 0, 255));
			else
				p.setBrush(QColor(255, 0, 0, 255));

			QRectF r(p1, p2);
			p.drawRect(r);

			p.setPen(QColor(0, 0, 0, 255));
			p2 = QPointF(sensor_pos_[i], BallDiameter + WallThickness * 3 / 2.0 + SensorHeight);
			p2 = worldToWindow(p2);
			std::string str;
			str += (char)('A' + i);

			QPointF p3(p2.x() - fm.horizontalAdvance(str.c_str()) / 2.0, p2.y() + fm.height());
			p.drawText(p3, str.c_str());
		}
	}

	double sensdia = SensorBallDiameter * scale_;
	double balldia = BallDiameter * scale_;
	if (ball_pos_.size() == ball_present_.size()) {
		for (int i = 0; i < ball_pos_.size(); i++) {
			if (ball_present_[i]) {
				p1 = worldToWindow(QPointF(ball_pos_[i], BallDiameter / 2.0 + WallThickness / 2.0));

				QRectF r2(p1.x() - balldia / 2.0, p1.y() - balldia / 2.0, balldia, balldia);
				p.setPen(QColor(0, 0, 0, 255));
				p.setBrush(QColor(255, 255, 51, 255));
				p.drawEllipse(r2);

				QRectF r1(p1.x() - sensdia / 2.0, p1.y() - sensdia / 2.0, sensdia, sensdia);
				p.setPen(Qt::PenStyle::NoPen);
				p.setBrush(QColor(204, 204, 0, 255));
				p.drawEllipse(r1);
			}
		}
	}

	p.setBrush(QColor(0, 0, 0, 255));
	p.setPen(QColor(0, 0, 0, 255));
	p1 = worldToWindow(QPointF(total / 2, (BallDiameter + WallThickness) / 2));
	QString str = "Launched " + QString::number(fired_);
	QPointF p3(p1.x() - fm.horizontalAdvance(str) / 2.0, p1.y() + fm.height());
	p.drawText(p3, str);
}

void ConveyorWidget::drawBall(QPainter &p, double pos) 
{
}

void ConveyorWidget::resizeEvent(QResizeEvent* ev)
{
	need_scale_ = true;
}