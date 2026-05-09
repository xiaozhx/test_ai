# 程序架构说明

## 工程结构

```
test_ai/
├── CMakeLists.txt              # CMake 构建配置
├── build.sh                    # Linux 一键构建脚本
├── include/
│   ├── effect.h                # 效果基类接口
│   ├── light_band_effect.h     # 第一阶段：光带效果头文件
│   ├── dot_circle_effect.h     # 第二阶段：圆点圆圈效果头文件
│   └── renderer.h              # 渲染器及配置结构头文件
└── src/
    ├── main.cpp                # 程序入口，参数解析
    ├── renderer.cpp            # 渲染器主逻辑，阶段调度
    ├── light_band_effect.cpp   # 第一阶段：移动光带效果实现
    └── dot_circle_effect.cpp   # 第二阶段：圆点移动效果实现
```

---

## 核心设计说明

### 架构设计

- `effect.h` 定义抽象基类 `Effect`，所有渲染效果通过统一接口 `apply(frame, progress)` 调用
  - `frame`：当前视频帧（`cv::Mat`），效果直接在其上修改
  - `progress`：归一化进度值，范围 `[0.0, 1.0]`，表示当前帧在本阶段的进度
- `renderer.cpp` 负责逐帧读取原视频，根据帧序号计算所属阶段及阶段内进度，分发给对应效果处理器

### 渲染配置（RendererConfig）

| 字段 | 类型 | 说明 |
|------|------|------|
| `inputPath` | string | 输入 mp4 文件路径 |
| `outputPath` | string | 输出 mp4 文件路径 |
| `phase1Ratio` | double | 第一阶段占原视频总帧数的比例，范围 0~1 |
| `phase2Ratio` | double | 第二阶段占原视频总帧数的比例，范围 0~1 |
| `imageDurationSec` | double | 第三阶段追加图片的持续秒数 |
| `imagePath` | string | 第三阶段追加的 jpg 图片路径 |

> `phase1Ratio + phase2Ratio` 之和不超过 1.0，剩余帧不做特殊处理直接输出原始画面

---

### 第一阶段 — 移动光带效果（`light_band_effect.cpp`）

- 预定义 **4 条光带**，每条具有独立的初始位置、移动速度、宽度、倾斜角度、颜色（蓝白色调）和亮度强度
- 使用 **smoothstep 平滑插值** 生成光带软边缘遮罩，避免硬边
- 光带沿倾斜方向从左向右以不同速度扫过画面，形成动态扫光效果
- 整体进度使用 **ease in-out 二次缓动**，使光带运动开始和结束更自然
- 每帧叠加 **暗角（vignette）** 效果，压暗四角，增强画面层次感

**光带参数表：**

| 编号 | 初始位置 | 速度 | 宽度 | 倾斜角 | 颜色 | 强度 |
|------|---------|------|------|--------|------|------|
| 1 | 0.00 | 0.6 | 8% | 25° | (180,220,255) | 0.85 |
| 2 | 0.30 | 0.5 | 5% | 20° | (150,200,255) | 0.65 |
| 3 | 0.60 | 0.7 | 6% | 30° | (200,230,255) | 0.75 |
| 4 | 0.15 | 0.4 | 4% | 15° | (160,210,255) | 0.55 |

---

### 第二阶段 — 圆点移动效果（`dot_circle_effect.cpp`）

- **白色圆圈**：固定在画面中心，带有 5 层渐变发光（glow）效果，增强视觉焦点感
- **黑色圆点**：从画面左上角（宽度 20%，高度 25%）位置出发，逐渐移动到画面中心
- 运动曲线使用 **三次缓动（cubic ease in-out）**，前后慢、中间快，运动更自然
- 圆点带有 **4 帧渐隐运动拖尾**，每帧拖尾半径和透明度递减，增强速度感
- 圆点进入中心区域（距中心 < 圆圈半径 15%）时，与圆圈**平滑融合消失**
- 阶段前 10% 和后 10% 设置缓冲区，避免效果突然出现或消失

**运动参数：**

| 参数 | 值 |
|------|-----|
| 起始位置 | (width×20%, height×25%) |
| 终点位置 | 画面中心 |
| 圆点半径 | min(width,height) / 20 |
| 圆圈半径 | min(width,height) / 6 |
| 圆圈线宽 | min(width,height) / 80 |

---

### 第三阶段 — 图片追加（`renderer.cpp`）

- 原视频所有帧处理完毕后，在视频末尾**追加额外帧**
- 使用 `cv::imread` 加载指定 jpg 图片，`cv::resize`（Lanczos4 插值）缩放至与视频相同分辨率
- 按 `imageDurationSec × fps` 计算追加帧数，循环写入同一图片帧
- 若图片加载失败则打印警告并跳过，不影响前两阶段输出

**追加帧数计算：**
```
imageFrameCount = imageDurationSec × fps
```

---

## 编译与运行

### 依赖

```bash
# Ubuntu/Debian
sudo apt-get install libopencv-dev cmake build-essential
```

### 构建

```bash
chmod +x build.sh
./build.sh
```

### 运行

```bash
# 使用默认参数（前50%光带 + 后50%圆点 + 末尾追加3秒图片）
./build/video_renderer input.mp4

# 完整参数
./build/video_renderer <输入.mp4> [输出.mp4] [第一阶段比例] [第二阶段比例] [图片时长秒] [图片路径]

# 示例：前40%光带 + 后60%圆点 + 末尾追加5秒图片
./build/video_renderer input.mp4 output.mp4 0.4 0.6 5.0 overlay.jpg
```

### 参数说明

| 参数 | 默认值 | 说明 |
|------|--------|------|
| 输入视频 | 必填 | 输入 mp4 文件路径 |
| 输出视频 | output.mp4 | 输出 mp4 文件路径 |
| 第一阶段比例 | 0.5 | 光带效果占总时长比例 |
| 第二阶段比例 | 0.5 | 圆点移动效果占总时长比例 |
| 图片时长秒 | 3.0 | 末尾追加图片的持续秒数 |
| 图片路径 | overlay.jpg | 末尾追加的 jpg 图片路径 |
