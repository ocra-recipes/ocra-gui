#ifndef PHYSICSTOOLS_H
#define PHYSICSTOOLS_H
#include <Eigen/Lgsm>
#include <cmath>


void quaternionToRPY(const Eigen::Rotation3d& q, double& roll, double& pitch, double& yaw);
void displacementToXYZRPY(const Eigen::Displacementd& disp, std::vector<double>& v);

#endif // PHYSICSTOOLS_H
