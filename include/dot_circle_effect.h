#pragma once
#include "effect.h"

class DotCircleEffect : public Effect
{
public:
    DotCircleEffect(int width, int height, cv::Point2f dotStart = cv::Point2f(-1.0f, -1.0f));
    void apply(cv::Mat& frame, double progress) override;

private:
    int         m_width;
    int         m_height;
    cv::Point2f m_dotStart;
    cv::Point2f m_center;
    int         m_dotRadius;
    int         m_circleRadius;
    int         m_circleThickness;

    void drawWhiteCircle(cv::Mat& frame);
    void drawBlackDot(cv::Mat& frame, cv::Point2f pos);
    void drawGlowCircle(cv::Mat& frame, cv::Point2f center, int radius, cv::Scalar color, int thickness, float glowIntensity);
};
