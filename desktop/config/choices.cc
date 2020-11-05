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
}


// std::unordered_map <eInitiators, std::string> eInitiators_strmap = {
//     {DEFAULT, "DEFAULT"},
//     {TWOHAND, "TWOHAND"},
//     {INVALID, "INVALID"}
// };

// std::ostream& operator<<(std::ostream& out, const eInitiators value) {
//     cout << eInitiators_strmap[value];
// }


// std::unordered_map <eAnchors, char*> eAnchors_strmap = {
//     {DYNAMIC, "DYNAMIC"},
//     {STATIC, "STATIC"},
//     {MIDAIR, "MIDAIR"},
//     {INVALID, "INVALID"}
// };


// std::ostream& operator<<(std::ostream& out, const eAnchors value) {
//     cout << eAnchors_strmap[value];
// }


// std::unordered_map <eTriggers, std::string> eTriggers_strmap = {
//     {THUMBBASEPALM: "THUMBBASEPALM"},
//     {THUMBFREEPALM: "THUMBFREEPALM"},
//     {PINCHFREEPALM: "PINCHFREEPALM"},
//     {WAIT: "WAIT"},
//     {TAP: "TAP"},
//     {DWELL: "DWELL"},
//     {INVALID: "INVALID"}
// };

// std::ostream& operator<<(std::ostream& out, const eTriggers value) {
//     cout << eTriggers_strmap[value];
// }


// std::unordered_map <eScreenSize, std::string> eScreenSize_strmap = {
//     {SMALL: "SMALL"},
//     {LARGE: "LARGE"},
//     {FULL: "FULL"},
//     {INVALID: "INVALID"}
// };


// std::ostream& operator<<(std::ostream& out, const eScreenSize value) {
//     std::cerr << "escreeensize str:" <<  eScreenSize_strmap[value] << "\n";
//     cout << eScreenSize_strmap[value];
// }


