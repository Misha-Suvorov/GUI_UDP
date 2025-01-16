#include "drawsymbols.h"
#include <opencv2/opencv.hpp>

namespace cv
{
void drawGuidanceMarker(InputOutputArray img, Rect2d rect, const Scalar& color,
                        int thickness, int lineLength, int lineType)
{
    //Up Left corner
    int lineLengthPx = (lineLength/100.0f)*rect.width;
    Point ptLeftUp = Point(rect.x, rect.y);
    cv::line(img, ptLeftUp, Point(ptLeftUp.x + lineLengthPx, ptLeftUp.y), color, thickness, lineType);
    cv::line(img, ptLeftUp, Point(ptLeftUp.x , ptLeftUp.y + lineLengthPx), color, thickness, lineType);

    //Up Right corner
    Point ptRightUp = Point(rect.x + rect.width, rect.y);
    cv::line(img, ptRightUp, Point(ptRightUp.x - lineLengthPx, ptRightUp.y), color, thickness, lineType);
    cv::line(img, ptRightUp, Point(ptRightUp.x , ptRightUp.y + lineLengthPx), color, thickness, lineType);

    //Down Right corner
    Point ptRightDown = Point(rect.x + rect.width, rect.y + rect.height);
    cv::line(img, ptRightDown, Point(ptRightDown.x - lineLengthPx, ptRightDown.y), color, thickness, lineType);
    cv::line(img, ptRightDown, Point(ptRightDown.x , ptRightDown.y - lineLengthPx), color, thickness, lineType);

    //Down Left corner
    Point ptLeftDown = Point(rect.x, rect.y + rect.height);
    cv::line(img, ptLeftDown, Point(ptLeftDown.x + lineLengthPx, ptLeftDown.y), color, thickness, lineType);
    cv::line(img, ptLeftDown, Point(ptLeftDown.x , ptLeftDown.y - lineLengthPx), color, thickness, lineType);

}



}
