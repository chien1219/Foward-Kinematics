/*
   display.h

   Display the skeleton, ground plane and other objects.			

   Revision 1 - Steve Lin, Jan. 14, 2002
   Revision 2 - Alla and Kiran, Jan 18, 2002
   */

#ifndef _GUI_DSIPLAY_H_
#define _GUI_DISPLAY_H_

#include "gui_def.h"
#include <memory>
#include <vector>
#include "boost/ptr_container/ptr_container.hpp"
#include "param_fwd.h"
#include "math_type.h"
#include "acclaim_fwd.h"
#include "kinematics_fwd.h"
#include "geometry_fwd.h"
#include "gui_fwd.h"

namespace gui {

class Display final
{

public:
    /**
     * \brief Default constructor
     */
    Display();
    /**
     * \brief Forbid copy constructor
     */
    Display(const Display &) = delete;
    /**
     * \brief Destructor
     */
    virtual ~Display();
    /**
     * \brief Forbid assignment operator
     */
    Display &operator=(const Display &) = delete;
    /**
     * \brief
     * \return
     */
    int32_t spot_joint_idx() const;
    /**
     * \brief
     * \param[in] skeleton_idx
     */
    std::shared_ptr<acclaim::Skeleton> skeleton(const int32_t skeleton_idx) const;
    /**
     * \brief
     * \param[in] motion_idx
     * \return
     */
    std::shared_ptr<acclaim::Motion> motion(const int32_t motion_idx) const;
    /**
     * \brief
     */
    math::Vector6d_t skeleton_root_offset(const int32_t skeleton_idx) const;
    /**
     * \brief Get the number of actors
     * \return
     */
    int32_t skeleton_num() const;
    /**
     * \brief Get the number of motions
     * \return
     */
    int32_t motion_num() const;
    /**
     * \brief Get the frame number of the motion file indicated by a_klMotionIdx
     * \param[in] motion_idx
     * \return
     */
    int32_t frame_num(const int32_t motion_idx) const;
    /**
     * \brief Get the frame offset in the motion file indicated by a_klMotionIdx
     * \param[in] motion_idx
     * \return
     */
    int32_t frame_offset(const int32_t motion_idx) const;
    /**
     * \brief Get the remaining frame numbers after offset,
     * i.e. frame_num(i) - frame_offset(i), i is index of motion files
     * \param[in] motion_idx
     * \return
     */
    int32_t offset_frame_num(const int32_t motion_idx) const;
    /**
     * \brief
     * \param[in] motion_idx
     * \param[in] frame_idx
     */
    Posture posture(const int32_t motion_idx, const int32_t frame_idx);
    /**
     * \brief
     * \param[in] spot_joint_idx
     */
    void set_spot_joint_idx(const int32_t spot_joint_idx);
    /**
     * \brief
     * \param[in] frame_idx
     */
    void set_frame_idx(const int32_t frame_idx);
    /**
     * \brief
     * \param[in] skeleton_idx
     * \param[in] posture
     */
    //void set_posture(const int32_t skeleton_idx, const Posture &posture);
    /**
     * \brief
     */
    void set_skeleton_root_offset(
            const int32_t skeleton_idx,
            const math::Vector6d_t &root_offset
            );
    /**
     * \brief
     */
    void Configure(
            const std::shared_ptr<param::Config> &param,
            const std::shared_ptr<Renderer> &renderer
            );
    /**
     * \brief Set actor for display
     * \param[in] skeleton
     */
    void LoadSkeleton(const std::shared_ptr<acclaim::Skeleton> &skeleton);
    /**
     * \brief Set motion for display
     * \param[in] motion
     */
    void LoadMotion(const std::shared_ptr<acclaim::Motion> &motion);
    /**
     * \brief Display the scene (actor, ground plane ....)
     */
    void Show();
    /**
     * \brief
     */
    void Save();
    /**
     * \brief
     */
    void Reset();
    /**
     * \brief
     * \param[in] ik_target
     */
    void SolveIk(const math::Vector3d_t &ik_target);
    /**
     * \brief
     */
    void SolveTimeWarp();
    /**
     * \brief
     */
    void Slerp();
    /**
     * \brief
     */
    void CleanFootskate();
    /**
     * \brief
     * \param[in] motion_idx
     */
    bool is_empty_motion(const int32_t motion_idx) const;

private:

    typedef std::vector<std::shared_ptr<acclaim::Skeleton>> SkeletonColl_t;
    typedef std::vector<std::shared_ptr<acclaim::Motion>> MotionColl_t;
    typedef std::vector<std::shared_ptr<kinematics::ForwardSolver>> FkSolverColl_t;

    static const int32_t skeleton_num_hint()
    {return int32_t{16};}
    static const int32_t motion_num_hint()
    {return int32_t{16};}
    /**
     * \brief Compute FK and draw the specified character
     * \param[in] skeleton_idx
     */
    void ShowSkeleton(const int32_t skeleton_idx);

    int32_t frame_idx_;             //!< current playing frame
    int32_t spot_joint_idx_;		//!< joint whose local coordinate system is drawn

    std::unique_ptr<SkeletonColl_t> skeleton_coll_;
    std::unique_ptr<MotionColl_t> motion_coll_;
    std::unique_ptr<FkSolverColl_t> fk_solver_coll_;
    std::unique_ptr<math::Vector6dColl_t> skeleton_root_offset_coll_;

    std::unique_ptr<gui::Ball> drop_ball_;
    std::unique_ptr<kinematics::EvalDropBallCatch> eval_drop_ball_catch_;
    std::unique_ptr<kinematics::EvalDropBallCatchTime> eval_drop_ball_catch_time_;
    std::unique_ptr<kinematics::EvalDropBallInitPos> eval_drop_ball_init_pos_;
    std::shared_ptr<kinematics::EvalDropBallPos> eval_drop_ball_pos_;
    std::unique_ptr<kinematics::DropBallCatchPosEvaluator> drop_ball_catch_pos_evaluator_;
    std::unique_ptr<kinematics::TimeWarper> time_warper_;
    std::unique_ptr<math::SpatialTemporalVector6d_t> warped_motion_sequence_;
    std::unique_ptr<ConvertSimulationTimeToFrameIdx> convert_simulation_time_to_frame_idx_;

    std::shared_ptr<param::Config> param_;
    std::shared_ptr<Renderer> renderer_;
    std::unique_ptr<SkeletonColor> skeleton_color_;
};

} // namespace gui {

#endif // #ifndef _GUI_DSIPLAY_H_
