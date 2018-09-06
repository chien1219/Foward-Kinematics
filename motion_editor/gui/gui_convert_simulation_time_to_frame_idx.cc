#include "gui_convert_simulation_time_to_frame_idx.h"
#include "boost/numeric/conversion/cast.hpp"

namespace gui {

// public func.

ConvertSimulationTimeToFrameIdx::ConvertSimulationTimeToFrameIdx()
    :simulation_time_step_(boost::none)
{
}

ConvertSimulationTimeToFrameIdx::ConvertSimulationTimeToFrameIdx(const ConvertSimulationTimeToFrameIdx &other)
    :simulation_time_step_(other.simulation_time_step())
{
}

ConvertSimulationTimeToFrameIdx::~ConvertSimulationTimeToFrameIdx()
{
}

ConvertSimulationTimeToFrameIdx &ConvertSimulationTimeToFrameIdx::operator=(const ConvertSimulationTimeToFrameIdx &other)
{
    if (&other == this)
    {
        return *this;
    }

    simulation_time_step_ = other.simulation_time_step();

    return *this;
}

int32_t ConvertSimulationTimeToFrameIdx::operator()(const double simulation_second) const
{
    return (*this)(this->simulation_time_step().get(), simulation_second);
}

int32_t ConvertSimulationTimeToFrameIdx::operator()(
        const double simulation_time_step,
        const double simulation_second
        ) const
{
    const double frame_idx = round(simulation_second / simulation_time_step);
    return boost::numeric_cast<int32_t>(frame_idx);
}

boost::optional<double> ConvertSimulationTimeToFrameIdx::simulation_time_step() const
{
    return simulation_time_step_;
}

void ConvertSimulationTimeToFrameIdx::set_simulation_time_step(const double simulation_time_step)
{
    simulation_time_step_ = simulation_time_step;
}

// protected func.

// private func.

} // namespace gui {
