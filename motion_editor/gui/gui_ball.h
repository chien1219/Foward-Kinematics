#ifndef _GUI_BALL_H_
#define _GUI_BALL_H_

#include "gui_def.h"
#include <memory>
#include "math_type.h"
#include "gui_type.h"

namespace gui {

class Ball final
{

public:

    Ball();
    Ball(const Ball &other);
    virtual ~Ball();
    Ball &operator=(const Ball &other);

    double collision_radius() const;
    double visulation_radius() const;
    math::Vector3d_t pos() const;
    Color_t color() const;
    bool is_catched() const;

    void set_collision_radius(const double collision_radius);
    void set_visulation_radius(const double visulation_radius);
    void set_pos(const math::Vector3d_t &pos);
    void set_color(const Color_t &color);
    void set_catched(const bool is_catched);

protected:

private:

    double collision_radius_;
    double visulation_radius_;
    std::unique_ptr<math::Vector3d_t> pos_;
    std::unique_ptr<Color_t> color_;
    bool is_catched_;
};

} // namespace gui {

#endif // #ifndef _GUI_BALL_H_
