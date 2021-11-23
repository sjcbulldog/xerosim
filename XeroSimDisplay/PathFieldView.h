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
#pragma once

#include "GameField.h"
#include "RobotTracking.h"
#include <RobotParams.h>
#include <Pose2d.h>
#include <QWidget>
#include <QPixmap>
#include <QTransform>
#include <QDebug>
#include <memory>

class PathFieldView : public QWidget
{
	friend class ShowRobotWindow;

	Q_OBJECT

public:
	PathFieldView(QWidget* parent = Q_NULLPTR);
	virtual ~PathFieldView();

	void setField(std::shared_ptr<GameField> field);
	void setUnits(const std::string& units);

	QPointF worldToWindow(const QPointF& pt);
	QPointF windowToWorld(const QPointF& pt);
	std::vector<QPointF> worldToWindow(const std::vector<QPointF>& points);
	std::vector<QPointF> windowToWorld(const std::vector<QPointF>& points);

	void doPaint(QPainter& paint, bool printing = false);

	void addTrackPosition(double t, const xero::paths::Pose2d &pose) {
		track_.add(t, pose);
	}

	void addPathPosition(double t, const xero::paths::Pose2d& pose) {
		path_.add(t, pose);
	}

signals:
	void mouseMoved(xero::paths::Translation2d pos);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;

private:
	void drawPath(QPainter& p, QColor c, const RobotTracking &track);

private:
	void emitMouseMoved(xero::paths::Translation2d pos);

	std::vector<QPointF> transformPoints(QTransform& trans, const std::vector<QPointF>& points);
	void createTransforms();

private:
	QImage field_image_;
	std::shared_ptr<GameField> field_;
	double image_scale_;
	QTransform world_to_window_;
	QTransform window_to_world_;
	std::string units_;

	RobotTracking track_;
	RobotTracking path_;
};
