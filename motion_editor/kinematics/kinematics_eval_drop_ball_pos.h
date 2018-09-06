#ifndef _KINEMATICS_EVAL_DROP_BALL_POS_H_
#define _KINEMATICS_EVAL_DROP_BALL_CATCH_H_

#include "kinematics_def.h"
#include <memory>
#include "math_type.h"

namespace kinematics {

class EvalDropBallPos final
{

public:

    EvalDropBallPos();
    EvalDropBallPos(const EvalDropBallPos &other);
    virtual ~EvalDropBallPos();
    EvalDropBallPos &operator=(const EvalDropBallPos &other);

    math::Vector3d_t operator()(const int32_t frame_idx) const;

    double time_step() const;
    math::Vector3d_t init_pos() const;
    math::Vector3d_t drop_acc() const;

    void Configure(
            const double time_step_,
            const math::Vector3d_t &init_pos,
            const math::Vector3d_t &drop_acc
            );

protected:

private:

    double time_step_;
    std::unique_ptr<math::Vector3d_t> init_pos_;
    std::unique_ptr<math::Vector3d_t> drop_acc_;
};

} // namespace kinematics {

#endif // #ifndef _KINEMATICS_EVAL_DROP_BALL_POS_H_
