#ifndef _KINEMATICS_TIME_WARPER_H_
#define _KINEMATICS_TIME_WARPER_H_

#include "kinematics_def.h"
#include <memory>
#include <utility>
#include "math_type.h"
#include "kinematics_time_warp_hard_constraint.h"

namespace kinematics {

class TimeWarper final
{

public:

    TimeWarper();
    TimeWarper(const TimeWarper &) = delete;
    virtual ~TimeWarper();
    TimeWarper &operator=(const TimeWarper &) = delete;

    double time_step() const;
    double min_time_step() const;
    double max_time_step() const;

    void Configure(
            const math::SpatialTemporalVector6d_t &original_motion_sequence,
            const double time_step,
            const double min_time_step,
            const double max_time_step
            );

    math::SpatialTemporalVector6d_t ComputeWarpedMotion(
            const TimeWarpHardConstraintColl_t &hard_constraint_coll
            );

	math::Quaternion_t getQuaternion(int joint_id, int frame_idx);
	math::Vector3d_t getGlobalPosition(int joint_id, int frame_idx);
	void firstStage(math::SpatialTemporalVector6d_t& STV, int drop_ball_catched_frame, int desired_catched_frame);
	void secondStage(math::SpatialTemporalVector6d_t& STV, int drop_ball_catched_frame, int desired_catched_frame);

protected:

private:

    std::unique_ptr<math::SpatialTemporalVector6d_t> original_motion_sequence_;
    std::unique_ptr<TimeWarpHardConstraintColl_t> hard_constraint_coll_;

    double time_step_;
    double min_time_step_;
    double max_time_step_;
};

} // namespace kinematics {

#endif // #ifndef _KINEMATICS_TIME_WARPER_H_
