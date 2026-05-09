#include "light_band_effect.h"

#include <cmath>

LightBandEffect::LightBandEffect(int width, int height)
    : m_width(width)
    , m_height(height)
{

    m_bands = {
        {0.0f, 0.6f, 0.08f, 25.0f, cv::Scalar(180, 220, 255), 0.85f},
        {0.3f, 0.5f, 0.05f, 20.0f, cv::Scalar(150, 200, 255), 0.65f},
        {0.6f, 0.7f, 0.06f, 30.0f, cv::Scalar(200, 230, 255), 0.75f},
        {0.15f, 0.4f, 0.04f, 15.0f, cv::Scalar(160, 210, 255), 0.55f},
    };
}

cv::Mat LightBandEffect::createBandMask(const LightBand& band, double progress)
{
    cv::Mat mask = cv::Mat::zeros(m_height, m_width, CV_32FC1);

    double pos = std::fmod(band.position + band.speed * progress, 1.2) - 0.1;
    float  cx  = static_cast<float>(pos * (m_width + m_width * 0.2) - m_width * 0.1);

    float angleRad = band.angle * static_cast<float>(CV_PI) / 180.0f;
    float tanA     = std::tan(angleRad);
    float halfW    = band.width * m_width * 0.5f;

    for (int y = 0; y < m_height; y++) {
        float xCenter = cx + tanA * (y - m_height * 0.5f);
        for (int x = 0; x < m_width; x++) {
            float dist = std::abs(x - xCenter);
            if (dist < halfW) {
                float t              = 1.0f - dist / halfW;
                float val            = t * t * (3.0f - 2.0f * t);
                mask.at<float>(y, x) = val;
            }
        }
    }
    return mask;
}

void LightBandEffect::drawBand(cv::Mat& frame, const LightBand& band, double progress)
{
    cv::Mat mask = createBandMask(band, progress);

    std::vector<cv::Mat> channels;
    cv::split(frame, channels);

    for (int c = 0; c < 3; c++) {
        cv::Mat ch;
        channels[c].convertTo(ch, CV_32FC1);
        float   colorVal = static_cast<float>(band.color[c]);
        cv::Mat blended  = ch + mask * colorVal * band.intensity;
        cv::threshold(blended, blended, 255.0, 255.0, cv::THRESH_TRUNC);
        blended.convertTo(channels[c], CV_8UC1);
    }

    cv::merge(channels, frame);
}

void LightBandEffect::apply(cv::Mat& frame, double progress)
{
    double easedProgress = progress < 0.5 ? 2.0 * progress * progress : 1.0 - std::pow(-2.0 * progress + 2.0, 2.0) / 2.0;

    for (const auto& band : m_bands) {
        drawBand(frame, band, easedProgress);
    }

    float       vignetteStrength = 0.3f;
    cv::Mat     vignette(m_height, m_width, CV_32FC1);
    cv::Point2f center(m_width / 2.0f, m_height / 2.0f);
    float       maxDist = std::sqrt(center.x * center.x + center.y * center.y);
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            float dx                 = x - center.x;
            float dy                 = y - center.y;
            float dist               = std::sqrt(dx * dx + dy * dy) / maxDist;
            vignette.at<float>(y, x) = 1.0f - vignetteStrength * dist * dist;
        }
    }

    std::vector<cv::Mat> channels;
    cv::split(frame, channels);
    for (auto& ch : channels) {
        cv::Mat chF;
        ch.convertTo(chF, CV_32FC1);
        cv::multiply(chF, vignette, chF);
        chF.convertTo(ch, CV_8UC1);
    }
    cv::merge(channels, frame);
}
