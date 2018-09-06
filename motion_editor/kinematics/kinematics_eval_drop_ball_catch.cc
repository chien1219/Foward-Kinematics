#include "kinematics_eval_drop_ball_catch.h"
#include "kinematics_eval_drop_ball_pos.h"

namespace kinematics {

// public func.

EvalDropBallCatch::EvalDropBallCatch()
    :eval_drop_ball_pos_(nullptr)
{
}

EvalDropBallCatch::~EvalDropBallCatch()
{
}

bool EvalDropBallCatch::operator()(
        const int32_t frame_idx,
        const double ball_radius,
        const math::Vector3d_t &end_effector_pos
        ) const
{
    const math::Vector3d_t present_ball_pos = (*eval_drop_ball_pos_)(frame_idx);
    const math::Vector3d_t next_ball_pos = (*eval_drop_ball_pos_)(frame_idx + 1);

    const bool is_horizontally_catchable = this->EvalHorizontallyCatchable(
            end_effector_pos,
            present_ball_pos,
            ball_radius
            );
    if (!is_horizontally_catchable)
    {
        return false;
    }

    const bool is_vertically_catchable = this->EvalVerticallyCatchable(
            present_ball_pos,
            next_ball_pos,
            end_effector_pos
            );
    if (!is_vertically_catchable)
    {
        return false;
    }

    return true;
}

void EvalDropBallCatch::Configure(
        const std::shared_ptr<EvalDropBallPos> &eval_drop_ball_pos
        )
{
    eval_drop_ball_pos_ = eval_drop_ball_pos;
}

// protected func.

// private func.

bool EvalDropBallCatch::EvalHorizontallyCatchable(
        const math::Vector3d_t &end_effector_pos,
        const math::Vector3d_t &present_ball_pos,
        const double ball_radius
        ) const
{
    const math::Vector3d_t diff = end_effector_pos - present_ball_pos;
    const double horizontal_dist = diff.ProjectToXzPlane().norm();

    return horizontal_dist <= ball_radius ? true : false;
}

bool EvalDropBallCatch::EvalVerticallyCatchable(
        const math::Vector3d_t &present_ball_pos,
        const math::Vector3d_t &next_ball_pos,
        const math::Vector3d_t &end_effector_pos
        ) const
{
    bool is_vertically_catchable = false;
    if (present_ball_pos.y() >= next_ball_pos.y())
    {
        is_vertically_catchable
            = end_effector_pos.y() <= present_ball_pos.y()
            && end_effector_pos.y() >= next_ball_pos.y();
    }
    else
    {
        is_vertically_catchable
            = end_effector_pos.y() >= present_ball_pos.y()
            && end_effector_pos.y() <= next_ball_pos.y();
    }

    return is_vertically_catchable;
}

} // namespace kinematics {
