#pragma once

#include <Pose2d.h>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <vector>
#include <iostream>

class RobotTracking
{
public:
	static constexpr const double TimeToken = 1e6;

public:
	RobotTracking(const QString &name) {
		name_ = name;
		current_time_ = std::numeric_limits<double>::max();
	}

	virtual ~RobotTracking() {
	}

	size_t size() const {
		return data_.size();
	}

	double time(size_t index) const {
		return data_[index].time_;
	}

	const xero::paths::Pose2d& pose(size_t index) const {
		return data_[index].pose_;
	}

	void add(double t, const xero::paths::Pose2d& pose) {
		if (std::abs(t - TimeToken) < kEpsilon)
			return;

		if (std::abs(t - current_time_) < kEpsilon)
			return;

		if (t < current_time_) {
			//
			// Time went backwards, we are starting over.  Clear the current data.
			//
			data_.clear();
		}
		data_.push_back(Pose2dAtTime(t, pose));

		current_time_ = t;
	}

private:
	struct Pose2dAtTime {
		Pose2dAtTime(double t, const xero::paths::Pose2d& p) {
			time_ = t;
			pose_ = p;
		}

		xero::paths::Pose2d pose_;
		double time_;
	};

private:
	static constexpr const double kEpsilon = 1e-6 ;

private:
	QString name_;
	double current_time_;
	std::vector<Pose2dAtTime> data_;
};

