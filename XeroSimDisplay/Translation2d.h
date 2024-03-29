#pragma once

#include <cmath>
#include <algorithm>

namespace xero 
{
	namespace paths
	{
		class Rotation2d;

		class Translation2d
		{
		public:
			Translation2d();
			Translation2d(double x, double y);
			Translation2d(const Translation2d& other);
			Translation2d(const Translation2d& start, const Translation2d& end);
			Translation2d(const Rotation2d& rot, double magnitude);
			Rotation2d toRotation() const;
			double normalize() const;
			double normalizeSquared() const;
			Translation2d translateBy(const Translation2d& translation) const;
			Translation2d rotateBy(const Rotation2d& rotation) const;
			Translation2d inverse() const;
			Translation2d interpolate(const Translation2d& other, double percent) const;
			Translation2d extrapolate(const Translation2d& other, double percent) const;
			Translation2d scale(double scale) const;
			Rotation2d direction() const;
			double dot(const Translation2d& a, const Translation2d& b);
			static double cross(const Translation2d& a, const Translation2d& b);
			double distance(const Translation2d& other) const;
			bool epsilonEqual(const Translation2d& other, double eps) const;
			Rotation2d getAngle(const Translation2d& a, const Translation2d& b);

			double getX() const {
				return x_;
			}

			double getY() const {
				return y_;
			}
			

			static Translation2d kIdentity;

		private:
			double x_;
			double y_;
		};
	}
}

