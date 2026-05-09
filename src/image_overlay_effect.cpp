#include "image_overlay_effect.h"
#include <iostream>
#include <cmath>

ImageOverlayEffect::ImageOverlayEffect(int width, int height, const std::string& imagePath)
    : m_width(width), m_height(height), m_loaded(false) {

    cv::Mat raw = cv::imread(imagePath, cv::IMREAD_COLOR);
    if (raw.empty()) {
        std::cerr << "警告: 无法加载图片: " << imagePath << std::endl;
        return;
    }

    cv::resize(raw, m_image, cv::Size(width, height), 0, 0, cv::INTER_LANCZOS4);
    m_loaded = true;
}

bool ImageOverlayEffect::isLoaded() const {
    return m_loaded;
}

void ImageOverlayEffect::blendImage(cv::Mat& frame, float alpha) {
    cv::addWeighted(frame, 1.0 - alpha, m_image, alpha, 0.0, frame);
}

void ImageOverlayEffect::apply(cv::Mat& frame, double progress) {
    if (!m_loaded) return;

    float alpha = 0.0f;

    if (progress < 0.2) {
        float t = static_cast<float>(progress / 0.2);
        alpha = t * t * (3.0f - 2.0f * t);
    } else if (progress < 0.8) {
        alpha = 1.0f;
    } else {
        float t = static_cast<float>((progress - 0.8) / 0.2);
        alpha = 1.0f - t * t * (3.0f - 2.0f * t);
    }

    blendImage(frame, alpha);
}
