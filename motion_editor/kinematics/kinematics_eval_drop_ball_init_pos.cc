#include "kinematics_eval_drop_ball_init_pos.h"
#include <cmath>
#include "math_utils.h"

namespace kinematics {

// public func.

EvalDropBallInitPos::EvalDropBallInitPos()
{
}

EvalDropBallInitPos::~EvalDropBallInitPos()
{
}

math::Vector3dPair_t EvalDropBallInitPos::operator()(
        const math::Vector3d_t &target_pos,
        const math::Vector3d_t &drop_acc,
        const double desired_catch_time,
        const double time_step
        ) const
{
    const double frame_idx = floor(desired_catch_time / time_step);

    const double lower_time = frame_idx * time_step;
    const double upper_time = lower_time + time_step;
    const math::Vector3d_t init_pos_first_boud
        = target_pos - double{0.5} * drop_acc * math::Square(lower_time);
    const math::Vector3d_t init_pos_second_bound
        = target_pos - double{0.5} * drop_acc * math::Square(upper_time);

    math::Vector3dPair_t init_pos_pair;
    if (init_pos_first_boud.y() <= init_pos_second_bound.y())
    {
        init_pos_pair.first = init_pos_first_boud;
        init_pos_pair.second = init_pos_second_bound;
    }
    else
    {
        init_pos_pair.first = init_pos_second_bound;
        init_pos_pair.second = init_pos_first_boud;
    }

    return init_pos_pair;
}


// protected func.

// private func.

} // namespace kinematics {
