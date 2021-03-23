#include "choices.h"
#include <utility>

namespace choices {
    namespace screensize {
        types from_int(int choice_screensize) {
            switch (choice_screensize) {
                case 1:
                    return SMALL;
                case 2:
                    return LARGE;
                case 3:
                    return FULL;
                case 4:
                    return FOURHUNDRED;
                default:
                    return INVALID;
            }
        }
    }


    namespace visibility {
        types from_int(int choice_visibility) {
            switch (choice_visibility) {
                case 1:
                    return FIXED;
                case 2:
                    return CONDITIONAL;
                default:
                    return INVALID;
            }
        }

    }
    
    namespace anchor {
        std::string str(const types & a) {
            switch (a) {
                case DYNAMIC:
                    return "S2H_relative";
                case PADLARGE:
                    return "S2H_absolute";
                case HANDTOSCREEN:
                    return "H2S_absolute";
                case H2SRELATIVE:
                    return "H2S_relative";
                default:
                    return "invalid";
            }
        }
    }
}



