#include "kinematics_drop_ball_catch_pos_evaluator.h"
#include "console_log.h"
#include "acclaim_skeleton.h"
#include "acclaim_motion.h"
#include "kinematics_forward_solver.h"
#include "kinematics_pose.h"

namespace kinematics {

// public func.

DropBallCatchPosEvaluator::DropBallCatchPosEvaluator()
    :fk_solver_(nullptr),
    target_body_idx_(boost::none),
    target_frame_idx_(boost::none),
    target_pos_(new math::Vector3d_t)
{
}

DropBallCatchPosEvaluator::~DropBallCatchPosEvaluator()
{
}

int32_t DropBallCatchPosEvaluator::target_body_idx() const
{
    return target_body_idx_.get();
}

int32_t DropBallCatchPosEvaluator::target_frame_idx() const
{
    return target_frame_idx_.get();
}

math::Vector3d_t DropBallCatchPosEvaluator::target_pos() const
{
    return *target_pos_;
}

void DropBallCatchPosEvaluator::Solve(
        const std::shared_ptr<ForwardSolver> &fk_solver,
        const int32_t target_body_idx,
        const int32_t target_frame_idx
        )
{
    if (!this->is_valid_input(*fk_solver, target_body_idx))
    {
        LOGERR << "invalid input" << std::endl;
        return;
    }

    fk_solver_ = fk_solver;
    target_body_idx_ = target_body_idx;
    target_frame_idx_ = target_frame_idx;

    const PoseColl_t pose_coll = fk_solver->ComputeSkeletonPose(target_frame_idx_.get());
    *target_pos_ = pose_coll[target_body_idx_.get()].end_pos();
}

// protected func.

// private func.

bool DropBallCatchPosEvaluator::is_valid_input(
        const ForwardSolver &fk_solver,
        const int32_t target_body_idx
        ) const
{
    if (!fk_solver.skeleton())
    {
        LOGERR << "skeleton in ForwradSolver is empty" << std::endl;
        return false;
    }

    if (!fk_solver.motion())
    {
        LOGERR << "motion in ForwardSolver is empty" << std::endl;
        return false;
    }

    const bool is_valid_body_idx
        = target_body_idx >= acclaim::Skeleton::root_idx()
        && target_body_idx < fk_solver.skeleton()->bone_num();

    if (!is_valid_body_idx)
    {
        LOGERR << "invalid body idx " << target_body_idx << std::endl;
        return false;
    }

    return true;
}

} // namespace kinematics {
