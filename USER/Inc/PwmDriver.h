//
// Created by crepusculumx on 2021/12/19.
//

#ifndef ROBOTMOTORCONTROL_USER_INC_PWMDRIVER_H_
#define ROBOTMOTORCONTROL_USER_INC_PWMDRIVER_H_

#include "main.h"

typedef struct PwmDriver {
  // pwm var

  int rate; // 输出频率
  double duty_ratio; // 占空比

  // timer setting

  TIM_HandleTypeDef *htim;
  uint32_t channel;
  uint16_t prescaler;
  uint64_t max_counter_priod;

  // timer var

  uint16_t counter_priod; // 计数器触发值

  // clock setting

  int clock_rate; //MHz
} PwmDriver;

/*
 * 初始化并开始输出pwm
 * */
void PwmDriver_init(PwmDriver *pwm_driver,
                    uint16_t clock_rate,
                    TIM_HandleTypeDef *htim,
                    uint32_t channel,
                    uint16_t prescaler,
                    uint64_t max_counter_priod);

/*
 * PWM start
 * */
void PwmDriver_start(PwmDriver *pwm_driver);

/*
 * PWM stop
 * */
void PwmDriver_stop(PwmDriver *pwm_driver);

/*
 * 设置频率和占空比
 * */
void PwmDriver_set_pwm(PwmDriver *pwm_driver, int rate, double duty_ratio);

#endif //ROBOTMOTORCONTROL_USER_INC_PWMDRIVER_H_
