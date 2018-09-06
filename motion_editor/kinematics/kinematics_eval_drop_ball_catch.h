#ifndef _KINEMATICS_EVAL_DROP_BALL_CATCH_H_
#define _KINEMATICS_EVAL_DROP_BALL_CATCH_H_

#include "kinematics_def.h"
#include <memory>
#include "math_type.h"
#include "kinematics_fwd.h"

namespace kinematics {

class EvalDropBallCatch final
{

public:

    EvalDropBallCatch();
    EvalDropBallCatch(const EvalDropBallCatch &other) = delete;
    virtual ~EvalDropBallCatch();
    EvalDropBallCatch &operator=(const EvalDropBallCatch &other) = delete;

    bool operator()(
            const int32_t frame_idx,
            const double ball_radius,
            const math::Vector3d_t &end_effector_pos
            ) const;
    void Configure(
            const std::shared_ptr<EvalDropBallPos> &eval_drop_ball_pos
            );

protected:

private:

    bool EvalHorizontallyCatchable(
            const math::Vector3d_t &end_effector_pos,
            const math::Vector3d_t &present_ball_pos,
            const double ball_radius
            ) const;
    bool EvalVerticallyCatchable(
            const math::Vector3d_t &present_ball_pos,
            const math::Vector3d_t &next_ball_pos,
            const math::Vector3d_t &end_effector_pos
            ) const;

    std::shared_ptr<EvalDropBallPos> eval_drop_ball_pos_;
};

} // namespace kinematics {

#endif // #ifndef _KINEMATICS_EVAL_DROP_BALL_CATCH_H_
