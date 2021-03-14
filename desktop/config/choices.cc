#include "choices.h"
#include <utility>

namespace choices {
    eScreenSize getScreenSize(int choice_screensize) {
        return from_int_to_eScreenSize[choice_screensize];
    }

    std::unordered_map <int, eScreenSize> from_int_to_eScreenSize = {
        {0, INVALID},
        {1, SMALL},
        {2, LARGE},
        {3, FULL}
    };


    eVisibility getVisibility(int choice_visibility) {
        return from_int_to_eVisibility[choice_visibility];
    }

    std::unordered_map <int, eVisibility> from_int_to_eVisibility = {
        {0, INVALID_VISIBILITY},
        {1, FIXED},
        {2, CONDITIONAL},
    };

    std::string anchor::str(const types & a) {
        if (a == DYNAMIC) return "S2H_relative";
        if (a == PADLARGE) return "S2H_absolute";
        if (a == HANDTOSCREEN) return "H2S";
        return "invalid";
    }
}



