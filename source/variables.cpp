
#include "include/variables.hpp"

namespace GLOBAL_STATES {

    bool VIDEO_PAUSED;
    bool VIDEO_SEEK_REQUESTED;

    std::condition_variable VIDEO_PAUSED_CONDITION;

}