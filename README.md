# ESP32 频率与占空比测量库

这是一个基于 ESP32/ESP32S3 的频率和占空比测量库，使用 MCPWM 捕获功能实现高精度测量。

## 功能特性

- 高精度测量：使用 MCPWM 硬件捕获，精度可达纳秒级
- 实时响应：中断驱动，无轮询延迟
- 易于使用：封装为简洁的类接口
- 可配置性：支持自定义 GPIO 引脚
- 稳定性：包含临界区保护，确保数据完整性

## 硬件要求

- ESP32 或 ESP32S3 开发板
- 待测信号源
- 面包板和连接线

## 软件要求

- Arduino IDE 或 PlatformIO
- ESP32 开发框架

## 安装方法

1. 将 `src/FrequencyMeter.h` 和 `src/FrequencyMeter.cpp` 文件复制到你的项目目录中
2. 在你的代码中包含头文件：`#include "FrequencyMeter.h"`

## 快速开始

### 基本示例

```cpp
#include <Arduino.h>
#include "FrequencyMeter.h"

// 创建频率计实例
FrequencyMeter freqMeter(1); // 使用GPIO 1作为输入

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT); // 内置LED
  
  // 初始化频率计
  if (freqMeter.begin()) {
    Serial.println("频率测量已启动");
  }
}

void loop() {
  if (freqMeter.isNewDataReady()) {
    float frequency = freqMeter.getFrequency();
    float dutyCycle = freqMeter.getDutyCycle();
    Serial.printf("频率: %.2f Hz, 占空比: %.2f%%\n", frequency, dutyCycle);
  }
  delay(100);
}
```

## API 参考

### 构造函数

```cpp
FrequencyMeter(int gpio = 1);
```
- `gpio`：输入引脚编号，默认为 1

### 初始化

```cpp
bool begin();
```
- 返回值：是否初始化成功

### 数据检查

```cpp
bool isNewDataReady();
```
- 返回值：是否有新的测量数据

### 获取频率

```cpp
float getFrequency();
```
- 返回值：频率值，单位为 Hz

### 获取占空比

```cpp
float getDutyCycle();
```
- 返回值：占空比值，单位为 %

## 测试结果

该库已在 ESP32S3 上测试通过，测量范围为 1Hz - 1MHz，精度高，稳定性好。

## 注意事项

- 确保输入信号电平与 ESP32 的 GPIO 兼容（3.3V）
- 避免信号噪声过大，可能导致测量不准确
- 并非所有 GPIO 引脚都支持 MCPWM 捕获功能，请参考 ESP32 数据手册
- 建议测量范围为 1Hz - 1MHz，超出此范围可能影响精度

## 许可证

本项目采用 MIT 许可证。

## 贡献

欢迎提交 Issue 和 Pull Request 来改进这个项目！