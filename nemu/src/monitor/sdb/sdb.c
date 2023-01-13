#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/paddr.h>
#include "sdb.h"
#include "utils.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  // 优雅地退出
  nemu_state.state = NEMU_QUIT;
  return -1;
}

// 单步执行
static int cmd_si(char *args) {
  // 取出si后的参数
  char *arg = strtok(NULL, " ");
  // 无参数，缺省为1
  int N = 1;
  if (arg != NULL) {
    // 这里如果si之后输入的是字母，默认参数为1
    sscanf(arg, "%d", &N);
  }
  cpu_exec(N);
  return 0;
}

// 打印程序状态
static int cmd_info(char *args) {
  char *arg = strtok(NULL, " ");
  // 打印寄存器状态
  if (strcmp(arg, "r") == 0) isa_reg_display();
  // 打印监视点信息
  else if (strcmp(arg, "w") == 0) {

  }
  else printf("info: Unknown command '%s'\n", arg);
  return 0;
}

static int cmd_x(char *args) {
  if(args == NULL) printf("x: Unknown command\n");
  // 输出n个4字节
  // TODO: 扫描内存部分的表达式先进行简化，暂且保证EXPR一定是一个十六进制数
  int N = -1;
  paddr_t EXPR = 0x80000000;
  sscanf(args, "%d%x", &N, &EXPR);
  int i, j;
  for (i = 0; i < N; i++) {
    printf("0x%8x: ", EXPR + i*4);
    for (j = 0; j < 4; j++) {
      uint8_t* position = guest_to_host(EXPR + i*4 + j);
      printf("%.2x ", *position);
    }
    printf("\n");
  }

  return 0;
}

static int cmd_p(char *args) {

}

static int cmd_w(char *args) {

}

static int cmd_d(char *args) {
  
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Execute N instructions in one step before pausing the program", cmd_si},
  { "info", "Print registers or monitoring points", cmd_info},
  { "x", "Calculate the expression, and use it as the beginning to scan memory", cmd_x},
  { "p", "Calculate the expression", cmd_p},
  { "w", "Set up the monitoring points", cmd_w},
  { "d", "Delete the monitoring point whose serial number is N", cmd_d},

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("help: Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
