/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-17 02:29:32
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-29 05:10:38
 * @Description:  
 */
#include <am.h>
#include <nemu.h>
#include <klib.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t keycode = inl(KBD_ADDR);
  // kbd->keydown = 0;
  // kbd->keycode = AM_KEY_NONE;
  kbd->keydown = keycode & KEYDOWN_MASK;
  kbd->keycode = keycode & (~(KEYDOWN_MASK));
}
