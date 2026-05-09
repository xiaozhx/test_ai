#pragma once
#include "box_coord.h"
#include "effect.h"

#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

struct PhaseConfig
{
    std::string effectName;
    double      ratio;
};

struct RendererConfig
{
    std::string inputPath;
    std::string outputPath;
    PhaseConfig phase1;
    PhaseConfig phase2;
    double      imageDurationSec;
    std::string imagePath;
    float       dotStartX = -1.0f;
    float       dotStartY = -1.0f;
    BoxCoord    boxCoord  = {-1.0f, -1.0f, -1.0f};
};

class Renderer
{
public:
    explicit Renderer(const RendererConfig& config);
    bool run();

private:
    RendererConfig m_config;

    bool processVideo(cv::VideoCapture& cap, cv::VideoWriter& writer, int totalFrames, int fps, int width, int height);
};
