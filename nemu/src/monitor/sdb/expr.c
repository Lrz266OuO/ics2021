#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <memory/paddr.h>

enum {
  /* Add more token types */
  TK_NOTYPE = 256,  // space
  DEC_NUM,          // decimal numbers
  HEX_NUM,          // hexadecimal numbers
  REG,              // registers
  TK_EQ,            // equal
  TK_NEQ,           // not equal
  TK_AND,           // logical and
  TK_OR,            // logical or
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
  {"0x[0-9a-f]+",         HEX_NUM},   // hexadecimal numbers
  {"[0-9]+",              DEC_NUM},   // decimal numbers
  
  {"\\$[0-9a-z]{1,3}",    REG},       // registers  
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

  /*
  printf("Last Tokens: %d\n", nr_token);
  for (int i=0; i<10; i++) {
    printf("%d\t%s\t%d\n", tokens[i].type, tokens[i].str, tokens[i].priority);
  }
  */

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
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case '-':
            // 需要考虑负数的问题
            // 减号在第一位或前面一个token不是数字/变量/右括号
            if ((nr_token==0) || 
                (nr_token!=0 && tokens[nr_token-1].type!=DEC_NUM 
                             && tokens[nr_token-1].type!=HEX_NUM 
                             && tokens[nr_token-1].type!=REG
                             && tokens[nr_token-1].type!=')')
                ) {
              tokens[nr_token].type = NEG_NUM;
              tokens[nr_token].priority = 2;
            }
            else {
              tokens[nr_token].type = '-';
              tokens[nr_token].priority = 4;
            }
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case '*':
            // 需要考虑指针解引用的问题
            // 乘号在第一位或前面一个token不是数字/变量/右括号
            if ((nr_token==0) || 
                (nr_token!=0 && tokens[nr_token-1].type!=DEC_NUM 
                             && tokens[nr_token-1].type!=HEX_NUM 
                             && tokens[nr_token-1].type!=REG
                             && tokens[nr_token-1].type!=')')
                ) {
              tokens[nr_token].type = DEFER;
              tokens[nr_token].priority = 2;
            }
            else {
              tokens[nr_token].type = '*';
              tokens[nr_token].priority = 3;
            }
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case '/':
            tokens[nr_token].type = '/';
            tokens[nr_token].priority = 3;
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case '%':
            tokens[nr_token].type = '%';
            tokens[nr_token].priority = 3;
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case '(':
            tokens[nr_token].type = '(';
            tokens[nr_token].priority = 1;
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case ')':
            tokens[nr_token].type = ')';
            tokens[nr_token].priority = 1;
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case DEC_NUM:
            tokens[nr_token].type = DEC_NUM;
            tokens[nr_token].priority = 0;
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case HEX_NUM:
            tokens[nr_token].type = HEX_NUM;
            tokens[nr_token].priority = 0;
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case REG:
            tokens[nr_token].type = REG;
            tokens[nr_token].priority = 0;
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case TK_EQ:
            tokens[nr_token].type = TK_EQ;
            tokens[nr_token].priority = 7;
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case TK_NEQ:
            tokens[nr_token].type = TK_NEQ;
            tokens[nr_token].priority = 7;
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case TK_AND:
            tokens[nr_token].type = TK_AND;
            tokens[nr_token].priority = 11;
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case TK_OR:
            tokens[nr_token].type = TK_OR;
            tokens[nr_token].priority = 12;
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case '!':
            tokens[nr_token].type = '!';
            tokens[nr_token].priority = 2;
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            nr_token++;
            break;
          case TK_NOTYPE:
            break;
          default: 
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
bool check_parentheses(int left, int right);
int check_dominant_operator(int left, int right, bool *success);

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  *success = true;
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
          // printf("%s\n", tokens[left].str);
          val = isa_reg_str2val(tokens[left].str, success);
          break;
        default:
          // 理论上每别的可能了，这一段就不管了
          break;
      }
      return val;
    }
    else {
      /* Bad expression */
      *success = false;
      printf("Wrong expression: unknown number or variate(%d)\n", left);
      return 0;
    }

  }

  else if (check_parentheses(left, right) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(left+1, right-1, success);
  }

  else {
    int op = check_dominant_operator(left, right, success);
    // word_t left_val = eval(left, op-1, success);
    // word_t right_val = eval(op+1, right, success);
    // word_t left_val, right_val;

    switch (tokens[op].type) {
      case '+': return eval(left, op-1, success) + eval(op+1, right, success);
      case '-': return eval(left, op-1, success) - eval(op+1, right, success);
      case '*': return eval(left, op-1, success) * eval(op+1, right, success);
      case '/':
        // 需要考虑除数为0的情况
        if (eval(op+1, right, success) == 0) {
          *success = false;
          printf("Wrong expression: divided by zero\n");
          assert(0);
        }
        else return eval(left, op-1, success) / eval(op+1, right, success);
      case '%': return eval(left, op-1, success) % eval(op+1, right, success);
      case TK_EQ: return eval(left, op-1, success) == eval(op+1, right, success);
      case TK_NEQ: return eval(left, op-1, success) != eval(op+1, right, success);
      case TK_AND: return eval(left, op-1, success) && eval(op+1, right, success);
      case TK_OR: return eval(left, op-1, success) || eval(op+1, right, success);
      case '!': return !(eval(op+1, right, success));
      case NEG_NUM: return -(eval(op+1, right, success));
      case DEFER: return *((uint32_t *)guest_to_host(eval(op+1, right, success)));
      default:
        *success = false;
        printf("Wrong expression: unknown opreator\n");
        assert(0);
        break;
    }
  }

}

bool check_parentheses(int left, int right) {
  if (!(tokens[left].type == '(' && tokens[right].type == ')')) {
    return false;
  }
  int i;
  int count_parentheses = 1;
  for(i = left+1; i <= right; i++) {
    // 要考虑'()+()'的情况
    if (count_parentheses == 0) {
      return false;
    }
    // '('->count++, ')'->count--, others->continue
    if (tokens[i].type == '(') {
      count_parentheses++;
    }
    else if (tokens[i].type == ')') {
      count_parentheses--;
    }
    else {
      continue;
    }
  }

  if (count_parentheses == 0) {
    return true;
  }
  else return false;
}

int check_dominant_operator(int left, int right, bool *success) {
  /* 注意括号里的运算符不可以是主运算符，
   * 当in_parentheses>0时，
   * 说明此时在括号内
   * 且外层包裹着in_parentheses层括号
   */
  int in_parentheses = 0;
  /* 运算符优先级是有边界的，
   * 优先级为1->括号，
   * 优先级为0->数字，
   * 如果最后highest_level = 1时，
   * 说明没有主运算符，出错了
   */
  int highest_priority = 1;
  
  int ret = left;
  int i;
  for (i = left; i <= right; i++) {
    switch (tokens[i].priority) {
      case 0:
        // 是数字或寄存器，不是运算符
        break;
      case 1:
        // 是左/右括号，括号内的运算符都不是主运算符
        if (tokens[i].type == '(') {
          in_parentheses++;
        }
        else {
          in_parentheses--;
        }
        break;
      default:
        // 是运算符，如果不在括号里，就可以判断是否是主运算符
        if (in_parentheses == 0 && tokens[i].priority >= highest_priority) {
          ret = i;
          highest_priority = tokens[i].priority;
        }
        break;
    }
  }
  if (highest_priority == 1) {
    *success = false;
    printf("error: we don't find dominant operator\n");
    assert(0);
  }
  return ret;
}
