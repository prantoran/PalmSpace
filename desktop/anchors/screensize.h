#ifndef SCREENSIZE_H
#define SCREENSIZE_H


class ScreenSize {
    public:
    choices::eScreenSize size;
    
    void setMinWidthHeight(
        double &min_width, 
        double &min_height, 
        int max_width, 
        int max_height) {
        switch (size) {
            case choices::SMALL:
                min_width = max_width/4;
                min_height = max_height/4;
                break;
            case choices::LARGE:
                min_width = (3*max_width)/7;
                min_height = (3*max_height)/7;
                break; 
            case choices::FULL:
                min_width = max_width;
                min_height = max_height;
                break;
            default:
                std::cout << "ERROR anchors/anchor.h ScreenSize setMinWidthHeight() size invalid\n";
                min_width = -1;
                min_height = -1;
        }

    }

    bool isFull() {
        return size == choices::FULL;
    }

    bool isCentered() {
        // TODO 2020/11/07: get choice from UI
        return true;
    }

};


#endif