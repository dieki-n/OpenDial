#include <arduino.h>

#include "encoder.h"
#include "driver/pcnt.h"



void RotaryEncoder::init(){
    pcnt_config_t pcnt_config_a; 
    
    
        // Set PCNT input signal and control GPIOs
        pcnt_config_a.pulse_gpio_num = PIN_ENC_A;
        pcnt_config_a.ctrl_gpio_num = PIN_ENC_B;
        
        pcnt_config_a.unit = PCNT_UNIT_0;
        pcnt_config_a.channel = PCNT_CHANNEL_0;
  
        // What to do on the positive / negative edge of pulse input?
        pcnt_config_a.pos_mode = PCNT_COUNT_DIS;   // Count up on the positive edge
        pcnt_config_a.neg_mode = PCNT_COUNT_INC;   // Keep the counter value on the negative edge
        // What to do when control input is low or high?
        pcnt_config_a.lctrl_mode = PCNT_MODE_KEEP; // Reverse counting direction if low
        pcnt_config_a.hctrl_mode = PCNT_MODE_REVERSE;    // Keep the primary counter mode if high
        // Set the maximum and minimum limit values to watch
        pcnt_config_a.counter_h_lim = INT16_MAX;
        pcnt_config_a.counter_l_lim = INT16_MIN;
    
  
  
    pcnt_unit_config(&pcnt_config_a);
  
    pcnt_set_filter_value(PCNT_UNIT_0, 1023);  // Filter Runt Pulses
    pcnt_filter_enable(PCNT_UNIT_0);

    pinMode(PIN_ENC_A, INPUT_PULLUP);
    pinMode(PIN_ENC_B, INPUT);
  
    pcnt_counter_pause(PCNT_UNIT_0); // Initial PCNT init
    pcnt_counter_clear(PCNT_UNIT_0);
    pcnt_counter_resume(PCNT_UNIT_0);
}

int16_t RotaryEncoder::getPosition(){
  int16_t encoder_count;
  pcnt_get_counter_value(PCNT_UNIT_0, &encoder_count);  
  int16_t diff = encoder_count - last_encoder_count;
  last_encoder_count = encoder_count;
  return diff;
  
}
