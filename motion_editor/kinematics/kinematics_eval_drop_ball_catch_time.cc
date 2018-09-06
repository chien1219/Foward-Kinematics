#include "kinematics_eval_drop_ball_catch_time.h"
#include <cmath>

namespace kinematics {

// public func.

EvalDropBallCatchTime::EvalDropBallCatchTime()
{
}

EvalDropBallCatchTime::~EvalDropBallCatchTime()
{
}

double EvalDropBallCatchTime::operator()(
        const math::Vector3d_t &init_pos,
        const math::Vector3d_t &target_pos,
        const math::Vector3d_t &drop_acc
        ) const
{
    double distance = target_pos.y() - init_pos.y();
    return sqrt(double{2.0} * distance / drop_acc.y());
}

// protected func.

// private func.

} // namespace kinematics {
