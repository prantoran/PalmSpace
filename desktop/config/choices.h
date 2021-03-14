

#ifndef CHOICES_H
#define CHOICES_H

#include <string>
#include <iostream>
#include <unordered_map>

namespace choices {
    namespace anchor {
        enum types {
            DYNAMIC = 1,
            STATIC,
            MIDAIR,
            HANDTOSCREEN,
            PAD,
            PADLARGE
        };

        std::string str(const types & a);
    }

    // TODO clean up unused

    enum eInitiators {
        DEFAULT = 1, 
        TWOHAND
    };

    enum eAnchors {
        DYNAMIC = 1, 
        STATIC, 
        MIDAIR
    };

    enum eTriggers {
        THUMBBASEPALM = 1,
        THUMBFREEPALM,
        PINCHFREEPALM,
        WAIT,
        TAP,
        DWELL
    };


    enum eScreenSize {
        INVALID,
        SMALL = 1,
        LARGE,
        FULL,
    };

    enum eVisibility {
        INVALID_VISIBILITY,
        FIXED = 1,
        CONDITIONAL,
    };


    eScreenSize getScreenSize(int choice_screensize);

    extern std::unordered_map <int, choices::eScreenSize> from_int_to_eScreenSize;
   

    eVisibility getVisibility(int choice_visibility);

    extern std::unordered_map <int, choices::eVisibility> from_int_to_eVisibility;
}

#endif