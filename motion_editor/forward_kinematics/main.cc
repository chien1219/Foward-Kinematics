#include <memory>
#include "FL/Fl.H"
#include "param_config.h"
#include "gui_main_window.h"
#include "gui_signal.h"

int main(int argc, char **argv)
{
    gui::MainWindow main_window(750, 610, "Acclaim file player");
    std::shared_ptr<param::Config> param_config(
            new param::Config("../parameter/config.xml", "config.")
            );
    std::shared_ptr<gui::Signal> gui_signal(new gui::Signal);
    main_window.Configure(param_config, gui_signal);
    main_window.show();

    return Fl::run();
}
