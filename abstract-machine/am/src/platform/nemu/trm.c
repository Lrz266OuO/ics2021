/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-17 02:29:32
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-28 03:55:02
 * @Description:  
 */
#include <am.h>
#include <nemu.h>

extern char _heap_start;
int main(const char *args);

Area heap = RANGE(&_heap_start, PMEM_END);
#ifndef MAINARGS
#define MAINARGS ""
#endif
static const char mainargs[] = MAINARGS;

void putch(char ch) {
  outb(SERIAL_PORT, ch);
}

void halt(int code) {
  nemu_trap(code);

  // should not reach here
  while (1);
}

void _trm_init() {
  int ret = main(mainargs);
  halt(ret);
}
