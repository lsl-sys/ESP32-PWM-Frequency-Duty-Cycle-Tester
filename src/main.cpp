#include <Arduino.h>
#include "FrequencyMeter.h"

// 创建频率计实例
FrequencyMeter freqMeter(1); // 使用GPIO 1作为输入

void setup(){
  Serial.begin(115200);
  pinMode(2, OUTPUT);    // 内置 LED

  // 初始化频率计
  if (freqMeter.begin()) {
    Serial.println("MCPWM 频率+占空比测量已启动");
    Serial.println("GPIO1: 信号输入 | GPIO2: 中断指示灯");
  } else {
    Serial.println("频率计初始化失败");
  }
}

void loop(){
  if (freqMeter.isNewDataReady()) {  // 只在新数据到达时处理
    float frequency = freqMeter.getFrequency();
    float duty_cycle = freqMeter.getDutyCycle();
    
    if (frequency > 0) {
      Serial.printf("频率: %.2f Hz, 占空比: %.2f%%\n", frequency, duty_cycle);
    }
    
    digitalWrite(2, HIGH); // LED 指示数据处理
  }
  
  digitalWrite(2, LOW);  // 熄灭 LED
  delay(100);            // 100ms刷新率
}