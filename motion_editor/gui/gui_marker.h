#ifndef _GUI_MARKER_H_
#define _GUI_MARKER_H_

#include "gui_def.h"
#include <vector>
#include "math_type.h"
#include "gui_type.h"

namespace gui {

class Marker final
{

public:

    Marker();
    Marker(const Marker &) = delete;
    virtual ~Marker();
    Marker &operator=(const Marker &) = delete;
    /**
     * \brief
     * \return
     */
    math::Vector3d_t init_pos() const;
    /**
     * \brief
     * \return
     */
    math::Vector3d_t target_pos() const;
    /**
     * \brief
     * \return
     */
    Color_t color() const;
    /**
     * \brief
     * \return
     */
    bool is_select() const;

    //set info of object
    void set_color(const Color_t &color);
    void set_init_pos(const math::Vector3d_t &init_pos);
    void set_target_pos(const math::Vector3d_t &target_pos);

    //reset object to initial position
    void ResetPos();

    //set condition of object when selected/released
    void Select();
    void Release();
    void Move(const math::Vector3d_t &shift_vector);

private:

    math::Vector3d_t target_pos_;
    math::Vector3d_t init_pos_;
    std::unique_ptr<Color_t> color_;
    bool is_select_;
};

} // namespace gui {

#endif // #ifndef _GUI_MARKER_H_
