#ifndef _GUI_SIGNAL_H_
#define _GUI_SIGNAL_H_

#include "gui_def.h"
#include <memory>
#include <functional>
#include "boost/ptr_container/ptr_container.hpp"
#include "FL/Fl_Widget.H"
#include "gui_type.h"

namespace gui {

class Signal final
{

public:

    Signal();
    Signal(const Signal &) = delete;
    virtual ~Signal();
    Signal &operator=(const Signal &) = delete;
    /**
     * \brief
     * \param[in] fl_widget
     */
    static void Send(Fl_Widget *fl_widget);
    /**
     * \brief
     */
    void DisplayShow();
    /**
     * \brief
     */
    void ConnectDisplayShow(const DisplayShowSignal_t::slot_type &slot);
    /**
     * \brief
     */
    void MainWindowTimeout();
    /**
     * \brief
     */
    void ConnectMainWindowTimeout(const MainWindowTimeoutSignal_t::slot_type &slot);

protected:

private:

    //typedef void (*func)(Fl_Widget *);
    //typedef boost::ptr_map<const Id_t, WidgetSignal_t> WidgetSignalMap_t;

    //static Signal &instance();

    //std::unique_ptr<WidgetSignalMap_t> widget_signal_map_;
    std::unique_ptr<DisplayShowSignal_t> display_show_signal_;
    std::unique_ptr<MainWindowTimeoutSignal_t> main_window_timeout_signal_;
};

} // namespace gui {

#endif // #ifndef _GUI_SIGNAL_H_
