/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-11 02:09:44
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-17 00:11:04
 * @Description:  
 */
#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* Add more members if necessary */
  char expression[1024];
  word_t value;

} WP;

word_t expr(char *e, bool *success);
WP* new_wp(char *args, word_t value);
void free_wp(int number);
void display_wp();
bool check_watchpoint(WP **point);

#endif
