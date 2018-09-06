#ifndef _GUI_CONVERT_SIMULATION_TIME_TO_FRAME_IDX_H_
#define _GUI_CONVERT_SIMULATION_TIME_TO_FRAME_IDX_H_

#include "gui_def.h"
#include "boost/optional.hpp"
#include "math_type.h"

namespace gui {

class ConvertSimulationTimeToFrameIdx final
{

public:

    ConvertSimulationTimeToFrameIdx();
    ConvertSimulationTimeToFrameIdx(const ConvertSimulationTimeToFrameIdx &other);
    virtual ~ConvertSimulationTimeToFrameIdx();
    ConvertSimulationTimeToFrameIdx &operator=(const ConvertSimulationTimeToFrameIdx &other);

    int32_t operator()(const double simulation_second) const;
    int32_t operator()(
            const double simulation_time_step,
            const double simulation_second
            ) const;

    boost::optional<double> simulation_time_step() const;

    void set_simulation_time_step(const double simulation_time_step);

protected:

private:

    boost::optional<double> simulation_time_step_;
};

} // namespace gui {

#endif // #ifndef _GUI_CONVERT_SIMULATION_TIME_TO_FRAME_IDX_H_
