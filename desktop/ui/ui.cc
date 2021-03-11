


#include "ui.h"

/**
 * Draws a rectangle with rounded corners, the parameters are the same as in the OpenCV function @see rectangle();
 * @param cornerRadius A positive int value defining the radius of the round corners.
 */

namespace ui {
    void rounded_rectangle( cv::Mat& src, cv::Point topLeft, cv::Point bottomRight, const cv::Scalar lineColor, const int thickness, const int lineType , const int cornerRadius)
    {
        /* corners:
        * p1 - p2
        * |     |
        * p4 - p3
        */
        cv::Point p1 = topLeft;
        cv::Point p2 = cv::Point (bottomRight.x, topLeft.y);
        cv::Point p3 = bottomRight;
        cv::Point p4 = cv::Point (topLeft.x, bottomRight.y);

        // draw straight lines
        cv::line(src, cv::Point (p1.x+cornerRadius,p1.y), cv::Point (p2.x-cornerRadius,p2.y), lineColor, thickness, lineType);
        cv::line(src, cv::Point (p2.x,p2.y+cornerRadius), cv::Point (p3.x,p3.y-cornerRadius), lineColor, thickness, lineType);
        cv::line(src, cv::Point (p4.x+cornerRadius,p4.y), cv::Point (p3.x-cornerRadius,p3.y), lineColor, thickness, lineType);
        cv::line(src, cv::Point (p1.x,p1.y+cornerRadius), cv::Point (p4.x,p4.y-cornerRadius), lineColor, thickness, lineType);

        cv::rectangle(
                src,
                topLeft,
                bottomRight,
                lineColor,
                -1,
                cv::LINE_8,
                0
            );

        // draw arcs
        cv::ellipse( src, p1 + cv::Point(cornerRadius, cornerRadius), cv::Size( cornerRadius, cornerRadius ), 180.0, 0, 90, lineColor, thickness, lineType );
        cv::ellipse( src, p2 + cv::Point(-cornerRadius, cornerRadius), cv::Size( cornerRadius, cornerRadius ), 270.0, 0, 90, lineColor, thickness, lineType );
        cv::ellipse( src, p3 + cv::Point(-cornerRadius, -cornerRadius), cv::Size( cornerRadius, cornerRadius ), 0.0, 0, 90, lineColor, thickness, lineType );
        cv::ellipse( src, p4 + cv::Point(cornerRadius, -cornerRadius), cv::Size( cornerRadius, cornerRadius ), 90.0, 0, 90, lineColor, thickness, lineType );
    }


    void clear_rectangle(
        cv::Mat& src,
        cv::Point p1,
        cv::Point p2,
        cv::Point p3,
        cv::Point p4,
        cv::Scalar color
    ) {
        
        std::vector<cv::Point> contour = {p1, p2, p3, p4, p1};

        const cv::Point *pts = (const cv::Point*) cv::Mat(contour).data;
        int npts = cv::Mat(contour).rows;

        cv::polylines(
            src, 
            &pts, 
            &npts, 
            1, 
            false, 
            color, 
            3,
            CV_AA,
            0
        );
    }


    void clear_rect(
        cv::Mat& src,
        cv::Point topleft,
        cv::Point bottomright,
        cv::Scalar color) {

        clear_rectangle(
            src,
            topleft,
            cv::Point(topleft.x, bottomright.y),
            bottomright,
            cv::Point(bottomright.x, topleft.y),
            color
        );
    }
}
