#include "gui_marker.h"
#include "gui_color.h"

namespace gui {

Marker::Marker()
    :target_pos_(math::Vector3d_t::Zero()),
    init_pos_(math::Vector3d_t::Zero()),
    color_(new Color_t),
    is_select_(FALSE)
{
}

Marker::~Marker()
{
}

math::Vector3d_t Marker::init_pos() const
{
    return init_pos_;
}

math::Vector3d_t Marker::target_pos() const
{
    return this->target_pos_;
}

Color_t Marker::color() const
{
    return *color_;
}

bool Marker::is_select() const
{
    return this->is_select_;
}

void Marker::set_color(const Color_t &color)
{
    *color_ = color;
}

void Marker::set_init_pos(const math::Vector3d_t &init_pos)
{
    this->init_pos_ = init_pos;
}

void Marker::set_target_pos(const math::Vector3d_t &target_pos)
{
    this->target_pos_ = target_pos;
}

void Marker::ResetPos()
{
    this->target_pos_ = this->init_pos_;
}

void Marker::Select()
{
    this->is_select_ = TRUE;
    const Color_t new_color(
            color_->red(),
            color_->blue(),
            color_->green(),
            color_->alpha()
            );
    this->set_color(new_color);
}

void Marker::Release()
{
    this->is_select_ = FALSE;
    const Color_t new_color(
            color_->red(),
            color_->blue(),
            color_->green(),
            color_->alpha()
            );
    this->set_color(new_color);
}

void Marker::Move(const math::Vector3d_t &shift_vector)
{
    target_pos_ += shift_vector;
}

} // namespace gui {
