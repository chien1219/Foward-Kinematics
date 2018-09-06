#include "kinematics_eval_drop_ball_pos.h"
#include "math_utils.h"

namespace kinematics {

// public func.

EvalDropBallPos::EvalDropBallPos()
    :time_step_(double{0.0}),
    init_pos_(new math::Vector3d_t),
    drop_acc_(new math::Vector3d_t)
{
}

EvalDropBallPos::EvalDropBallPos(const EvalDropBallPos &other)
    :time_step_(other.time_step()),
    init_pos_(new math::Vector3d_t(other.init_pos())),
    drop_acc_(new math::Vector3d_t(other.drop_acc()))
{
}

EvalDropBallPos::~EvalDropBallPos()
{
}

EvalDropBallPos &EvalDropBallPos::operator=(const EvalDropBallPos &other)
{
    if (&other == this)
    {
        return *this;
    }

    time_step_ = other.time_step();
    *init_pos_ = other.init_pos();
    *drop_acc_ = other.drop_acc();

    return *this;
}

math::Vector3d_t EvalDropBallPos::operator()(const int32_t frame_idx) const
{
    const double drop_time = this->time_step() * boost::numeric_cast<double>(frame_idx);
    return this->init_pos()
         + math::Vector3d_t::RealScalar{0.5} * this->drop_acc() * math::Square(drop_time);
}

double EvalDropBallPos::time_step() const
{
    return time_step_;
}

math::Vector3d_t EvalDropBallPos::init_pos() const
{
    return *init_pos_;
}

math::Vector3d_t EvalDropBallPos::drop_acc() const
{
    return *drop_acc_;
}

void EvalDropBallPos::Configure(
        const double time_step,
        const math::Vector3d_t &init_pos,
        const math::Vector3d_t &drop_acc
        )
{
    time_step_ = time_step;
    *init_pos_ = init_pos;
    *drop_acc_ = drop_acc;
}

// protected func.

// private func.

} // namespace kinematics {
