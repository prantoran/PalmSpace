

#ifndef CHOICES_H
#define CHOICES_H

#include <string>

enum initiators {
    DEFAULT = 1, 
    TWOHAND
};

enum anchors {
    DYNAMIC = 1, 
    STATIC, 
    MIDAIR
};

enum triggers {
    THUMBBASEPALM = 1,
    THUMBFREEPALM,
    PINCHFREEPALM,
    WAIT,
    TAP,
    DWELL
};

class Choices {
    public:

    static std::string initiator_label(initiators i) {
        switch (i) {
            case DEFAULT:
                return "DEFAULT";
            case TWOHAND:
                return "TWOHAND";
            default:
                return "INVALID";
        }
    }
    
    static std::string anchor_label(anchors a) {
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

    static std::string trigger_label(triggers t) {
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
};



#endif