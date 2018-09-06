#include "gui_ball.h"
#include "gui_color.h"

namespace gui {

// public func.

Ball::Ball()
    :collision_radius_(double{0.0}),
    visulation_radius_(double{0.0}),
    pos_(new math::Vector3d_t),
    color_(new Color_t),
    is_catched_(false)
{
}

Ball::Ball(const Ball &other)
    :collision_radius_(other.collision_radius()),
    visulation_radius_(other.visulation_radius()),
    pos_(new math::Vector3d_t(other.pos())),
    color_(new Color_t(other.color())),
    is_catched_(other.is_catched())
{
}

Ball::~Ball()
{
}

Ball &Ball::operator=(const Ball &other)
{
    if (&other == this)
    {
        return *this;
    }

    collision_radius_ = other.collision_radius();
    visulation_radius_ = other.visulation_radius();
    *pos_ = other.pos();
    *color_ = other.color();
    is_catched_ = other.is_catched();

    return *this;
}

double Ball::collision_radius() const
{
    return collision_radius_;
}

double Ball::visulation_radius() const
{
    return visulation_radius_;
}

math::Vector3d_t Ball::pos() const
{
    return *pos_;
}

Color_t Ball::color() const
{
    return *color_;
}

bool Ball::is_catched() const
{
    return is_catched_;
}

void Ball::set_collision_radius(const double collision_radius)
{
    collision_radius_ = collision_radius;
}

void Ball::set_visulation_radius(const double visulation_radius)
{
    visulation_radius_ = visulation_radius;
}

void Ball::set_pos(const math::Vector3d_t &pos)
{
    *pos_ = pos;
}

void Ball::set_color(const Color_t &color)
{
    *color_ = color;
}

void Ball::set_catched(const bool is_catched)
{
    is_catched_ = is_catched;
}

// protected func.

// private func.

} // namespace gui {
