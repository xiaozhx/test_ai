#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class Effect
{
public:
    virtual ~Effect()                                   = default;
    virtual void apply(cv::Mat& frame, double progress) = 0;
};
