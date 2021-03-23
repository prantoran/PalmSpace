

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
            PADLARGE,
            S2HABSOLUTE,
            H2SRELATIVE
        };

        std::string str(const types & a);
    }

    namespace inputspace {
        enum types {
            SAMEASSCREENSIZE = 1,
            PALMSIZED
        };
    }

    namespace screensize {
        enum types {
            INVALID,
            SMALL = 1,
            LARGE,
            FULL,
            FOURHUNDRED,
        };

        types from_int(int choice_screensize);
    }

    namespace visibility {
        enum types {
            INVALID,
            FIXED = 1,
            CONDITIONAL,
        };   

        types from_int(int choice_visibility);
    }
}

#endif