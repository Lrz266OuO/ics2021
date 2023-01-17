/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-11 02:09:44
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-16 19:47:36
 * @Description:  
 */
#include "sdb.h"

#define NR_WP 32

// 一开始没有初始化监视点，false为未初始化，true为已初始化
bool already_init = false;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  // 组织使用中的监视点结构
  // head = NULL;
  head = wp_pool;
  // 组织空闲的监视点的结构
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(char *args, word_t value) {
  if (!already_init) {
    already_init = true;
    init_wp_pool();
  }
  if (free_ == NULL) {
		printf("Warning: There is no free watchpoint left\n");
		return NULL;
	}

	WP *watchpoint = NULL;
	watchpoint = free_;
	free_ = free_->next;
	// watchpoint->next = head;
	strcpy(watchpoint->expression, args);
	watchpoint->value = value;
	// head = watchpoint;
	return watchpoint;
  
}

void free_wp(int number) {
  // TODO: 删除watchpoint并将该节点放至free_
  return;
}

void display_wp() {
  if (!already_init) {
    printf("Warning: You haven't initialized the watchpoint pool\n");
  }
  else if (head == free_) {
    printf("Warning: The watchpoint pool is empty\n");
  }
  else {
    WP *cur = head;
    printf("NO\texpr\tvalue\n");
    while (cur != free_) {
      printf("%d\t", cur->NO);
      printf("%s\t", cur->expression);
      printf("%u\n", cur->value);
      cur = cur->next;
    }
  }
  return;
}

