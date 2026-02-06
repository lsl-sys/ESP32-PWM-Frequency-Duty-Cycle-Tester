#ifndef FREQUENCY_METER_H
#define FREQUENCY_METER_H

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include "soc/rtc.h"
#include <Arduino.h>

class FrequencyMeter {
private:
    int gpioPin;
    mcpwm_unit_t mcpwmUnit;
    mcpwm_capture_channel_id_t capChannel;
    
    // 捕获变量
    volatile uint32_t rise_time;
    volatile uint32_t high_time;
    volatile uint32_t period;
    volatile bool new_data_ready;
    volatile bool is_first_capture;
    
    // 临界区互斥锁
    portMUX_TYPE timerMux;
    
    // 回调函数
    static bool cap_ISR_cb(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, 
                          const cap_event_data_t *edata, void *user_data);
    
    // 捕获配置
    mcpwm_capture_config_t cap_config;
    
public:
    /**
     * 构造函数
     * @param gpio 输入引脚
     * @param unit MCPWM单元
     * @param channel 捕获通道
     */
    FrequencyMeter(int gpio = 1, mcpwm_unit_t unit = MCPWM_UNIT_0, 
                  mcpwm_capture_channel_id_t channel = MCPWM_SELECT_CAP0);
    
    /**
     * 初始化函数
     * @return 是否初始化成功
     */
    bool begin();
    
    /**
     * 检查是否有新数据
     * @return 是否有新数据
     */
    bool isNewDataReady();
    
    /**
     * 获取频率
     * @return 频率值（Hz）
     */
    float getFrequency();
    
    /**
     * 获取占空比
     * @return 占空比（%）
     */
    float getDutyCycle();
    
    /**
     * 重置数据标志
     */
    void resetDataReady();
    
    /**
     * 停止测量
     */
    void end();
};

#endif // FREQUENCY_METER_H