/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-17 02:29:32
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-29 01:17:41
 * @Description:  
 */
#include <am.h>
#include <nemu.h>
#include <klib.h>

static uint64_t system_init_time;

void __am_timer_init() {
  system_init_time = (((uint64_t)inl(RTC_ADDR + 4) << 32 ) + (uint64_t)inl(RTC_ADDR));
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  // uptime->us = 0;
  uptime->us = (((uint64_t)inl(RTC_ADDR + 4) << 32 ) + (uint64_t)inl(RTC_ADDR)) - system_init_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
