#ifndef _KINEMATICS_TIME_WARP_HARD_CONSTRAINT_H_
#define _KINEMATICS_TIME_WARP_HARD_CONSTRAINT_H_

#include "kinematics_def.h"
#include <vector>
#include "kinematics_fwd.h"

namespace kinematics {

typedef TimeWarpHardConstraint TimeWarpHardConstraint_t;
typedef std::vector<TimeWarpHardConstraint_t> TimeWarpHardConstraintColl_t;

struct TimeWarpHardConstraint final
{
    int32_t frame_idx;
    double play_second;

    TimeWarpHardConstraint();
    explicit TimeWarpHardConstraint(
            const int32_t frame_idx_param,
            const double play_second_param
            );
    TimeWarpHardConstraint(const TimeWarpHardConstraint &) = default;
    virtual ~TimeWarpHardConstraint();
    TimeWarpHardConstraint &operator=(const TimeWarpHardConstraint &) = default;
};

} // namespace kinematics {

namespace kinematics {

template<typename charT, typename traits>
std::basic_ostream<charT, traits> &operator<<(
        std::basic_ostream<charT, traits> &ostream,
        const TimeWarpHardConstraint_t &constraint
        )
{
    ostream << "frame_idx " << constraint.frame_idx << ", "
            << "play_second " << constraint.play_second << std::endl;
    return ostream;
}

} // namespace kinematics {

#endif // #ifndef _KINEMATICS_TIME_WARP_HARD_CONSTRAINT_H_
