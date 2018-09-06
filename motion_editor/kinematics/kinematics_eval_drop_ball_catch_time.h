#ifndef _KINEMATICS_EVAL_DROP_BALL_CATCH_TIME_H_
#define _KINEMATICS_EVAL_DROP_BALL_CATCH_TIME_H_

#include "kinematics_def.h"
#include "math_type.h"

namespace kinematics {

class EvalDropBallCatchTime final
{

public:

    EvalDropBallCatchTime();
    EvalDropBallCatchTime(const EvalDropBallCatchTime &other) = default;
    virtual ~EvalDropBallCatchTime();
    EvalDropBallCatchTime &operator=(const EvalDropBallCatchTime &other) = default;

    double operator()(
            const math::Vector3d_t &init_pos,
            const math::Vector3d_t &target_pos,
            const math::Vector3d_t &drop_acc
            ) const;

protected:

private:

};

} // namespace kinematics {

#endif // #ifndef _KINEMATICS_EVAL_DROP_BALL_CATCH_TIME_H_
