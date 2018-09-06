#include "gui_display.h"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream> 
#include <iomanip>
#include "FL/glut.H"
#include "FL/gl.h"
#include "boost/numeric/conversion/cast.hpp"
#include "console_log.h"
#include "param_config.h"
#include "math_utils.h"
#include "acclaim_skeleton.h"
#include "acclaim_motion.h"
#include "geometry_sphere.h"
#include "kinematics_eval_drop_ball_catch.h"
#include "kinematics_eval_drop_ball_catch_time.h"
#include "kinematics_eval_drop_ball_init_pos.h"
#include "kinematics_eval_drop_ball_pos.h"
#include "kinematics_drop_ball_catch_pos_evaluator.h"
#include "kinematics_forward_solver.h"
#include "kinematics_pose.h"
#include "kinematics_time_warper.h"
#include "gui_utils.h"
#include "gui_color.h"
#include "gui_ball.h"
#include "gui_convert_simulation_time_to_frame_idx.h"
#include "gui_renderer.h"
#include "gui_skeleton_color.h"

namespace gui {

Display::Display()
    :frame_idx_(int32_t{0}),
    spot_joint_idx_(int32_t{-1}),
    skeleton_coll_(new SkeletonColl_t),
    motion_coll_(new MotionColl_t),
    fk_solver_coll_(new FkSolverColl_t),
    skeleton_root_offset_coll_(new math::Vector6dColl_t),
    drop_ball_(new gui::Ball),
    eval_drop_ball_catch_(new kinematics::EvalDropBallCatch),
    eval_drop_ball_catch_time_(new kinematics::EvalDropBallCatchTime),
    eval_drop_ball_init_pos_(new kinematics::EvalDropBallInitPos),
    eval_drop_ball_pos_(new kinematics::EvalDropBallPos),
    drop_ball_catch_pos_evaluator_(new kinematics::DropBallCatchPosEvaluator),
    time_warper_(new kinematics::TimeWarper),
    warped_motion_sequence_(nullptr),
    convert_simulation_time_to_frame_idx_(new ConvertSimulationTimeToFrameIdx),
    param_(nullptr),
    renderer_(nullptr),
    skeleton_color_(new SkeletonColor)
{
    skeleton_coll_->reserve(Display::skeleton_num_hint());
    motion_coll_->reserve(Display::motion_num_hint());
    fk_solver_coll_->reserve(Display::skeleton_num_hint());
    skeleton_root_offset_coll_->reserve(Display::skeleton_num_hint());
}

Display::~Display()
{
}

int32_t Display::spot_joint_idx() const
{
    return spot_joint_idx_;
}

std::shared_ptr<acclaim::Skeleton> Display::skeleton(const int32_t skeleton_idx) const
{
    return (*skeleton_coll_)[skeleton_idx];
}

std::shared_ptr<acclaim::Motion> Display::motion(const int32_t motion_idx) const
{
    return (*motion_coll_)[motion_idx];
}

math::Vector6d_t Display::skeleton_root_offset(const int32_t skeleton_idx) const
{
    return skeleton_root_offset_coll_->at(skeleton_idx);
}

int32_t Display::skeleton_num() const
{
    return boost::numeric_cast<int32_t>(skeleton_coll_->size());
}

int32_t Display::motion_num() const
{
    return boost::numeric_cast<int32_t>(motion_coll_->size());
}

int32_t Display::frame_num(const int32_t motion_idx) const
{
    return motion_coll_->at(motion_idx)->frame_num();
}

int32_t Display::frame_offset(const int32_t motion_idx) const
{
    return motion_coll_->at(motion_idx)->frame_offset();
}

int32_t Display::offset_frame_num(const int32_t motion_idx) const
{
    return this->frame_num(motion_idx) - this->frame_offset(motion_idx);
}

Posture Display::posture(const int32_t motion_idx, const int32_t frame_idx)
{
    const std::shared_ptr<acclaim::Motion> target_motion = motion_coll_->at(motion_idx);
    return *(target_motion->posture_ptr(target_motion->posture_idx(frame_idx)));
}

void Display::set_spot_joint_idx(const int32_t spot_joint_idx)
{
    spot_joint_idx_ = spot_joint_idx;
}

void Display::set_frame_idx(const int32_t frame_idx)
{
    frame_idx_ = frame_idx;
}

void Display::set_skeleton_root_offset(
        const int32_t skeleton_idx,
        const math::Vector6d_t &root_offset
        )
{
    (*skeleton_root_offset_coll_)[skeleton_idx] = root_offset;
}

void Display::Configure(
        const std::shared_ptr<param::Config> &param,
        const std::shared_ptr<Renderer> &renderer
        )
{
    param_ = param;
    renderer_ = renderer;

    static const ColorColl_t skeleton_color_coll =
    {
        Color_t(1.0f,  1.0f, 0.1f, 0.9f), // yellow
        Color_t(1.0f, 0.54f, 0.1f, 0.9f), // orange
        Color_t(1.0f,  0.2f, 0.2f, 1.0f), // red
        Color_t(0.2f,  1.0f, 0.2f, 1.0f), // green
        Color_t(0.2f,  0.2f, 1.0f, 1.0f), // blue
    };

    skeleton_color_->Configure(skeleton_color_coll);
}

void Display::LoadSkeleton(const std::shared_ptr<acclaim::Skeleton> &skeleton)
{
    skeleton_coll_->push_back(skeleton);
    std::shared_ptr<kinematics::ForwardSolver> fk_solver(new kinematics::ForwardSolver);
    fk_solver_coll_->push_back(fk_solver);
    fk_solver_coll_->back()->set_skeleton(skeleton_coll_->back());
    fk_solver_coll_->back()->ConstructArticPath();

    skeleton_root_offset_coll_->push_back(math::Vector6d_t::Zero());
}

void Display::LoadMotion(const std::shared_ptr<acclaim::Motion> &motion)
{
    motion_coll_->push_back(motion);
}

void Display::Show()
{
    glPushMatrix();

    for (int32_t skeleton_idx = 0; skeleton_idx < this->skeleton_num(); ++skeleton_idx)
    {
        renderer_->set_translation(
                skeleton_root_offset_coll_->at(skeleton_idx).linear_vector() * acclaim::MoCapScale()
                );
        renderer_->set_rotation(
                skeleton_root_offset_coll_->at(skeleton_idx).angular_vector().x(), math::UnitX()
                );
        renderer_->set_rotation(
                skeleton_root_offset_coll_->at(skeleton_idx).angular_vector().y(), math::UnitY()
                );
        renderer_->set_rotation(
                skeleton_root_offset_coll_->at(skeleton_idx).angular_vector().z(), math::UnitZ()
                );

        this->ShowSkeleton(skeleton_idx);
    } // for (int32_t skeleton_idx = 0; skeleton_idx < this->actorNum(); ++skeleton_idx)

    glPopMatrix();
}

void Display::Save()
{
    motion_coll_->at(0)->OutputAmcFile(
            "save.amc",
            acclaim::MoCapScale()
            );
}

void Display::Reset()
{
    skeleton_coll_->clear();
    motion_coll_->clear();
    skeleton_root_offset_coll_->clear();
    fk_solver_coll_->clear();
    skeleton_coll_->reserve(Display::skeleton_num_hint());
    motion_coll_->reserve(Display::motion_num_hint());
    skeleton_root_offset_coll_->reserve(Display::skeleton_num_hint());

    drop_ball_->set_catched(false);
    warped_motion_sequence_.reset();
}

void Display::SolveIk(const math::Vector3d_t &ik_target)
{
}

void Display::SolveTimeWarp()
{
    drop_ball_catch_pos_evaluator_->Solve(
            fk_solver_coll_->at(0),
            Body::rfingers,
            param_->value<int32_t>("time_warp.drop_ball_catch_frame_idx")
            );
    const double desired_catch_time
        = param_->value<double>("time_warp.desired_catch_frame_idx")
        * acclaim::Motion::time_step();
    math::Vector3dPair_t drop_ball_init_pos = (*eval_drop_ball_init_pos_)(
            drop_ball_catch_pos_evaluator_->target_pos(),
            param_->value<math::Vector3d_t>("time_warp.drop_ball_acc"),
            desired_catch_time,
            acclaim::Motion::time_step()
            );
    eval_drop_ball_pos_->Configure(
            acclaim::Motion::time_step(),
            double{0.5} * (drop_ball_init_pos.first + drop_ball_init_pos.second),
            param_->value<math::Vector3d_t>("time_warp.drop_ball_acc")
            );
    eval_drop_ball_catch_->Configure(eval_drop_ball_pos_);
    const double drop_ball_catch_time = (*eval_drop_ball_catch_time_)(
            eval_drop_ball_pos_->init_pos(),
            drop_ball_catch_pos_evaluator_->target_pos(),
            eval_drop_ball_pos_->drop_acc()
            );
    const int32_t drop_ball_catch_frame_idx = (*convert_simulation_time_to_frame_idx_)(
            acclaim::Motion::time_step(),
            drop_ball_catch_time
            );

    drop_ball_->set_collision_radius(
            param_->value<double>("time_warp.drop_ball_collision_radius")
            );
    drop_ball_->set_visulation_radius(
            param_->value<double>("time_warp.drop_ball_visulation_radius")
            );
    drop_ball_->set_color(
            param_->value<Color_t>("time_warp.drop_ball_color")
            );
    drop_ball_->set_pos(
            drop_ball_catch_pos_evaluator_->target_pos()
            );

    const double min_time_step = double{0.0};
    const double max_time_step = double{0.0};
    time_warper_->Configure(
            fk_solver_coll_->at(0)->motion()->whole_sequence(),
            acclaim::Motion::time_step(),
            min_time_step,
            max_time_step
            );

    const kinematics::TimeWarpHardConstraintColl_t time_warp_hard_constraint_coll =
    {
        kinematics::TimeWarpHardConstraint_t(int32_t{0}, double{0.0}),
        kinematics::TimeWarpHardConstraint_t(
                param_->value<int32_t>("time_warp.desired_catch_frame_idx"),
                param_->value<double>("time_warp.drop_ball_catch_frame_idx")
                * acclaim::Motion::time_step()
                ),
        kinematics::TimeWarpHardConstraint_t(
                (fk_solver_coll_->at(0)->motion()->frame_num() - 1),
                boost::numeric_cast<double>((fk_solver_coll_->at(0)->motion()->frame_num() - 1))
                * acclaim::Motion::time_step()
                ),
    };

    warped_motion_sequence_.reset(new math::SpatialTemporalVector6d_t);
    *warped_motion_sequence_ = time_warper_->ComputeWarpedMotion(
            time_warp_hard_constraint_coll
            );
}

void Display::Slerp()
{
}

void Display::CleanFootskate()
{
}

bool Display::is_empty_motion(const int32_t motion_idx) const
{
    return motion_idx < boost::numeric_cast<int32_t>(motion_coll_->size()) ? FALSE : TRUE;
}

// private func.

void Display::ShowSkeleton(const int32_t skeleton_idx)
{
    int32_t fk_frame_idx = 0;
    kinematics::PoseColl_t fk_pose_coll;

    if (skeleton_idx == this->motion_num()) // the actor has no corresponding AMC file -> draw T-pose
    {
        std::shared_ptr<acclaim::Motion> default_motion(
                new acclaim::Motion(
                    (*skeleton_coll_)[skeleton_idx],
                    fk_frame_idx + 1
                    )
                );
        fk_solver_coll_->at(skeleton_idx)->set_motion(default_motion);
        fk_pose_coll = fk_solver_coll_->at(skeleton_idx)->ComputeSkeletonPose(fk_frame_idx);
    }
    else
    {
        const int32_t max_frame_idx = motion_coll_->at(skeleton_idx)->frame_num() - 1;
        if (frame_idx_ > max_frame_idx)
        {
            //current frameIdx > maxFrame of the motion file
            fk_frame_idx = max_frame_idx;
        }
        else
        {
            fk_frame_idx = frame_idx_;
        }
        fk_solver_coll_->at(skeleton_idx)->set_motion(motion_coll_->at(skeleton_idx));
        fk_pose_coll = fk_solver_coll_->at(skeleton_idx)->ComputeSkeletonPose(fk_frame_idx);
    }

    //draw all bones
    math::Vector3d_t bone_start_pos, bone_end_pos;
    for (int32_t bone_idx = 0; bone_idx < skeleton_coll_->at(skeleton_idx)->bone_num(); ++bone_idx)
    {
        bone_start_pos = fk_pose_coll[bone_idx].start_pos();
        bone_end_pos = fk_pose_coll[bone_idx].end_pos();
        
        //draw bone
        if (warped_motion_sequence_)
        {
            Color_t modified_color = (*skeleton_color_)(skeleton_idx);
            modified_color.set_alpha(
                    param_->value<float>("time_warp.skeleton_light_alpha")
                    );
            renderer_->set_color(modified_color);
        }
        else
        {
            renderer_->set_color((*skeleton_color_)(skeleton_idx));
        }
        double bone_radius = param_->value<double>("bone_radius");
        renderer_->DrawCylinder(
                fk_pose_coll[bone_idx].start_pos(),
                fk_pose_coll[bone_idx].end_pos(),
                bone_radius
                );
        math::Vector3d_t axis_x = fk_pose_coll[bone_idx].rotation().col(math::X_id);
        math::Vector3d_t axis_y = fk_pose_coll[bone_idx].rotation().col(math::Y_id);
        math::Vector3d_t axis_z = fk_pose_coll[bone_idx].rotation().col(math::Z_id);		
        
        double axis_scale = param_->value<double>("axis_scale");
        if (bone_idx == spot_joint_idx_)
        {
            renderer_->set_color(gui::Red());
            renderer_->DrawLine(bone_start_pos, math::Vector3d_t(bone_start_pos + axis_scale * axis_x));
            renderer_->set_color(gui::Green());
            renderer_->DrawLine(bone_start_pos, math::Vector3d_t(bone_start_pos + axis_scale * axis_y));
            renderer_->set_color(gui::Blue());
            renderer_->DrawLine(bone_start_pos, math::Vector3d_t(bone_start_pos + axis_scale * axis_z));
        }

        if (warped_motion_sequence_)
        {
            const int32_t max_warped_frame_idx = warped_motion_sequence_->temporal_size() - 1;
            int32_t warped_frame_idx = 0;
            if (frame_idx_ > max_warped_frame_idx)
            {
                warped_frame_idx = max_warped_frame_idx;
            }
            else
            {
                warped_frame_idx = frame_idx_;
            }

            const kinematics::PoseColl_t warped_pose_coll
                = fk_solver_coll_->at(0)->ComputeSkeletonPose(
                        warped_motion_sequence_->temporal_elements(warped_frame_idx)
                        );
            renderer_->set_color(param_->value<Color_t>("time_warp.skeleton_color"));
            const double bone_radius = param_->value<double>("bone_radius");
            for (auto pose = warped_pose_coll.begin();
                    pose != warped_pose_coll.end();
                    ++pose)
            {
                renderer_->DrawCylinder(
                        pose->start_pos(),
                        pose->end_pos(),
                        bone_radius
                        );
            }

            if (!drop_ball_->is_catched())
            {
                const bool is_ball_catched = (*eval_drop_ball_catch_)(
                        frame_idx_,
                        drop_ball_->collision_radius(),
                        warped_pose_coll[Body::rfingers].end_pos()
                        );
                drop_ball_->set_catched(is_ball_catched);
            }

            if (!drop_ball_->is_catched())
            {
                drop_ball_->set_pos((*eval_drop_ball_pos_)(frame_idx_));
            }
            else
            {
                drop_ball_->set_pos(
                        warped_pose_coll[Body::rfingers].end_pos()
                        );
            }
            renderer_->set_color(drop_ball_->color());
            renderer_->DrawSphere(
                    drop_ball_->pos(),
                    drop_ball_->visulation_radius()
                    );

            //renderer_->set_color(param_->value<Color_t>("time_warp.end_effector_color"));
            //renderer_->DrawSphere(
            //        warped_pose_coll[Body::rfingers].end_pos(),
            //        param_->value<double>("time_warp.end_effector_radius")
            //        );
        }
    } // for (int32_t bone_idx = 0; bone_idx < skeleton_coll_->at(skeleton_idx)->bone_num(); ++bone_idx)
}

} // namespace gui {
