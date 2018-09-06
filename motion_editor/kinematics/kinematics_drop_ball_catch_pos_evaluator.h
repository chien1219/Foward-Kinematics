#ifndef _KINEMATICS_DROP_BALL_CATCH_POS_EVALUATOR_H_
#define _KINEMATICS_DROP_BALL_CATCH_POS_EVALUATOR_H_

#include "kinematics_def.h"
#include <memory>
#include "boost/optional.hpp"
#include "math_type.h"
#include "kinematics_fwd.h"

namespace kinematics {

class DropBallCatchPosEvaluator final
{

public:

    DropBallCatchPosEvaluator();
    DropBallCatchPosEvaluator(const DropBallCatchPosEvaluator &) = delete;
    virtual ~DropBallCatchPosEvaluator();
    DropBallCatchPosEvaluator &operator=(const DropBallCatchPosEvaluator &) = delete;

    int32_t target_body_idx() const;
    int32_t target_frame_idx() const;
    math::Vector3d_t target_pos() const;

    void Solve(
            const std::shared_ptr<ForwardSolver> &fk_solver,
            const int32_t target_body_idx,
            const int32_t target_frame_idx
            );

protected:

private:

    bool is_valid_input(
            const ForwardSolver &fk_solver,
            const int32_t target_body_idx
            ) const;

    std::shared_ptr<ForwardSolver> fk_solver_;
    boost::optional<int32_t> target_body_idx_;
    boost::optional<int32_t> target_frame_idx_;
    std::unique_ptr<math::Vector3d_t> target_pos_;
};

} // namespace kinematics {

#endif // #ifndef _KINEMATICS_DROP_BALL_CATCH_POS_EVALUATOR_H_
