#ifndef _GUI_GL_WINDOW_H_
#define _GUI_GL_WINDOW_H_

#include "gui_def.h"
#include <memory>
#include "boost/optional.hpp"
#include "FL/Fl_Gl_Window.H"
#include "param_fwd.h"
#include "math_type.h"
#include "acclaim_fwd.h"
#include "gui_fwd.h"
#include "gui_type.h"

namespace gui {

class GlWindow final : public Fl_Gl_Window
{

public:

    GlWindow(int x, int y, int w, int h, const char *l = 0);
    /**
     * \brief Forbid copy constructor
     */
    GlWindow(const GlWindow &) = delete;
    /**
     * \brief Destructor
     */
    virtual ~GlWindow();
    /**
     * \brief Forbid assignment operator
     */
    GlWindow &operator=(const GlWindow &) = delete;
    /**
     * \brief
     */
    void set_ik_marker(const Marker &ik_marker);
    /**
     * \brief
     */
    void set_switch(const std::string &id, const bool is_on);
    /**
     */
    void Configure(
            const std::shared_ptr<param::Config> &param,
            const std::shared_ptr<Signal> &signal,
            const std::shared_ptr<Renderer> &renderer
            );
    /**
     * \brief Save screenshot to PNG
     */
    void SaveScreenshot();
    /**
     * \brief
     */
    bool is_switch_on(const std::string &id) const;

protected:

    /* This is an overloading of a Fl_Gl_Window call.  It is called
       whenever a window needs refreshing. */
    virtual void draw() override;
    
    /* This is an overloading of a Window call. It is 
       called whenever a event happens inside the space 
       taken up by the Fl_Gl_Window */
    virtual int32_t handle(int32_t event);

private:

    /**
     * \brief
     */
    void Redisplay();
    /**
     * \brief
     */
    void InitGL();
    /**
     * \brief
     */
    void InitLight();

    std::unique_ptr<gui::SwitchMap_t> switch_map_;
    std::unique_ptr<gui::Screenshot> screenshot_;
    std::unique_ptr<Mouse> mouse_;
    boost::optional<Marker &> ik_marker_;

    std::shared_ptr<Renderer> renderer_;
    std::shared_ptr<param::Config> param_;
    std::shared_ptr<Signal> signal_;
};

} // namespace gui {

#endif // #ifndef _GUI_GL_WINDOW_H_
