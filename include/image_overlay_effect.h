#pragma once
#include "effect.h"
#include <string>

class ImageOverlayEffect : public Effect {
public:
    ImageOverlayEffect(int width, int height, const std::string& imagePath);
    void apply(cv::Mat& frame, double progress) override;
    bool isLoaded() const;

private:
    int m_width;
    int m_height;
    cv::Mat m_image;
    bool m_loaded;

    void blendImage(cv::Mat& frame, float alpha);
};
