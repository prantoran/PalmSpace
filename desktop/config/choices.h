

#ifndef CHOICES_H
#define CHOICES_H

#include <string>
#include <iostream>
#include <unordered_map>


typedef int initiator_t;

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



class Choices {
    public:
    eScreenSize getScreenSize(int choice_screensize) {
        switch (choice_screensize) {
            case 1:
                return SMALL;
            case 2:
                return LARGE;
            case 3:
                return FULL;
            default:
                return INVALID;
        }
    }
};



// extern std::unordered_map <int, eScreenSize> fromIntToeScreenSize;

// extern std::unordered_map <initiator_t, std::string> eInitiators_strmap;

// extern std::unordered_map <eAnchors,std::string> eAnchors_strmap;

// extern std::unordered_map <eTriggers,std::string> eTriggers_strmap;

// extern std::unordered_map <eScreenSize,std::string> eScreenSize_strmap;



#endif