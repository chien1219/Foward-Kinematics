#include "kinematics_forward_solver.h"
#include <algorithm>
#include "math_utils.h"
#include "acclaim_skeleton.h"
#include "acclaim_motion.h"
#include "helper_forward_kinematics.h"
#include "kinematics_artic_idx.h"
#include "kinematics_pose.h"

using namespace std;

namespace kinematics {

	// public func.

	ForwardSolver::ForwardSolver()
		:skeleton_(nullptr),
		motion_(nullptr),
		artic_path_(new ArticIdxColl_t),
		helper_fk_(new helper::ForwardKinematics)
	{
	}

	ForwardSolver::~ForwardSolver()
	{
	}

	std::shared_ptr<acclaim::Skeleton> ForwardSolver::skeleton() const
	{
		return skeleton_;
	}

	std::shared_ptr<acclaim::Motion> ForwardSolver::motion() const
	{
		return motion_;
	}

	void ForwardSolver::set_skeleton(const std::shared_ptr<acclaim::Skeleton> &skeleton)
	{
		skeleton_ = skeleton;
		helper_fk_->set_skeleton(skeleton_);
	}

	void ForwardSolver::set_motion(const std::shared_ptr<acclaim::Motion> &motion)
	{
		motion_ = motion;
		helper_fk_->set_motion(motion_);
	}

	void ForwardSolver::ConstructArticPath()
	{
		ArticIdxColl_t ArtiColl;
		ArticIdx *ar = new ArticIdx(0);
		artic_path_->push_back(*ar);

		//lfoot
		for (int i = 1; i <= 5; i++){
			ar = new ArticIdx(i);
			ar->set_parent_idx(i - 1);
			artic_path_->push_back(*ar);
		}
		//rfoot
		for (int i = 6; i <= 10; i++){
			ar = new ArticIdx(i);
			if (i == 6)
				ar->set_parent_idx(0);
			else
				ar->set_parent_idx(i - 1);
			artic_path_->push_back(*ar);
		}
		//body & head
		for (int i = 11; i <= 16; i++){
			ar = new ArticIdx(i);
			if (i == 11)
				ar->set_parent_idx(0);
			else
				ar->set_parent_idx(i - 1);
			artic_path_->push_back(*ar);
		}

		//larm
		for (int i = 17; i <= 22; i++){
			ar = new ArticIdx(i);
			if (i == 17)
				ar->set_parent_idx(13);
			else
				ar->set_parent_idx(i - 1);
			artic_path_->push_back(*ar);
		}
		ar = new ArticIdx(23);
		ar->set_parent_idx(20);
		artic_path_->push_back(*ar);

		//rarm
		for (int i = 24; i <= 29; i++){
			ar = new ArticIdx(i);
			if (i == 24)
				ar->set_parent_idx(13);
			else
				ar->set_parent_idx(i - 1);
			artic_path_->push_back(*ar);
		}
		ar = new ArticIdx(30);
		ar->set_parent_idx(27);
		artic_path_->push_back(*ar);

		//helper_fk_->ConstructArticPath();
	}

	PoseColl_t ForwardSolver::ComputeSkeletonPose(const int32_t frame_idx)
	{
		return this->ComputeSkeletonPose(motion_->joint_spatial_pos(frame_idx));
	}

	PoseColl_t ForwardSolver::ComputeSkeletonPose(const math::Vector6dColl_t &joint_spatial_pos)
	{
		// TO DO
		PoseColl_t newPosColl;
		Pose lastPos;
		math::Quaternion_t accQua;

		//Root Position
		lastPos.set_start_pos(math::Vector3d_t(joint_spatial_pos.at(0).linear_vector()));
		lastPos.set_end_pos(lastPos.start_pos());

		newPosColl.push_back(lastPos);

		for (int i = 1; i < joint_spatial_pos.size(); i++){

			Pose newPos;			
			lastPos = newPosColl.at(artic_path_->at(i).parent_idx().value());
			newPos.set_start_pos(lastPos.end_pos());
			accQua = computeCommulativeQ(i, joint_spatial_pos);

			math::Quaternion_t Qtmp;
			Qtmp.w() = 0;
			Qtmp.vec() = skeleton()->bone_ptr(i)->dir;

			//v' = QvQ
			Qtmp = accQua * Qtmp * accQua.inverse();
						
			newPos.set_end_pos(newPos.start_pos() + Qtmp.vec() * skeleton()->bone_ptr(i)->length);

			newPosColl.push_back(newPos);
		}

		//return helper_fk_->ComputeSkeletonPose(joint_spatial_pos);
		return newPosColl;
	}

	math::Quaternion_t ForwardSolver::computeCommulativeQ(int id, const math::Vector6dColl_t &joint_spatial_pos){

		if (id == 0){
			return  computeQasf(0) * computeQamc(0, joint_spatial_pos) ;
		}
		else{
			int parentID = artic_path_->at(id).parent_idx().value();
			return computeCommulativeQ(parentID, joint_spatial_pos) * computeQasf(id) * computeQamc(id, joint_spatial_pos);
		}
	}

	math::Quaternion_t ForwardSolver::computeQamc(int id, const math::Vector6dColl_t &joint_spatial_pos){
		return math::ComputeQuaternionXyz(math::ToRadian(joint_spatial_pos[id][0]), math::ToRadian(joint_spatial_pos[id][1]), math::ToRadian(joint_spatial_pos[id][2]));
	}

	math::Quaternion_t ForwardSolver::computeQasf(int id){
		return math::Quaternion_t(math::ToRotMat(skeleton()->bone_ptr(id)->rot_parent_current).transpose());
	}
	// protected func.

	// private func.

} // namespace kinematics {
