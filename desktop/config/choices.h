

#ifndef CHOICES_H
#define CHOICES_H

#include <string>
#include <iostream>

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
};

class Choices {
    public:
    static std::string initiatorLabel(eInitiators i) {
        switch (i) {
            case DEFAULT:
                return "DEFAULT";
            case TWOHAND:
                return "TWOHAND";
            default:
                return "INVALID";
        }
    }


    static std::string anchorLabel(eAnchors a) {
        switch (a) {
            case DYNAMIC:
                return "DYNAMIC";
            case STATIC:
                return "STATIC";
            case MIDAIR:
                return "MIDAIR";
            default:
                return "INVALID";
        }
    }


    static std::string triggerLabel(eTriggers t) {
        switch (t) {
            case THUMBBASEPALM:
                return "THUMBBASEPALM";
            case THUMBFREEPALM:
                return "THUMBFREEPALM";
            case PINCHFREEPALM:
                return "PINCHFREEPALM";
            case WAIT:
                return "WAIT";
            case TAP:
                return "TAP";
            case DWELL:
                return "DWELL";
            default:
                return "INVALID";
        }
    }


    static std::string screensizeLabel(eScreenSize s) {
        switch (s) {
            case SMALL:
                return "small";
            case LARGE:
                return "large";
            default:
                return "INVALID";
        }
    }


    eScreenSize getScreenSize(int choice_screensize) {
        switch (choice_screensize) {
            case 1:
                return SMALL;
            case 2:
                return LARGE;
            default:
                return INVALID;
        }
    }
};

#endif