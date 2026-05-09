#include "effect_factory.h"
#include "renderer.h"

#include <iostream>
#include <memory>

Renderer::Renderer(const RendererConfig& config)
    : m_config(config)
{}

bool Renderer::processVideo(cv::VideoCapture& cap, cv::VideoWriter& writer, int totalFrames, int fps, int width, int height)
{
    int phase1End = static_cast<int>(totalFrames * m_config.phase1.ratio);
    int phase2End = static_cast<int>(totalFrames * (m_config.phase1.ratio + m_config.phase2.ratio));

    cv::Point2f dotStart(m_config.dotStartX, m_config.dotStartY);
    auto        effect1 = EffectFactory::create(m_config.phase1.effectName, width, height, m_config.imagePath, dotStart, m_config.boxCoord);
    auto        effect2 = EffectFactory::create(m_config.phase2.effectName, width, height, m_config.imagePath, dotStart, m_config.boxCoord);

    cv::Mat frame;
    int     frameIndex = 0;

    while (cap.read(frame)) {
        if (frame.empty()) break;

        if (frameIndex < phase1End) {
            if (effect1) {
                double progress = static_cast<double>(frameIndex) / phase1End;
                effect1->apply(frame, progress);
            }
        }
        else if (frameIndex < phase2End) {
            if (effect2) {
                double progress = static_cast<double>(frameIndex - phase1End) / (phase2End - phase1End);
                effect2->apply(frame, progress);
            }
        }

        writer.write(frame);

        frameIndex++;
        if (frameIndex % (fps * 5) == 0) {
            double percent = static_cast<double>(frameIndex) / totalFrames * 100.0;
            std::cout << "\r处理进度: " << static_cast<int>(percent) << "% (" << frameIndex << "/" << totalFrames << " 帧)" << std::flush;
        }
    }

    int imageFrameCount = static_cast<int>(m_config.imageDurationSec * fps);
    if (imageFrameCount > 0 && !m_config.imagePath.empty()) {
        cv::Mat raw = cv::imread(m_config.imagePath, cv::IMREAD_COLOR);
        if (raw.empty()) {
            std::cerr << "\n警告: 无法加载图片: " << m_config.imagePath << ", 跳过第三阶段" << std::endl;
        }
        else {
            cv::Mat imageFrame;
            cv::resize(raw, imageFrame, cv::Size(width, height), 0, 0, cv::INTER_LANCZOS4);
            std::cout << "\n追加图片帧: " << imageFrameCount << " 帧 (" << m_config.imageDurationSec << " 秒)" << std::endl;
            for (int i = 0; i < imageFrameCount; i++) {
                writer.write(imageFrame);
            }
        }
    }

    int writtenFrames = frameIndex + imageFrameCount;
    std::cout << "\r处理进度: 100% (" << writtenFrames << " 帧写入)" << std::endl;
    return true;
}

bool Renderer::run()
{
    cv::VideoCapture cap(m_config.inputPath);
    if (!cap.isOpened()) {
        std::cerr << "错误: 无法打开视频文件: " << m_config.inputPath << std::endl;
        return false;
    }

    int width       = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height      = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fps         = static_cast<int>(cap.get(cv::CAP_PROP_FPS));
    int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));

    std::cout << "视频信息: " << width << "x" << height << " @ " << fps << "fps, 共 " << totalFrames << " 帧" << std::endl;
    std::cout << "第一阶段 [" << m_config.phase1.effectName << "]: 前 " << static_cast<int>(m_config.phase1.ratio * 100) << "% 帧" << std::endl;
    std::cout << "第二阶段 [" << m_config.phase2.effectName << "]: 接下来 " << static_cast<int>(m_config.phase2.ratio * 100) << "% 帧" << std::endl;
    std::cout << "第三阶段 (图片追加): 末尾追加 " << m_config.imageDurationSec << " 秒, 图片: " << m_config.imagePath << std::endl;

    int             fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
    cv::VideoWriter writer(m_config.outputPath, fourcc, fps, cv::Size(width, height));
    if (!writer.isOpened()) {
        std::cerr << "错误: 无法创建输出视频文件: " << m_config.outputPath << std::endl;
        return false;
    }

    bool result = processVideo(cap, writer, totalFrames, fps, width, height);

    cap.release();
    writer.release();

    if (result) {
        std::cout << "渲染完成! 输出文件: " << m_config.outputPath << std::endl;
    }
    return result;
}
