#pragma once

#include <QWidget>
#include <vector>

class ConveyorWidget : public QWidget
{
	Q_OBJECT

public:
	ConveyorWidget(QWidget *parent = Q_NULLPTR);
	~ConveyorWidget();

	void setFired(int fired) {
		fired_ = fired;
	}

	void setSensorPosition(int i, double pos) {
		if (i >= sensor_pos_.size())
			sensor_pos_.resize(i + 1);

		sensor_pos_[i] = pos;
		update();
	}

	void setSensorState(int i, bool state) {
		if (i >= sensor_state_.size())
			sensor_state_.resize(i + 1);

		sensor_state_[i] = state;
		update();
	}

	void setBallPosition(int i, double pos) {
		if (i >= ball_pos_.size())
			ball_pos_.resize(i + 1);

		ball_pos_[i] = pos;
		update();
	}

	void setBallPresent(int i, bool state) {
		if (i >= ball_present_.size())
			ball_present_.resize(i + 1);
		ball_present_[i] = state;
		update();
	}

	void setDimensions(double minpos, double maxpos) {
		minpos_ = minpos;
		maxpos_ = maxpos;
	}

	void setReady() {
		ready_ = true;
		update();
	}

protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private:
	void drawBall(QPainter& p, double pos);
	void calcScale();

	QPointF worldToWindow(const QPointF& p);

private:
	static constexpr const double BallDiameter = 7.0;
	static constexpr const double SensorBallDiameter = 3.0;
	static constexpr const double WallThickness = 1.0;
	static constexpr const double SensorHeight = 1.0;
	static constexpr const double SensorTextHeight = 0.5;
	static constexpr const double SensorWidth = 0.5;

private:
	double top_margin_;
	double bottom_margin_;
	double left_margin_;
	double right_margin_;

	double scale_;

	double xoffset_;
	double yoffset_;

	std::vector<double> sensor_pos_;
	std::vector<bool> sensor_state_;
	std::vector<double> ball_pos_;
	std::vector<bool> ball_present_;
	int fired_;

	double minpos_;
	double maxpos_;

	bool ready_;
	bool need_scale_;
};
