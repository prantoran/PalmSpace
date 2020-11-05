

#ifndef CHOICES_H
#define CHOICES_H

#include <string>
#include <iostream>
#include <unordered_map>

namespace choices {
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


    eScreenSize getScreenSize(int choice_screensize);

    // external linkage, to make it visible to linker when processing other translation units,
    // apart from the one this header file will be a part of.
    extern std::unordered_map <int, choices::eScreenSize> from_int_to_eScreenSize;

    // extern std::unordered_map <eInitiators, std::string> eInitiators_strmap;

    // extern std::unordered_map <eAnchors, char*> eAnchors_strmap;

    // extern std::unordered_map <eTriggers,std::string> eTriggers_strmap;

    // extern std::unordered_map <eScreenSize,std::string> eScreenSize_strmap;
}

#endif