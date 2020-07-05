#include "anchors.h"

#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"


Anchor::~Anchor() {
    std::cerr << "destroying Anchor abstract class:" + name + "\n";
}


std::tuple<int, int> Anchor::selectedIndexes() {
    return std::make_tuple(selected_i, selected_j);
}

void Anchor::setGreen(int i, int j) {
    green_i = i;
    green_j = j;
}