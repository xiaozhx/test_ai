#pragma once
#include "effect.h"

#include <vector>

struct LightBand
{
    float      position;
    float      speed;
    float      width;
    float      angle;
    cv::Scalar color;
    float      intensity;
};

class LightBandEffect : public Effect
{
public:
    LightBandEffect(int width, int height);
    void apply(cv::Mat& frame, double progress) override;

private:
    int                    m_width;
    int                    m_height;
    std::vector<LightBand> m_bands;

    void    drawBand(cv::Mat& frame, const LightBand& band, double progress);
    cv::Mat createBandMask(const LightBand& band, double progress);
};
