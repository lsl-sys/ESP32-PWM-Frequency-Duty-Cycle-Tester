#include "FrequencyMeter.h"

FrequencyMeter::FrequencyMeter(int gpio, mcpwm_unit_t unit, mcpwm_capture_channel_id_t channel) {
    gpioPin = gpio;
    mcpwmUnit = unit;
    capChannel = channel;
    
    // 初始化变量
    rise_time = 0;
    high_time = 0;
    period = 0;
    new_data_ready = false;
    is_first_capture = true;
    
    // 初始化互斥锁
    timerMux = portMUX_INITIALIZER_UNLOCKED;
    
    // 初始化捕获配置
    cap_config.cap_edge = MCPWM_BOTH_EDGE;
    cap_config.cap_prescale = 1;
    cap_config.capture_cb = cap_ISR_cb;
    cap_config.user_data = this;
}

bool FrequencyMeter::cap_ISR_cb(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, 
                               const cap_event_data_t *edata, void *user_data) {
    
    FrequencyMeter *meter = (FrequencyMeter *)user_data;
    static bool next_is_rising = true;
    uint32_t current_cap = mcpwm_capture_signal_get_value(meter->mcpwmUnit, meter->capChannel);
    
    portENTER_CRITICAL_ISR(&meter->timerMux);
    
    if (next_is_rising) {
        // 当前是上升沿
        if (!meter->is_first_capture) {
            meter->period = current_cap - meter->rise_time;
            meter->new_data_ready = true;
        }
        meter->rise_time = current_cap;
        meter->is_first_capture = false;
    } else {
        // 当前是下降沿
        meter->high_time = current_cap - meter->rise_time;
    }
    
    next_is_rising = !next_is_rising;
    
    portEXIT_CRITICAL_ISR(&meter->timerMux);
    
    return false;
}

bool FrequencyMeter::begin() {
    // 初始化GPIO
    mcpwm_gpio_init(mcpwmUnit, MCPWM_CAP_0, gpioPin);
    gpio_pulldown_en((gpio_num_t)gpioPin);
    
    // 启用捕获通道
    esp_err_t err = mcpwm_capture_enable_channel(mcpwmUnit, capChannel, &cap_config);
    
    return (err == ESP_OK);
}

bool FrequencyMeter::isNewDataReady() {
    return new_data_ready;
}

float FrequencyMeter::getFrequency() {
    portENTER_CRITICAL(&timerMux);
    uint32_t current_period = period;
    new_data_ready = false;
    portEXIT_CRITICAL(&timerMux);
    
    if (current_period > 0) {
        // 计算频率 (80MHz -> 12.5ns 周期)
        return 1.0 / (current_period * 12.5e-9);
    }
    return 0.0;
}

float FrequencyMeter::getDutyCycle() {
    portENTER_CRITICAL(&timerMux);
    uint32_t current_period = period;
    uint32_t current_high = high_time;
    portEXIT_CRITICAL(&timerMux);
    
    if (current_period > 0) {
        return (float)current_high / current_period * 100.0;
    }
    return 0.0;
}

void FrequencyMeter::resetDataReady() {
    portENTER_CRITICAL(&timerMux);
    new_data_ready = false;
    portEXIT_CRITICAL(&timerMux);
}

void FrequencyMeter::end() {
    mcpwm_capture_disable_channel(mcpwmUnit, capChannel);
}