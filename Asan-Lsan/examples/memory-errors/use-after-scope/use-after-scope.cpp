// RUN: clang++ -O -g -fsanitize=address -fsanitize-address-use-after-scope \
//    use-after-scope.cpp -o /tmp/use-after-scope
// RUN: /tmp/use-after-scope

// Check can be disabled in run-time:
// RUN: ASAN_OPTIONS=detect_stack_use_after_scope=0 /tmp/use-after-scope


//发现编译时缺少-fsanitize-address-use-after-scope

#include <stdlib.h>

//定义为volatile的变量是说这变量可能会被意想不到地改变，
//volatile是“易变的”、“不稳定”的意思。volatile是C的一个较为少用的关键字，
//它用来解决变量在“共享”环境下容易出现读取错误的问题。
volatile int *p = 0;

int main() {
  {
    int x = 0;
    p = &x;
  }
  *p = 5;
  return 0;
}