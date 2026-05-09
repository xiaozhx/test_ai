#include "box_effect.h"

#include <cmath>

BoxEffect::BoxEffect(int width, int height, const BoxCoord& coord)
    : m_width(width)
    , m_height(height)
    , m_coord(coord)
{
}

void BoxEffect::drawBox(cv::Mat& frame, double progress)
{
    int centerX = static_cast<int>(m_coord.x * m_width);
    int centerY = static_cast<int>(m_coord.y * m_height);

    float scale   = m_coord.z * (0.5f + 0.5f * static_cast<float>(progress));
    int   baseSize = std::min(m_width, m_height) / 10;
    int   boxSize  = static_cast<int>(baseSize * scale);

    int x1 = centerX - boxSize / 2;
    int y1 = centerY - boxSize / 2;
    int x2 = centerX + boxSize / 2;
    int y2 = centerY + boxSize / 2;

    x1 = std::max(0, std::min(x1, m_width - 1));
    y1 = std::max(0, std::min(y1, m_height - 1));
    x2 = std::max(0, std::min(x2, m_width - 1));
    y2 = std::max(0, std::min(y2, m_height - 1));

    float      alpha     = 0.3f + 0.7f * static_cast<float>(progress);
    int        intensity = static_cast<int>(255 * alpha);
    cv::Scalar color(0, intensity, 0);

    int thickness = std::max(2, static_cast<int>(boxSize / 20));
    cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), color, thickness, cv::LINE_AA);

    cv::Mat overlay = frame.clone();
    cv::rectangle(overlay, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 50, 0), -1);
    cv::addWeighted(frame, 1.0 - alpha * 0.2, overlay, alpha * 0.2, 0, frame);

    // std::string label = "(" + std::to_string(static_cast<int>(m_coord.x * 100)) + "%, " +
    //                     std::to_string(static_cast<int>(m_coord.y * 100)) + "%, z=" +
    //                     std::to_string(m_coord.z).substr(0, 4) + ")";

    // int      baseline = 0;
    // cv::Size textSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
    // int      textX    = centerX - textSize.width / 2;
    // int      textY    = y1 - 10;
    // if (textY < textSize.height) {
    //     textY = y2 + textSize.height + 10;
    // }

    // cv::putText(frame, label, cv::Point(textX, textY), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
}

void BoxEffect::apply(cv::Mat& frame, double progress)
{
    drawBox(frame, progress);
}
