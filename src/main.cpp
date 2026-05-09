#include "effect_factory.h"
#include "renderer.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

static void printUsage(const char* progName)
{
    std::cout << "用法: " << progName
              << " <输入视频> [输出视频] [第一阶段效果] [第一阶段比例] [第二阶段效果] [第二阶段比例] [图片时长秒] [图片路径] [圆点起始X] [圆点起始Y] "
                 "[方框坐标]\n"
              << "  输入视频      : 输入 mp4 文件路径\n"
              << "  输出视频      : 输出 mp4 文件路径 (默认: output.mp4)\n"
              << "  第一阶段效果  : 效果名称 (默认: light_band)\n"
              << "  第一阶段比例  : 占总时长比例, 0~1 (默认: 0.5)\n"
              << "  第二阶段效果  : 效果名称 (默认: dot_circle)\n"
              << "  第二阶段比例  : 占总时长比例, 0~1 (默认: 0.5)\n"
              << "  图片时长秒    : 末尾追加图片的持续秒数 (默认: 3.0)\n"
              << "  图片路径      : 末尾追加的 jpg 图片路径 (默认: overlay.jpg)\n"
              << "  圆点起始X     : dot_circle 圆点起始位置 X 比例, 0~1 (默认: 0.2)\n"
              << "  圆点起始Y     : dot_circle 圆点起始位置 Y 比例, 0~1 (默认: 0.25)\n"
              << "  方框坐标      : box 效果的坐标，格式: x,y,z (默认: 0.5,0.5,1.0)\n\n";
    EffectFactory::printAvailable();
    std::cout << "\n示例:\n"
              << "  " << progName << " input.mp4 output.mp4 light_band 0.5 dot_circle 0.5 3.0 overlay.jpg\n"
              << "  " << progName << " input.mp4 output.mp4 dot_circle 0.6 none 0.0 5.0 photo.jpg\n"
              << "  " << progName << " input.mp4 output.mp4 light_band 0.5 dot_circle 0.5 3.0 overlay.jpg 0.1 0.8\n"
              << "  " << progName << " input.mp4 output.mp4 box 1.0 none 0.0 0.0 none.jpg -1 -1 0.3,0.3,1.0\n";
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    RendererConfig config;
    config.inputPath         = argv[1];
    config.outputPath        = (argc >= 3) ? argv[2] : "output.mp4";
    config.phase1.effectName = (argc >= 4) ? argv[3] : "light_band";
    config.phase1.ratio      = (argc >= 5) ? std::stod(argv[4]) : 0.5;
    config.phase2.effectName = (argc >= 6) ? argv[5] : "dot_circle";
    config.phase2.ratio      = (argc >= 7) ? std::stod(argv[6]) : 0.5;
    config.imageDurationSec  = (argc >= 8) ? std::stod(argv[7]) : 3.0;
    config.imagePath         = (argc >= 9) ? argv[8] : "overlay.jpg";
    config.dotStartX         = (argc >= 10) ? std::stof(argv[9]) : -1.0f;
    config.dotStartY         = (argc >= 11) ? std::stof(argv[10]) : -1.0f;

    if (argc >= 12) {
        std::string coordStr = argv[11];
        std::replace(coordStr.begin(), coordStr.end(), ',', ' ');
        std::istringstream iss(coordStr);
        if (!(iss >> config.boxCoord.x >> config.boxCoord.y >> config.boxCoord.z)) {
            std::cerr << "警告: 无法解析方框坐标: " << argv[11] << "\n";
        }
    }

    if (config.phase1.ratio < 0.0 || config.phase1.ratio > 1.0 || config.phase2.ratio < 0.0 || config.phase2.ratio > 1.0 ||
        config.phase1.ratio + config.phase2.ratio > 1.0) {
        std::cerr << "错误: 阶段比例无效，两者之和不能超过 1.0\n";
        return 1;
    }
    if (config.imageDurationSec < 0.0) {
        std::cerr << "错误: 图片时长不能为负数\n";
        return 1;
    }

    if (!EffectFactory::isValid(config.phase1.effectName) || !EffectFactory::isValid(config.phase2.effectName)) {
        std::cerr << "错误: 未知的效果名称\n\n";
        EffectFactory::printAvailable();
        return 1;
    }

    Renderer renderer(config);
    return renderer.run() ? 0 : 1;
}
