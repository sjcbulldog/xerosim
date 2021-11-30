//
// Copyright 2019 Jack W. Griffin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
//
#include "PathFieldView.h"
#include <Pose2d.h>
#include <QPainter>
#include <QPointF>
#include <QMouseEvent>
#include <QFontMetrics>
#include <QDebug>
#include <QCoreApplication>
#include <QFontMetricsF>
#include <cmath>
#include <cassert>

using namespace xero::paths;

PathFieldView::PathFieldView(QWidget* parent) : QWidget(parent), track_("robot"), path_("path")
{
	units_ = "in";
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	image_scale_ = 1.0;
}

PathFieldView::~PathFieldView()
{
}

void PathFieldView::setUnits(const std::string& units)
{
	units_ = units;
	createTransforms();
	repaint();
}

QSize PathFieldView::minimumSizeHint() const
{
	return QSize(field_image_.size().width() / 4, field_image_.size().height() / 4);
}

QSize PathFieldView::sizeHint() const
{
	return field_image_.size();
}

void PathFieldView::paintEvent(QPaintEvent* event)
{
	(void)event;

	//
	// Draw the field
	//
	QPainter paint(this);
	doPaint(paint);
}

void PathFieldView::doPaint(QPainter& paint, bool printing)
{
	QRectF rect(0.0f, 0.0f, field_image_.width() * image_scale_, field_image_.height() * image_scale_);
	paint.drawImage(rect, field_image_);

	drawPath(paint, QColor(255, 0, 0), track_);
	drawPath(paint, QColor(0, 0, 255), path_);
}


void PathFieldView::emitMouseMoved(Translation2d pos)
{
	emit mouseMoved(pos);
}

void PathFieldView::mouseMoveEvent(QMouseEvent* ev)
{
}

void PathFieldView::mousePressEvent(QMouseEvent* ev)
{
}

void PathFieldView::mouseReleaseEvent(QMouseEvent* ev)
{
}

void PathFieldView::keyPressEvent(QKeyEvent* ev)
{
}

std::vector<QPointF> PathFieldView::transformPoints(QTransform& trans, const std::vector<QPointF>& points)
{
	std::vector<QPointF> result;

	for (const QPointF& pt : points)
	{
		QPointF newpt = trans.map(pt);
		result.push_back(newpt);
	}

	return result;
}

void PathFieldView::resizeEvent(QResizeEvent* event)
{
	(void)event;
	createTransforms();
}

void PathFieldView::setField(std::shared_ptr<GameField> field)
{
	field_ = field;

	//
	// Load the image,
	//
	QImage image(field_->getImageFile());
	field_image_ = image;

	//
	// Create new transforms
	//
	createTransforms();

	//
	// Redraw the widget
	//
	repaint(geometry());
}

void PathFieldView::createTransforms()
{
	//
	// First calculate the scale to use to scale the image to the window
	// without changing the aspec ratio.  Pick the largest scale factor that will fit
	// the most constrained dimension
	//
	double sx = (double)width() / (double)field_image_.width();
	double sy = (double)height() / (double)field_image_.height();

	if (sx < sy)
		image_scale_ = sx;
	else
		image_scale_ = sy;

	//
	// Now, we know now to scale the image to window pixels, scale the top left and bottom
	// right coordiantes from the image to the window
	//
	Translation2d tl = field_->getTopLeft().scale(image_scale_);
	Translation2d br = field_->getBottomRight().scale(image_scale_);

	//
	// Now we know two points on the field and how they coorespond to two points on the window.
	// The field point (0, FieldSize.Y) cooresponds to the point tl.
	// The field point (FieldsSize.X, 0) coorespons to the point br.
	//
	// Compute the transform based on this.  
	//
	world_to_window_ = QTransform();

	world_to_window_.translate(tl.getX(), br.getY());
	double scale = (br.getX() - tl.getX()) / field_->getSize().getX();
	world_to_window_.scale(scale, -scale);

	window_to_world_ = world_to_window_.inverted();
}

QPointF PathFieldView::worldToWindow(const QPointF& pt)
{
	return world_to_window_.map(pt);
}

QPointF PathFieldView::windowToWorld(const QPointF& pt)
{
	return window_to_world_.map(pt);
}

std::vector<QPointF> PathFieldView::worldToWindow(const std::vector<QPointF>& points)
{
	return transformPoints(world_to_window_, points);
}

std::vector<QPointF> PathFieldView::windowToWorld(const std::vector<QPointF>& points)
{
	return transformPoints(window_to_world_, points);
}


static int count = 0;
void PathFieldView::drawPath(QPainter &p, QColor c, const RobotTracking &track)
{
	const int cross = 6;
	QPointF p1, p2;

	QPen pen(c);
	pen.setWidth(2);
	p.setPen(pen);

	count++;

	for (int i = 0; i < track.size(); i++) {
		QPointF winpos = worldToWindow(QPointF(track.pose(i).getTranslation().getX(), track.pose(i).getTranslation().getY()));

		p1 = QPointF(winpos.x() - cross, winpos.y());
		p2 = QPointF(winpos.x() + cross, winpos.y());
		p.drawLine(p1, p2);

		p1 = QPointF(winpos.x(), winpos.y() - cross);
		p2 = QPointF(winpos.x(), winpos.y() + cross);
		p.drawLine(p1, p2);
	}
}
