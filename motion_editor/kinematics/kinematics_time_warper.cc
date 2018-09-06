#include "kinematics_time_warper.h"
#include <utility>
#include "boost/numeric/conversion/cast.hpp"
#include "math_utils.h"

using namespace std;

namespace kinematics {

// public func.

TimeWarper::TimeWarper()
    :original_motion_sequence_(new math::SpatialTemporalVector6d_t),
    hard_constraint_coll_(new TimeWarpHardConstraintColl_t),
    time_step_(double{0.0}),
    min_time_step_(double{0.0}),
    max_time_step_(double{0.0})
{
}

TimeWarper::~TimeWarper()
{
}

double TimeWarper::time_step() const
{
    return time_step_;
}

double TimeWarper::min_time_step() const
{
    return min_time_step_;
}

double TimeWarper::max_time_step() const
{
    return max_time_step_;
}

void TimeWarper::Configure(
        const math::SpatialTemporalVector6d_t &original_motion_sequence,
        const double time_step,
        const double min_time_step,
        const double max_time_step
        )
{
    *original_motion_sequence_ = original_motion_sequence;
    time_step_ = time_step;
    min_time_step_ = min_time_step;
    max_time_step_ = max_time_step;
}

math::SpatialTemporalVector6d_t TimeWarper::ComputeWarpedMotion(
        const TimeWarpHardConstraintColl_t &hard_constraint_coll
        )
{
    // TO DO
    *hard_constraint_coll_ = hard_constraint_coll;

	math::SpatialTemporalVector6d_t newSTV = *original_motion_sequence_;
	int desired_catched_frame = hard_constraint_coll.at(1).frame_idx;	//param_->value<int32_t>("time_warp.desired_catch_frame_idx");
	int drop_ball_catched_frame = hard_constraint_coll.at(1).play_second / time_step();

	cout << "Time warping calculatin...Please wait!" << endl;

	firstStage(newSTV, drop_ball_catched_frame, desired_catched_frame);
	secondStage(newSTV, drop_ball_catched_frame, desired_catched_frame);
		
	cout << "Time Warping Calculation Finished!" << endl;
	return newSTV;
    //return *original_motion_sequence_;
}

math::Quaternion_t TimeWarper::getQuaternion(int joint_id, int frame_idx){
	
	math::Vector6d_t Vec6 = original_motion_sequence_->temporal_elements(frame_idx)[joint_id];
	return math::ComputeQuaternionXyz(math::ToRadian(Vec6[0]), math::ToRadian(Vec6[1]), math::ToRadian(Vec6[2]));
}

math::Vector3d_t TimeWarper::getGlobalPosition(int joint_id, int frame_idx){
	
	math::Vector6d_t Vec6 = original_motion_sequence_->temporal_elements(frame_idx)[joint_id];
	return math::Vector3d_t(Vec6[3], Vec6[4], Vec6[5]);
}

void TimeWarper::firstStage(math::SpatialTemporalVector6d_t& STV, int drop_ball_catched_frame, int desired_catched_frame){
	//For every joint
	for (int i = 0; i < original_motion_sequence_->spatial_size(); i++){

		double ratioLeft = (double)drop_ball_catched_frame / (double)desired_catched_frame;

		//For Frame in left Part
		for (int j = 1; j <= desired_catched_frame; j++){

			double correPoint = ratioLeft * j;
			int frame1_idx = floor(correPoint);
			int frame2_idx = ceil(correPoint);
			double newRatio = correPoint - (double)frame1_idx;

			math::Quaternion_t Q1 = getQuaternion(i, frame1_idx);
			math::Quaternion_t Q2 = getQuaternion(i, frame2_idx);
			math::Quaternion_t Qresult = math::Slerp(Q1, Q2, newRatio);

			math::Vector3d_t v = math::ToDegree(math::ComputeEulerAngleXyz(Qresult.toRotationMatrix()));

			math::Vector3d_t p1 = getGlobalPosition(i, frame1_idx);
			math::Vector3d_t p2 = getGlobalPosition(i, frame2_idx);

			math::Vector3d_t p = p1 *  (1 - newRatio) + p2 * newRatio;

			STV.set_element(i, j, math::Vector6d_t(v, p));
		}
	}
}


void TimeWarper::secondStage(math::SpatialTemporalVector6d_t& STV, int drop_ball_catched_frame, int desired_catched_frame){
	//For every joint
	for (int i = 0; i < original_motion_sequence_->spatial_size(); i++){
		double ratioRight = (original_motion_sequence_->temporal_size() - (double)drop_ball_catched_frame) / (original_motion_sequence_->temporal_size() - (double)desired_catched_frame);

		//For Frame in Right Part
		for (int j = desired_catched_frame + 1; j < original_motion_sequence_->temporal_size(); j++){

			double correPoint = ratioRight * (j - desired_catched_frame) + drop_ball_catched_frame;
			int frame1_idx = floor(correPoint);
			int frame2_idx = ceil(correPoint);


			if (frame2_idx >= original_motion_sequence_->temporal_size())
				frame2_idx = original_motion_sequence_->temporal_size() - 1;

			double newRatio = correPoint - (double)frame1_idx;

			math::Quaternion_t Q1 = getQuaternion(i, frame1_idx);
			math::Quaternion_t Q2 = getQuaternion(i, frame2_idx);
			math::Quaternion_t Qresult = math::Slerp(Q1, Q2, newRatio);

			math::Vector3d_t v = math::ToDegree(math::ComputeEulerAngleXyz(Qresult.toRotationMatrix()));

			math::Vector3d_t p1 = getGlobalPosition(i, frame1_idx);
			math::Vector3d_t p2 = getGlobalPosition(i, frame2_idx);

			math::Vector3d_t p = p1 *  (1 - newRatio) + p2 * newRatio;

			STV.set_element(i, j, math::Vector6d_t(v, p));
		}
	}
}


// protected func.

// private func.

} // namespace kinematics {
