#pragma once
#include "box_coord.h"
#include "effect.h"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class EffectFactory
{
public:
    using EffectCreator = std::function<std::unique_ptr<Effect>(int, int, const std::string&, cv::Point2f, const BoxCoord&)>;

    static std::unique_ptr<Effect> create(const std::string& name, int width, int height, const std::string& imagePath = "",
                                          cv::Point2f dotStart = cv::Point2f(-1.0f, -1.0f), const BoxCoord& boxCoord = {-1.0f, -1.0f, -1.0f});

    static bool isValid(const std::string& name);
    static void printAvailable();

private:
    struct EffectInfo
    {
        EffectCreator creator;
        std::string   description;
    };

    static const std::map<std::string, EffectInfo>& getEffectMap();
};
