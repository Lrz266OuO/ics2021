#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <string.h>
#include <assert.h>

enum {
  /* Add more token types */
  TK_NOTYPE = 256,  // space
  DEC_NUM,          // decimal numbers
  HEX_NUM,          // hexadecimal numbers
  REG,              // registers
  VAR,              // variates
  TK_EQ,            // equal
  TK_NEQ,           // not equal
  TK_AND,           // logical and
  TK_OR,            // logical or
  
  // TODO: optional task
  NEG_NUM,          // negative number(-)
  DEFER,            // deference(*)

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +",                  TK_NOTYPE}, // spaces
  {"\\+",                 '+'},       // plus
  {"\\-",                 '-'},       // minus or negative number
  {"\\*",                 '*'},       // multiply or deference
  {"\\/",                 '/'},       // divide
  {"%",                   '%'},       // remainder
  {"\\(",                 '('},       // left bracket
  {"\\)",                 ')'},       // right bracket
  {"[0-9]+",              DEC_NUM},   // decimal numbers
  {"0x[0-9a-f]{1,8}",     HEX_NUM},   // hexadecimal numbers
  {"\\$[0-9a-z]{1,3}",    REG},       // registers
  {"[A-Za-z_]{1,31}",     VAR},       // variates   
  {"==",                  TK_EQ},     // equal
  {"!=",                  TK_NEQ},    // not equal
  {"&&",                  TK_AND},    // logical and
  {"\\|\\|",              TK_OR},     // logical or
  {"!",                   '!'},       // logical not

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

// 词法分析
typedef struct token {
  int type;
  char str[32];
  int priority;   // 运算符优先级，数字越小优先级越高
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0; // 当前处理到的位置
  int i;
  regmatch_t pmatch;

  nr_token = 0;     // 已经被识别出的token数目

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case '+':
            tokens[nr_token].type = '+';
            tokens[nr_token].priority = 4;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case '-':
            // 需要考虑负数的问题
            // 减号在第一位或前面一个token不是数字/变量/右括号
            if ((nr_token==0) || 
                (nr_token!=0 && tokens[nr_token-1].type!=DEC_NUM 
                             && tokens[nr_token-1].type!=HEX_NUM 
                             && tokens[nr_token-1].type!=REG
                             && tokens[nr_token-1].type!=VAR
                             && tokens[nr_token-1].type!=')')
                ) {
              tokens[nr_token].type = NEG_NUM;
              tokens[nr_token].priority = 2;
            }
            else {
              tokens[nr_token].type = '-';
              tokens[nr_token].priority = 4;
            }
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case '*':
            // 需要考虑指针解引用的问题
            // 乘号在第一位或前面一个token不是数字/变量/右括号
            if ((nr_token==0) || 
                (nr_token!=0 && tokens[nr_token-1].type!=DEC_NUM 
                             && tokens[nr_token-1].type!=HEX_NUM 
                             && tokens[nr_token-1].type!=REG
                             && tokens[nr_token-1].type!=VAR
                             && tokens[nr_token-1].type!=')')
                ) {
              tokens[nr_token].type = DEFER;
              tokens[nr_token].priority = 2;
            }
            else {
              tokens[nr_token].type = '*';
              tokens[nr_token].priority = 3;
            }
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case '/':
            tokens[nr_token].type = '/';
            tokens[nr_token].priority = 3;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case '%':
            tokens[nr_token].type = '%';
            tokens[nr_token].priority = 3;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case '(':
            tokens[nr_token].type = '(';
            tokens[nr_token].priority = 1;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case ')':
            tokens[nr_token].type = ')';
            tokens[nr_token].priority = 1;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case DEC_NUM:
            tokens[nr_token].type = DEC_NUM;
            tokens[nr_token].priority = 16;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case HEX_NUM:
            tokens[nr_token].type = HEX_NUM;
            tokens[nr_token].priority = 16;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case REG:
            tokens[nr_token].type = REG;
            tokens[nr_token].priority = 16;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case VAR:
            tokens[nr_token].type = VAR;
            tokens[nr_token].priority = 16;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case TK_EQ:
            tokens[nr_token].type = TK_EQ;
            tokens[nr_token].priority = 7;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case TK_NEQ:
            tokens[nr_token].type = TK_NEQ;
            tokens[nr_token].priority = 7;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case TK_AND:
            tokens[nr_token].type = TK_AND;
            tokens[nr_token].priority = 11;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case TK_OR:
            tokens[nr_token].type = TK_OR;
            tokens[nr_token].priority = 12;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case '!':
            tokens[nr_token].type = '!';
            tokens[nr_token].priority = 2;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case TK_NOTYPE:
            break;
          default: 
            // TODO();
            printf("EXPR: Unknown operator\n");
            assert(0);
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}



// 递归求值
word_t eval(int left, int right, bool *success);

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // TODO();
  int left = 0, right = nr_token - 1;
  word_t result = eval(left, right, success);

  return result;
}

word_t eval(int left, int right, bool *success) {
  if (left > right) {
    /* Bad expression */
    *success = false;
    printf("Wrong expression: left > right\n");
    return 0;
  }

  else if (left == right) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number
     */
    if (tokens[left].type == DEC_NUM
     || tokens[left].type == HEX_NUM
     || tokens[left].type == REG
     || tokens[left].type == VAR
     ) {
      word_t val;
      switch (tokens[left].type) {
        case DEC_NUM:
          // strtol函数用于将string以十进制/十六进制的方式转为十进制数
          val = strtol(tokens[left].str, NULL, 10);
          break;
        case HEX_NUM:
          val = strtol(tokens[left].str, NULL, 16);
          break;
        case REG:
          // 使用isa_reg_str2val函数找到寄存器
          val = isa_reg_str2val(tokens[left].str, success);
          break;
        case VAR:
          break;
        default:
          break;
      }
      return val;
    }
    else {
      /* Bad expression */
      *success = false;
      printf("Wrong expression: unknown number or variate\n");
      return 0;
    }

  }

}

