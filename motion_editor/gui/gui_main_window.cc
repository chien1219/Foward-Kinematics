#include "gui_main_window.h"
#include <algorithm>
#include <functional>
#include "boost/numeric/conversion/cast.hpp"
#include "boost/bind.hpp"
// define the maximum parameter number in boost/ptr_map_inserter.hpp,
// since parameter number in the constructor Fl_Button exceeds the default value
#define BOOST_ASSIGN_MAX_PARAMS 7
#include "boost/assign/list_of.hpp"
#include "boost/assign/ptr_map_inserter.hpp"
#include "FL/gl.h" 
#include "FL/Fl_Group.H"
#include "FL/Fl_Widget.H"
#include "FL/Fl_Button.H"
#include "FL/Fl_Light_Button.H"
#include "FL/Fl_Value_Slider.H"
#include "FL/Fl_Value_Input.H"
#include "FL/Fl_File_Chooser.H" // Allow a file chooser for save.
#include "console_log.h"
#include "param_config.h"
#include "acclaim_skeleton.h"
#include "acclaim_motion.h"
#include "gui_utils.h"
#include "gui_display.h"
#include "gui_gl_window.h"
#include "gui_renderer.h"
#include "gui_marker.h"
#include "gui_signal.h"
#include "gui_mouse.h"
#include "gui_color.h"

namespace gui {

// public func.

MainWindow::MainWindow(int32_t width, int32_t height, const char *title)
    :Fl_Window(width, height, title),
    button_map_(new ButtonMap_t),
    light_button_map_(new LightButtonMap_t),
    value_input_map_(new ValueInputMap_t),
    value_slider_map_(new ValueSliderMap_t),
    gl_window_(nullptr),
    display_(new Display),
    renderer_(new Renderer),
    button_slot_map_(new SlotFuncMap_t),
    light_button_slot_map_(new SlotFuncMap_t),
    value_input_slot_map_(new SlotFuncMap_t),
    switch_map_(new SwitchMap_t),
    mouse_(new Mouse),
    param_(nullptr),
    signal_(nullptr),
    max_frame_num_(0),
    frame_idx_(0),
    frame_increment_(4),
    time_step_(0.0),
    timeout_callback_data_(new TimeoutCallbackData_t),
    callback_data_coll_(),
    widget_callback_data_map_(new WidgetCallbackDataMap_t)
{
}

MainWindow::~MainWindow()
{
}

int32_t MainWindow::handle(int32_t event)
{
    // retval 0 if the event was not used or understood
    // retval 1 if the event was used and can be deleted
    int32_t used_event = 1;
    used_event = Fl_Window::handle(event);
    gl_window_->redraw();

    return used_event;  // Returning one acknowledges that we handled this event
}

void MainWindow::Configure(
        const std::shared_ptr<param::Config> &param,
        const std::shared_ptr<Signal> &signal
        )
{
    param_ = param;
    signal_ = signal;

    display_->Configure(param_, renderer_);

    this->InitButton();
    this->InitLightButton();
    this->InitValueInput();
    this->InitValueSlider();
    this->InitGlWindow();

    *switch_map_ = SwitchMap_t(
        {
            {"play",      false},
            {"rewind",    false},
            {"repeat",    false},
            {"record",    false},
            {"ik_mode",   false},
            {"time_warp", false},
        }
    );

    this->DeactivateWidget();

    light_button_map_->at("light").value(gl_window_->is_switch_on("light"));
    light_button_map_->at("ground").value(gl_window_->is_switch_on("ground"));
    light_button_map_->at("ik_marker").value(gl_window_->is_switch_on("ik_marker"));
    light_button_map_->at("ik_mode").value(this->is_switch_on("ik_mode"));
    light_button_map_->at("time_warp").value(this->is_switch_on("time_warp"));
    light_button_map_->at("record").value(this->is_switch_on("record"));

    time_step_ = param_->value<double>("time_step");

    timeout_callback_data_->signal = signal_;
    timeout_callback_data_->time_step = time_step_;

    frame_increment_ = param_->value<int32_t>("frame_increment");

    signal_->ConnectDisplayShow(
            boost::bind(&Display::Show, display_.get())
            );
    signal_->ConnectMainWindowTimeout(
            boost::bind(&MainWindow::TimeoutSlot, this)
            );
    Fl::add_timeout(
            time_step_,
            MainWindow::timeout_callback,
            static_cast<void *>(timeout_callback_data_.get())
            );
}

// protected func.

// private func.

void MainWindow::InitButton()
{
    boost::assign::ptr_map_insert<Fl_Button>(*button_map_)
        ("load_motion",     315, 540, 120, 30, "Load motion")
        ("load_skeleton",   315, 500, 120, 30, "Load skeleton")
        ("reset_skeleton",  445, 500, 120, 30, "Reset skeleton")
        ("reset_marker",    445, 540, 120, 30, "Reset marker")
        ("clean_footskate", 575, 500, 120, 30, "Clean footskate")
        ("exit",            575, 540, 120, 30, "Exit")
        ("rewind",          115, 525,  35, 25, "@|<")
        ("pause",           155, 525,  35, 25, "@||")
        ("play",            195, 525,  35, 25, "@>")
        ("rewind",           75, 525,  35, 25, "@|<")
        ("repeat",          235, 525,  35, 25, "@<->");
    //("slerp", 535, 500, 100, 30,       "Slerp")
    //("save",   x,   y,   w,  h,        "Save")

    *button_slot_map_ = SlotFuncMap_t(
        {
            {"exit",            &MainWindow::ExitSlot},
            {"load_motion",     &MainWindow::LoadMotionSlot},
            {"load_skeleton",   &MainWindow::LoadSkeletonSlot},
            {"reset_skeleton",  &MainWindow::ResetSkeletonSlot},
            {"reset_marker",    &MainWindow::ResetMarkerSlot},
            {"clean_footskate", &MainWindow::CleanFootskateSlot},
            {"rewind",          &MainWindow::RewindSlot},
            {"pause",           &MainWindow::PauseSlot},
            {"play",            &MainWindow::PlaySlot},
            {"repeat",          &MainWindow::RepeatSlot},
            //{"slerp",            &MainWindow::slerpSlot},
        }
    );
    
    auto found_rewind = button_map_->find("rewind");
    found_rewind->second->labeltype(FL_SYMBOL_LABEL);
    found_rewind->second->labelsize(12);
    auto found_pause = button_map_->find("pause");
    found_pause->second->labeltype(FL_SYMBOL_LABEL);
    found_pause->second->labelsize(12);
    auto found_paly = button_map_->find("play");
    found_paly->second->labeltype(FL_SYMBOL_LABEL);
    found_paly->second->labelsize(12);
    auto found_repeat = button_map_->find("repeat");
    found_repeat->second->labeltype(FL_SYMBOL_LABEL);
    found_repeat->second->labelsize(12);
    found_repeat->second->align(200 | FL_ALIGN_INSIDE);

    // register the signal-slot
    std::for_each(
            button_map_->begin(),
            button_map_->end(),
            [&](const decltype(*(button_map_->begin())) &id_button)
            {
                widget_callback_data_map_->insert(
                        id_button.first,
                        new FlWidgetCallbackData_t
                        );
                id_button.second->user_data(
                        &widget_callback_data_map_->at(id_button.first)
                        );
                id_button.second->user_data(
                        &widget_callback_data_map_->at(id_button.first)
                        );
                id_button.second->callback(Signal::Send);

                auto found_slot = button_slot_map_->find(id_button.first);
                widget_callback_data_map_->at(id_button.first).signal.connect(
                        boost::bind(found_slot->second, this, _1)
                        );
            }
            );
}

void MainWindow::InitLightButton()
{
    boost::assign::ptr_map_insert<Fl_Light_Button>(*light_button_map_)
        ("record",     70, 525, 40, 25, "R")
        ("light",     655, 300, 90, 25, "Light")
        ("ground",    655, 330, 90, 25, "Ground")
        ("ik_marker", 655, 370, 90, 25, "IK marker")
        ("ik_mode",   655, 400, 90, 25, "IK mode")
        ("time_warp", 655, 430, 90, 25, "Time warp");

    *light_button_slot_map_ = SlotFuncMap_t(
        {
            {"record",    &MainWindow::RecordSlot},
            {"light",     &MainWindow::LightSlot},
            {"ground",    &MainWindow::GroundSlot},
            {"ik_marker", &MainWindow::IkMarkerSlot},
            {"ik_mode",   &MainWindow::IkModeSlot},
            {"time_warp", &MainWindow::TimeWarpSlot},
        }
    );

    std::for_each(
            light_button_map_->begin(),
            light_button_map_->end(),
            [&](const decltype(*(light_button_map_->begin())) &id_button)
            {
                widget_callback_data_map_->insert(
                        id_button.first,
                        new FlWidgetCallbackData_t
                        );
                id_button.second->user_data(
                        &widget_callback_data_map_->at(id_button.first)
                        );
                id_button.second->callback(signal_->Send);
                auto found_slot = light_button_slot_map_->find(id_button.first);
                widget_callback_data_map_->at(id_button.first).signal.connect(
                        boost::bind(found_slot->second, this, _1)
                        );
            }
            );
}

void MainWindow::InitValueInput()
{
    boost::assign::ptr_map_insert<Fl_Value_Input>(*value_input_map_)
        ("frame_steps", 240, 560, 30, 20, "Frame steps")
        ("dt",          680, 660, 45, 25, "dt:")
        ("sub",         680,  20, 45, 25, "sub:")
        ("tx",          680,  95, 45, 25, "tx:")
        ("ty",          680, 125, 45, 25, "ty:")
        ("tz",          680, 155, 45, 25, "tz:")
        ("rx",          680, 190, 45, 25, "rx:")
        ("ry",          680, 220, 45, 25, "ry:")
        ("rz",          680, 250, 45, 25, "rz:")
        ("joint_idx",    65, 560, 30, 20, "Joint");

    *value_input_slot_map_ = SlotFuncMap_t(
        {
            {"frame_steps", &MainWindow::FrameStepsSlot},
            {"dt",          &MainWindow::FrameOffsetSlot},
            {"sub",         &MainWindow::IndicateSkeletonSlot},
            {"tx",          &MainWindow::SkeletonTranslationXSlot},
            {"ty",          &MainWindow::SkeletonTranslationYSlot},
            {"tz",          &MainWindow::SkeletonTranslationZSlot},
            {"rx",          &MainWindow::SkeletonRotationXSlot},
            {"ry",          &MainWindow::SkeletonRotationYSlot},
            {"rz",          &MainWindow::SkeletonRotationZSlot},
            {"joint_idx",   &MainWindow::JointIdxSlot},
        }
    );

    auto found_joint_idx = value_input_map_->find("joint_idx");
    found_joint_idx->second->maximum(30);
    found_joint_idx->second->step(1);
    found_joint_idx->second->value(-1);
    auto found_frame_steps = value_input_map_->find("frame_steps");
    found_frame_steps->second->minimum(1);
    found_frame_steps->second->maximum(30);
    found_frame_steps->second->step(1);
    found_frame_steps->second->value(1);

    std::for_each(
            value_input_map_->begin(),
            value_input_map_->end(),
            [&](const decltype(*value_input_map_->begin()) &id_value_input)
            {
                widget_callback_data_map_->insert(
                        id_value_input.first,
                        new FlWidgetCallbackData_t
                        );
                id_value_input.second->user_data(
                        &widget_callback_data_map_->at(id_value_input.first)
                        );
                id_value_input.second->callback(&signal_->Send);

                auto found_slot = value_input_slot_map_->find(id_value_input.first);
                widget_callback_data_map_->at(id_value_input.first).signal.connect(
                        boost::bind(found_slot->second, this, _1)
                        );
            }
            );
}

void MainWindow::InitValueSlider()
{
    boost::assign::ptr_map_insert<Fl_Value_Slider>(*value_slider_map_)
        ("frame_slider", 30, 500, 240, 20);
    auto found_frame_slider = value_slider_map_->find("frame_slider");
    found_frame_slider->second->user_data(const_cast<char *>(found_frame_slider->first.c_str()));
    found_frame_slider->second->value(boost::numeric_cast<int32_t>(1));
    found_frame_slider->second->type(5);
    found_frame_slider->second->labelsize(18);
    found_frame_slider->second->minimum(1);
    found_frame_slider->second->maximum(60000);
    found_frame_slider->second->step(1);
    found_frame_slider->second->align(197);

    widget_callback_data_map_->insert(
            std::string("frame_slider"),
            new FlWidgetCallbackData_t
            );
    found_frame_slider->second->user_data(
            &widget_callback_data_map_->at(found_frame_slider->first)
            );
    found_frame_slider->second->callback(&signal_->Send);

    widget_callback_data_map_->at(found_frame_slider->first).signal.connect(
            boost::bind(&MainWindow::FrameSlideSlot, this, _1)
            );
}

void MainWindow::InitGlWindow()
{
    gl_window_ = std::unique_ptr<GlWindow>(new GlWindow(5, 5, 640, 480, "label"));
    gl_window_->box(FL_DOWN_FRAME);
    gl_window_->labeltype(FL_NO_LABEL);

    gl_window_->Configure(param_, signal_, renderer_);
}

void MainWindow::DeactivateWidget()
{
    button_map_->at("clean_footskate").deactivate();
    button_map_->at("reset_marker").deactivate();
    light_button_map_->at("light").deactivate();
    light_button_map_->at("ik_marker").deactivate();
    light_button_map_->at("ik_mode").deactivate();
    light_button_map_->at("time_warp").deactivate();
}

void MainWindow::set_switch(const Id_t &id, const bool is_on)
{
    switch_map_->at(id) = is_on;
}

bool MainWindow::is_switch_on(const Id_t &id)
{
    return switch_map_->at(id);
}

void MainWindow::ExitSlot(Fl_Widget *widget)
{
    exit(1);
}

void MainWindow::LoadMotionSlot(Fl_Widget *widget)
{
    if (gl_window_->is_switch_on("skeleton_exist"))
    {
        const char *amc_file_name = fl_file_chooser(
                "Select file name",
                "*.AMC",
                "../acclaim_file"
                );
        if (nullptr == amc_file_name)
        {
            return;
        }

        std::shared_ptr<acclaim::Motion> motion(
                new acclaim::Motion(
                    std::string(amc_file_name),
                    acclaim::MoCapScale(),
                    display_->skeleton(display_->motion_num())
                    )
                );

        display_->LoadMotion(motion);
        int32_t motion_idx = display_->motion_num() - 1;
        if ((display_->frame_num(motion_idx) - 1) > max_frame_num_)
        {
            max_frame_num_ = display_->frame_num(motion_idx) - 1;
            value_slider_map_->at("frame_slider").maximum(
                    boost::numeric_cast<double>(max_frame_num_ + 1)
                    );
        }

        frame_idx_ = boost::numeric_cast<int32_t>(value_slider_map_->at("frame_slider").value() - 1);
        display_->set_frame_idx(frame_idx_);

        if (display_->skeleton_num() == display_->motion_num())
        {
            const int32_t skeleton_idx = display_->skeleton_num() - 1;
            math::Vector6d_t skeleton_root_offset = display_->skeleton_root_offset(skeleton_idx);
            math::Vector3d_t linear_offset(
                    math::Vector3d_t::RealScalar{0.0},
                    -param_->value<math::Vector3d_t::RealScalar>("drift_up"),
                    math::Vector3d_t::RealScalar{0.0}
                    );
            skeleton_root_offset.set_linear_vector(
                    skeleton_root_offset.linear_vector() + linear_offset
                    );
            display_->set_skeleton_root_offset(skeleton_idx, skeleton_root_offset);
        }

        Fl::flush();
        gl_window_->redraw();
    } // if (gl_window_->is_switch_on("skeleton_exist"))

    light_button_map_->at("time_warp").activate();
}

void MainWindow::LoadSkeletonSlot(Fl_Widget *widget)
{
    const char *asf_file_name = fl_file_chooser(
            "Select file name",
            "*.ASF",
            "../acclaim_file"
            );
    if (nullptr == asf_file_name)
    {
        return;
    }

    std::shared_ptr<acclaim::Skeleton> skeleton(
            new acclaim::Skeleton(std::string(asf_file_name), acclaim::MoCapScale())
            );
    //skeleton->set_base_posture();
    display_->LoadSkeleton(skeleton);
    gl_window_->set_switch("skeleton_exist", true);

    const int32_t skeleton_idx = display_->skeleton_num() - 1;
    math::Vector6d_t skeleton_root_offset = display_->skeleton_root_offset(skeleton_idx);
    math::Vector3d_t linear_offset(
            math::Vector3d_t::RealScalar{0.0},
            param_->value<math::Vector3d_t::RealScalar>("drift_up"),
            math::Vector3d_t::RealScalar{0.0}
            );
    skeleton_root_offset.set_linear_vector(
            skeleton_root_offset.linear_vector() + linear_offset
            );
    display_->set_skeleton_root_offset(skeleton_idx, skeleton_root_offset);

    gl_window_->redraw();
}

void MainWindow::ResetSkeletonSlot(Fl_Widget *widget)
{
    this->set_switch("rewind", true);
    this->set_switch("play", false);
    this->set_switch("repeat", false);

    this->set_switch("time_warp", false);
    light_button_map_->at("time_warp").value(switch_map_->at("time_warp"));
    light_button_map_->at("time_warp").deactivate();

    gl_window_->set_switch("skeleton_exist", false);
    display_->Reset();

    max_frame_num_ = 0;
    frame_idx_ = 0;

    gl_window_->redraw();
}

void MainWindow::ResetMarkerSlot(Fl_Widget *widget)
{
    light_button_map_->at("ik_marker").value(boost::numeric_cast<int32_t>(FALSE));
    this->set_switch("ik_mode", false);
    //ik_marker_->ResetPos();
}

void MainWindow::SlerpSlot(Fl_Widget *widget)
{
    display_->Slerp();
}

void MainWindow::CleanFootskateSlot(Fl_Widget *widget)
{
    display_->CleanFootskate();
}

void MainWindow::SaveSlot(Fl_Widget *widget)
{
    display_->Save();
}

void MainWindow::RewindSlot(Fl_Widget *widget)
{
    this->PlaySlot(widget);
}

void MainWindow::PauseSlot(Fl_Widget *widget)
{
    this->PlaySlot(widget);
}

void MainWindow::PlaySlot(Fl_Widget *widget)
{
    if (!display_->is_empty_motion(display_->motion_num() - 1)
            && display_->skeleton_num() == display_->motion_num())
    {
        if (widget == &(button_map_->at("play")))
        {
            this->set_switch("play", true);
            this->set_switch("rewind", false);
        }

        if (widget == &(button_map_->at("pause")))
        {
            this->set_switch("play", false);
            this->set_switch("repeat", false);
        }

        if (widget == &(button_map_->at("repeat")))
        {
            this->set_switch("rewind", false);
            this->set_switch("play", true);
            this->set_switch("repeat", true);
        }

        if (widget == &(button_map_->at("rewind")))
        {
            this->set_switch("rewind", true);
            this->set_switch("play", false);
            this->set_switch("repeat", false);
        }
    }
}

void MainWindow::RepeatSlot(Fl_Widget *widget)
{
    this->PlaySlot(widget);
}

void MainWindow::RecordSlot(Fl_Widget *widget)
{
    if (!this->is_switch_on("play"))
    {
        bool is_record_light_on = boost::numeric_cast<bool>(light_button_map_->at("record").value());
        if (!this->is_switch_on("record") && is_record_light_on)
        {
            this->set_switch("record", true);
        }

        if (this->is_switch_on("record") && !is_record_light_on)
        {
            this->set_switch("record", false);
        }
    }

    light_button_map_->at("record").value(
            boost::numeric_cast<char>(this->is_switch_on("record"))
            );
}

void MainWindow::LightSlot(Fl_Widget *widget)
{
    gl_window_->set_switch(
            "light",
            boost::numeric_cast<bool>(light_button_map_->at("light").value())
            );
    gl_window_->set_switch(
            "ground",
            boost::numeric_cast<bool>(light_button_map_->at("ground").value())
            );
    gl_window_->set_switch(
            "ik_marker",
            boost::numeric_cast<bool>(light_button_map_->at("ik_marker").value())
            );
    gl_window_->redraw();
}

void MainWindow::GroundSlot(Fl_Widget *widget)
{
    this->LightSlot(widget);
}

void MainWindow::IkMarkerSlot(Fl_Widget *widget)
{
    this->LightSlot(widget);
}

void MainWindow::IkModeSlot(Fl_Widget *widget)
{
    switch_map_->at("ik_mode")
        = boost::numeric_cast<bool>(light_button_map_->at("ik_mode").value());
    bool is_solve_ik = this->is_switch_on("ik_mode")
        && gl_window_->is_switch_on("ik_marker")
        && !display_->is_empty_motion(0);

    //if (is_solve_ik)
    //{
    //    display_->SolveIk(ik_marker_->target_pos());
    //}
}

void MainWindow::TimeWarpSlot(Fl_Widget *widget)
{
    switch_map_->at("time_warp")
        = boost::numeric_cast<bool>(light_button_map_->at("time_warp").value());

    display_->SolveTimeWarp();
}

void MainWindow::FrameSlideSlot(Fl_Widget *widget)
{
    if (gl_window_->is_switch_on("skeleton_exist") && !display_->is_empty_motion(0))
    {
        if (display_->frame_num(0) > 0)
        {
            frame_idx_ = boost::numeric_cast<int32_t>(value_slider_map_->at("frame_slider").value() - 1);
            display_->set_frame_idx(frame_idx_);

            Fl::flush();
            gl_window_->redraw();
        }
    }
}

void MainWindow::FrameStepsSlot(Fl_Widget *widget)
{
    display_->set_spot_joint_idx(boost::numeric_cast<int32_t>(value_input_map_->at("joint").value()));
    frame_increment_ = boost::numeric_cast<int32_t>(value_input_map_->at("frame_steps").value());
    gl_window_->redraw();
}

void MainWindow::FrameOffsetSlot(Fl_Widget *widget)
{
    const int32_t skeleton_idx = boost::numeric_cast<int32_t>(button_map_->at("sub").value());
    if (skeleton_idx >= 0 && skeleton_idx < display_->skeleton_num())
    {
        display_->motion(skeleton_idx)->set_frame_offset(
                boost::numeric_cast<int32_t>(value_input_map_->at("dt").value())
                );
        LOGMSG << "Shifting subject " << skeleton_idx
               << " by " << value_input_map_->at("dt").value()
               << std::endl;
        int32_t max_offset_frame_num = 0;
        for (int32_t i = 0; i < display_->skeleton_num(); ++i)
        {
            int32_t offset_frame_num = display_->offset_frame_num(i) - 1;
            if (offset_frame_num > max_offset_frame_num)
            {
                max_offset_frame_num = offset_frame_num;
            }
        }
        max_frame_num_ = max_offset_frame_num;
        value_slider_map_->at("frame_slider").maximum(
                boost::numeric_cast<double>(max_frame_num_ + 1)
                );
        display_->set_frame_idx(frame_idx_);
    }

    gl_window_->redraw();
}

void MainWindow::IndicateSkeletonSlot(Fl_Widget *widget)
{
    const int32_t skeleton_idx = boost::numeric_cast<int32_t>(value_input_map_->at("sub").value());
    if (skeleton_idx < 0)
    {
        value_input_map_->at("sub").value(0);
    }
    else if (skeleton_idx > display_->skeleton_num() - 1)
    {
        value_input_map_->at("sub").value(display_->skeleton_num() - 1);
    }
    else
    {
        value_input_map_->at("dt").value(display_->motion(skeleton_idx)->frame_offset());
        math::Vector6d_t skeleton_root_offset = display_->skeleton_root_offset(skeleton_idx);
        value_input_map_->at("tx").value(skeleton_root_offset.linear_vector().x());
        value_input_map_->at("ty").value(skeleton_root_offset.linear_vector().y());
        value_input_map_->at("tz").value(skeleton_root_offset.linear_vector().z());
        value_input_map_->at("rx").value(skeleton_root_offset.angular_vector().x());
        value_input_map_->at("ry").value(skeleton_root_offset.angular_vector().y());
        value_input_map_->at("rz").value(skeleton_root_offset.angular_vector().z());
    }
    gl_window_->redraw();
}

void MainWindow::SkeletonTranslationXSlot(Fl_Widget *widget)
{
    const int32_t skeleton_idx = boost::numeric_cast<int32_t>(value_input_map_->at("sub").value());
    if (skeleton_idx >= 0 && skeleton_idx < display_->skeleton_num())
    {
        display_->set_skeleton_root_offset(
                skeleton_idx,
                this->skeleton_root_offset()
                );
    }
    gl_window_->redraw();
}

void MainWindow::SkeletonTranslationYSlot(Fl_Widget *widget)
{
    const int32_t skeleton_idx = boost::numeric_cast<int32_t>(value_input_map_->at("sub").value());
    if (skeleton_idx >= 0 && skeleton_idx < display_->skeleton_num())
    {
        display_->set_skeleton_root_offset(
                skeleton_idx,
                this->skeleton_root_offset()
                );
    }
    gl_window_->redraw();
}

void MainWindow::SkeletonTranslationZSlot(Fl_Widget *widget)
{
    const int32_t skeleton_idx = boost::numeric_cast<int32_t>(value_input_map_->at("sub").value());
    if (skeleton_idx >= 0 && skeleton_idx < display_->skeleton_num())
    {
        display_->set_skeleton_root_offset(
                skeleton_idx,
                this->skeleton_root_offset()
                );
    }
    gl_window_->redraw();
}

void MainWindow::SkeletonRotationXSlot(Fl_Widget *widget)
{
    const int32_t skeleton_idx = boost::numeric_cast<int32_t>(value_input_map_->at("sub").value());
    if (skeleton_idx >= 0 && skeleton_idx < display_->skeleton_num())
    {
        display_->set_skeleton_root_offset(
                skeleton_idx,
                this->skeleton_root_offset()
                );
    }
    gl_window_->redraw();
}

void MainWindow::SkeletonRotationYSlot(Fl_Widget *widget)
{
    const int32_t skeleton_idx = boost::numeric_cast<int32_t>(value_input_map_->at("sub").value());
    if (skeleton_idx >= 0 && skeleton_idx < display_->skeleton_num())
    {
        display_->set_skeleton_root_offset(
                skeleton_idx,
                this->skeleton_root_offset()
                );
    }
    gl_window_->redraw();
}

void MainWindow::SkeletonRotationZSlot(Fl_Widget *widget)
{
    const int32_t skeleton_idx = boost::numeric_cast<int32_t>(value_input_map_->at("sub").value());
    if (skeleton_idx >= 0 && skeleton_idx < display_->skeleton_num())
    {
        display_->set_skeleton_root_offset(
                skeleton_idx,
                this->skeleton_root_offset()
                );
    }
    gl_window_->redraw();
}

void MainWindow::JointIdxSlot(Fl_Widget *widget)
{
    display_->set_spot_joint_idx(boost::numeric_cast<int32_t>(value_input_map_->at("joint_idx").value()));
    frame_increment_ = boost::numeric_cast<int32_t>(value_input_map_->at("frame_steps").value());
    gl_window_->redraw();
}

void MainWindow::TimeoutSlot()
{
    if (gl_window_->is_switch_on("skeleton_exist") && !display_->is_empty_motion(0))
    {
        if (this->is_switch_on("rewind"))
        {
            frame_idx_ = 0;
            display_->set_frame_idx(frame_idx_);
            this->set_switch("rewind", false);

        } // if (this->is_switch_on("rewind"))

        if (this->is_switch_on("play"))
        {
            if (frame_idx_ >= max_frame_num_)
            {
                if (this->is_switch_on("repeat"))
                {
                    frame_idx_ = 0;
                }
            }
            else
            {
                frame_idx_ += frame_increment_;
            }

            display_->set_frame_idx(frame_idx_);

            if (this->is_switch_on("record"))
            {
                gl_window_->SaveScreenshot();
            }
        } // if (this->is_switch_on("play"))

        //if (this->is_switch_on("ik_mode") && ik_marker_->is_select())
        //{
        //    display_->SolveIk(ik_marker_->target_pos());
        //}
    }

    value_slider_map_->at("frame_slider").value(boost::numeric_cast<double>(frame_idx_ + 1));

    gl_window_->redraw();
}

void MainWindow::timeout_callback(void *data)
{
    TimeoutCallbackData_t *timeout_callback_data
        = static_cast<TimeoutCallbackData_t *>(data);
    timeout_callback_data->signal->MainWindowTimeout();
    Fl::add_timeout(
            timeout_callback_data->time_step.get(),
            MainWindow::timeout_callback,
            data
            );
}

math::Vector6d_t MainWindow::skeleton_root_offset() const
{
    const math::Vector3d_t rot_offset(
            boost::numeric_cast<double>(value_input_map_->at("rx").value()),
            boost::numeric_cast<double>(value_input_map_->at("ry").value()),
            boost::numeric_cast<double>(value_input_map_->at("rz").value())
            );
    const math::Vector3d_t xlt_offset(
            boost::numeric_cast<double>(value_input_map_->at("tx").value()),
            boost::numeric_cast<double>(value_input_map_->at("ty").value()),
            boost::numeric_cast<double>(value_input_map_->at("tz").value())
            );
    math::Vector6d_t root_offset;
    root_offset.set_angular_vector(rot_offset);
    root_offset.set_linear_vector(xlt_offset);

    return root_offset;
}

} // namespace gui {
