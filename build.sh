#!/bin/bash
set -e

BUILD_DIR="build"

echo "=== 检查依赖 ==="
if ! pkg-config --exists opencv4 2>/dev/null && ! pkg-config --exists opencv 2>/dev/null; then
    echo "警告: 未检测到 OpenCV，请确保已安装 libopencv-dev"
    echo "  Ubuntu/Debian: sudo apt-get install libopencv-dev"
    echo "  CentOS/RHEL:   sudo yum install opencv-devel"
fi

echo "=== 创建构建目录 ==="
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "=== CMake 配置 ==="
cmake .. -DCMAKE_BUILD_TYPE=Release

echo "=== 编译 ==="
make -j"$(nproc)"

echo ""
echo "=== 构建成功 ==="
echo "可执行文件: $BUILD_DIR/video_renderer"
echo ""
echo "使用方法:"
echo "  ./$BUILD_DIR/video_renderer <输入.mp4> [输出.mp4] [第一阶段效果] [第一阶段比例] [第二阶段效果] [第二阶段比例] [图片时长秒] [图片路径] [圆点起始X] [圆点起始Y] [方框坐标...]"
echo ""
echo "参数说明:"
echo "  输入视频      : 输入 mp4 文件路径"
echo "  输出视频      : 输出 mp4 文件路径 (默认: output.mp4)"
echo "  第一阶段效果  : 效果名称 (默认: light_band)"
echo "  第一阶段比例  : 占总时长比例, 0~1 (默认: 0.5)"
echo "  第二阶段效果  : 效果名称 (默认: dot_circle)"
echo "  第二阶段比例  : 占总时长比例, 0~1 (默认: 0.5)"
echo "  图片时长秒    : 末尾追加图片的持续秒数 (默认: 3.0)"
echo "  图片路径      : 末尾追加的 jpg 图片路径 (默认: overlay.jpg)"
echo "  圆点起始X     : dot_circle 圆点起始位置 X 比例, 0~1 (默认: 0.2)"
echo "  圆点起始Y     : dot_circle 圆点起始位置 Y 比例, 0~1 (默认: 0.25)"
echo "  方框坐标      : box 效果的坐标，格式: x,y,z (默认: 0.5,0.5,1.0)"
echo ""
echo "可用效果名称:"
echo "  light_band     - 移动光带扫光效果"
echo "  dot_circle     - 黑色圆点向白色圆圈移动效果"
echo "  image_overlay  - 图片叠加混合效果"
echo "  box            - 根据坐标绘制方框效果"
echo "  none           - 无效果，直接输出原始画面"
echo ""
echo "示例:"
echo "  ./$BUILD_DIR/video_renderer input.mp4 output.mp4 light_band 0.5 dot_circle 0.5 3.0 overlay.jpg"
echo "  ./$BUILD_DIR/video_renderer input.mp4 output.mp4 dot_circle 0.6 none 0.0 5.0 photo.jpg"
echo "  ./$BUILD_DIR/video_renderer input.mp4 output.mp4 light_band 0.5 dot_circle 0.5 3.0 overlay.jpg 0.1 0.8"
echo "  ./$BUILD_DIR/video_renderer input.mp4 output.mp4 box 1.0 none 0.0 0.0 none.jpg -1 -1 0.3,0.3,1.0"
