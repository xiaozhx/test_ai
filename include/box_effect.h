#pragma once
#include "box_coord.h"
#include "effect.h"

class BoxEffect : public Effect
{
public:
    BoxEffect(int width, int height, const BoxCoord& coord = {0.5f, 0.5f, 1.0f});
    void apply(cv::Mat& frame, double progress) override;

private:
    int      m_width;
    int      m_height;
    BoxCoord m_coord;

    void drawBox(cv::Mat& frame, double progress);
};
