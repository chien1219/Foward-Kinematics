#include "kinematics_time_warp_hard_constraint.h"

namespace kinematics {

// public func.

TimeWarpHardConstraint::TimeWarpHardConstraint()
    :frame_idx(0),
    play_second(double{0.0})
{
}

TimeWarpHardConstraint::TimeWarpHardConstraint(
        const int32_t frame_idx_param,
        const double play_second_param
        )
    :frame_idx(frame_idx_param),
    play_second(play_second_param)
{
}

TimeWarpHardConstraint::~TimeWarpHardConstraint()
{
}

// protected func.

// private func.

} // namespace kinematics {
