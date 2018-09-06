#ifndef _KINEMATICS_EVAL_DROP_BALL_INIT_POS_H_
#define _KINEMATICS_EVAL_DROP_BALL_INIT_POS_H_

#include "kinematics_def.h"
#include "math_type.h"

namespace kinematics {

class EvalDropBallInitPos final
{

public:

    EvalDropBallInitPos();
    EvalDropBallInitPos(const EvalDropBallInitPos &other) = default;
    virtual ~EvalDropBallInitPos();
    EvalDropBallInitPos &operator=(const EvalDropBallInitPos &other) = default;

    math::Vector3dPair_t operator()(
            const math::Vector3d_t &target_pos,
            const math::Vector3d_t &drop_acc,
            const double desired_catch_time,
            const double time_step
            ) const;

protected:

private:

};

} // namespace kinematics {

#endif // #ifndef _KINEMATICS_EVAL_DROP_BALL_INIT_POS_H_
