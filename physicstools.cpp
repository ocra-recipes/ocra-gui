#include "physicstools.h"


void quaternionToRPY(const Eigen::Rotation3d& q, double& roll, double& pitch, double& yaw)
{
    // double ysqr = q.y() * q.y();
    // double t0 = -2.0 * (ysqr + q.z() * q.z()) + 1.0;
    // double t1 = +2.0 * (q.x() * q.y() - q.w() * q.z());
    // double t2 = -2.0 * (q.x() * q.z() + q.w() * q.y());
    // double t3 = +2.0 * (q.y() * q.z() - q.w() * q.x());
    // double t4 = -2.0 * (q.x() * q.x() + ysqr) + 1.0;
    //
    // t2 = t2 > 1.0 ? 1.0 : t2;
    // t2 = t2 < -1.0 ? -1.0 : t2;
    //
    // roll = std::atan2(t3, t4);
    // pitch = std::asin(t2);
    // yaw = std::atan2(t1, t0);

    yaw = std::atan( 2.0*( q.x()*q.y() + q.w()*q.z() ) / ( q.w()*q.w() - q.z()*q.z() - q.y()*q.y() + q.x()*q.x() ) );

    pitch = std::asin( -2.0*( q.x()*q.z()-q.y()*q.w() ) );

    roll = std::atan( 2.0*( q.y()*q.z() + q.x()*q.w() ) / ( q.w()*q.w() + q.z()*q.z() - q.y()*q.y() - q.x()*q.x() ) );
}


void displacementToXYZRPY(const Eigen::Displacementd& disp, std::vector<double>& v)
{
    v.resize(6);
    v[0] = disp.x();
    v[1] = disp.y();
    v[2] = disp.z();
    double roll = 0.0;
    double pitch = 0.0;
    double yaw = 0.0;
    quaternionToRPY(disp.getRotation(), roll, pitch, yaw);
    v[3] = roll;
    v[4] = pitch;
    v[5] = yaw;
}
