#pragma once

#include <opencv2/core.hpp>

namespace cv
{

void drawGuidanceMarker(InputOutputArray img, Rect2d rect, const Scalar& color, int thickness = 1, int lineLength = 50,
                        int lineType = 1);
}
