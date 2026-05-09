#include "box_effect.h"
#include "dot_circle_effect.h"
#include "effect_factory.h"
#include "image_overlay_effect.h"
#include "light_band_effect.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>

const std::map<std::string, EffectFactory::EffectInfo>& EffectFactory::getEffectMap()
{
    static const std::map<std::string, EffectInfo> effectMap = {
        {"light_band",
         {[](int w, int h, const std::string&, cv::Point2f, const BoxCoord&) { return std::make_unique<LightBandEffect>(w, h); },
          "移动光带扫光效果"}},
        {"dot_circle",
         {[](int w, int h, const std::string&, cv::Point2f dotStart, const BoxCoord&) { return std::make_unique<DotCircleEffect>(w, h, dotStart); },
          "黑色圆点向白色圆圈移动效果"}},
        {"image_overlay",
         {[](int w, int h, const std::string& imagePath, cv::Point2f, const BoxCoord&) {
              return std::make_unique<ImageOverlayEffect>(w, h, imagePath);
          },
          "图片叠加混合效果"}},
        {"box",
         {[](int w, int h, const std::string&, cv::Point2f, const BoxCoord& coord) { return std::make_unique<BoxEffect>(w, h, coord); },
          "根据坐标绘制方框效果"}},
        {"none", {[](int, int, const std::string&, cv::Point2f, const BoxCoord&) { return nullptr; }, "无效果，直接输出原始画面"}}};
    return effectMap;
}

std::unique_ptr<Effect> EffectFactory::create(const std::string& name, int width, int height, const std::string& imagePath, cv::Point2f dotStart,
                                              const BoxCoord& boxCoord)
{
    if (name.empty()) {
        return nullptr;
    }

    const auto& effectMap = getEffectMap();
    auto        it        = effectMap.find(name);
    if (it != effectMap.end()) {
        return it->second.creator(width, height, imagePath, dotStart, boxCoord);
    }

    throw std::invalid_argument("未知的效果名称: " + name);
}

bool EffectFactory::isValid(const std::string& name)
{
    if (name.empty()) {
        return true;
    }
    const auto& effectMap = getEffectMap();
    return effectMap.find(name) != effectMap.end();
}

void EffectFactory::printAvailable()
{
    std::cout << "可用效果名称:\n";
    const auto& effectMap = getEffectMap();
    for (const auto& [name, info] : effectMap) {
        std::cout << "  " << std::left << std::setw(15) << name << " - " << info.description << "\n";
    }
}
