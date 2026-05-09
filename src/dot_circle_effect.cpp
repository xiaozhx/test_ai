#include "dot_circle_effect.h"

#include <cmath>

DotCircleEffect::DotCircleEffect(int width, int height, cv::Point2f dotStart)
    : m_width(width)
    , m_height(height)
{
    m_center   = cv::Point2f(width / 2.0f, height / 2.0f);
    m_dotStart = (dotStart.x < 0.0f || dotStart.y < 0.0f)
                     ? cv::Point2f(width * 0.2f, height * 0.25f)
                     : cv::Point2f(dotStart.x * width, dotStart.y * height);
    m_dotRadius       = std::min(width, height) / 20;
    m_circleRadius    = std::min(width, height) / 6;
    m_circleThickness = std::max(2, std::min(width, height) / 80);
}

void DotCircleEffect::drawGlowCircle(cv::Mat& frame, cv::Point2f center, int radius, cv::Scalar color, int thickness, float glowIntensity)
{
    int glowLayers = 5;
    for (int i = glowLayers; i >= 1; i--) {
        float      alpha      = glowIntensity * (1.0f - static_cast<float>(i) / (glowLayers + 1));
        int        extraThick = thickness + i * 3;
        cv::Scalar glowColor(color[0] * alpha, color[1] * alpha, color[2] * alpha);
        cv::Mat    overlay = frame.clone();
        cv::circle(overlay, cv::Point(static_cast<int>(center.x), static_cast<int>(center.y)), radius, glowColor, extraThick, cv::LINE_AA);
        cv::addWeighted(frame, 1.0 - alpha * 0.3, overlay, alpha * 0.3, 0, frame);
    }
    cv::circle(frame, cv::Point(static_cast<int>(center.x), static_cast<int>(center.y)), radius, color, thickness, cv::LINE_AA);
}

void DotCircleEffect::drawWhiteCircle(cv::Mat& frame)
{
    drawGlowCircle(frame, m_center, m_circleRadius, cv::Scalar(255, 255, 255), m_circleThickness, 0.8f);
}

void DotCircleEffect::drawBlackDot(cv::Mat& frame, cv::Point2f pos)
{
    int ix = static_cast<int>(pos.x);
    int iy = static_cast<int>(pos.y);

    cv::Mat overlay = frame.clone();
    cv::circle(overlay, cv::Point(ix, iy), m_dotRadius + 3, cv::Scalar(80, 80, 80), -1, cv::LINE_AA);
    cv::addWeighted(frame, 0.5, overlay, 0.5, 0, frame);

    cv::circle(frame, cv::Point(ix, iy), m_dotRadius, cv::Scalar(0, 0, 0), -1, cv::LINE_AA);

    cv::circle(frame, cv::Point(ix - m_dotRadius / 4, iy - m_dotRadius / 4), std::max(1, m_dotRadius / 5), cv::Scalar(60, 60, 60), -1, cv::LINE_AA);
}

void DotCircleEffect::apply(cv::Mat& frame, double progress)
{
    double easedProgress;
    if (progress < 0.1) {
        easedProgress = 0.0;
    }
    else if (progress > 0.9) {
        easedProgress = 1.0;
    }
    else {
        double t      = (progress - 0.1) / 0.8;
        easedProgress = t < 0.5 ? 4.0 * t * t * t : 1.0 - std::pow(-2.0 * t + 2.0, 3.0) / 2.0;
    }

    float       dotX = m_dotStart.x + (m_center.x - m_dotStart.x) * static_cast<float>(easedProgress);
    float       dotY = m_dotStart.y + (m_center.y - m_dotStart.y) * static_cast<float>(easedProgress);
    cv::Point2f dotPos(dotX, dotY);

    float distToCenter = std::sqrt((dotX - m_center.x) * (dotX - m_center.x) + (dotY - m_center.y) * (dotY - m_center.y));

    drawWhiteCircle(frame);

    if (distToCenter > m_circleRadius * 0.15f) {
        drawBlackDot(frame, dotPos);
    }
    else {
        float   mergeAlpha = 1.0f - distToCenter / (m_circleRadius * 0.15f);
        cv::Mat overlay    = frame.clone();
        drawBlackDot(overlay, dotPos);
        cv::addWeighted(frame, mergeAlpha, overlay, 1.0f - mergeAlpha, 0, frame);
    }

    if (progress > 0.05) {
        float trailAlpha = 0.3f;
        int   trailCount = 4;
        for (int i = 1; i <= trailCount; i++) {
            double trailProgress = std::max(0.0, progress - i * 0.03);
            double trailEased;
            if (trailProgress < 0.1) {
                trailEased = 0.0;
            }
            else if (trailProgress > 0.9) {
                trailEased = 1.0;
            }
            else {
                double t   = (trailProgress - 0.1) / 0.8;
                trailEased = t < 0.5 ? 4.0 * t * t * t : 1.0 - std::pow(-2.0 * t + 2.0, 3.0) / 2.0;
            }
            float tx = m_dotStart.x + (m_center.x - m_dotStart.x) * static_cast<float>(trailEased);
            float ty = m_dotStart.y + (m_center.y - m_dotStart.y) * static_cast<float>(trailEased);

            float alpha       = trailAlpha * (1.0f - static_cast<float>(i) / (trailCount + 1));
            int   trailRadius = static_cast<int>(m_dotRadius * (1.0f - static_cast<float>(i) * 0.15f));
            if (trailRadius > 0) {
                cv::Mat trailOverlay = frame.clone();
                cv::circle(trailOverlay, cv::Point(static_cast<int>(tx), static_cast<int>(ty)), trailRadius, cv::Scalar(20, 20, 20), -1, cv::LINE_AA);
                cv::addWeighted(frame, 1.0 - alpha, trailOverlay, alpha, 0, frame);
            }
        }
    }
}
