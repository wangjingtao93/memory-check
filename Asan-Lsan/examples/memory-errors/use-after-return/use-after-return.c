// RUN: clang -O -g -fsanitize=address %t && ./a.out
// By default, AddressSanitizer does not try to detect 默认是关闭的
// stack-use-after-return bugs.
// It may still find such bugs occasionally 也许仍然偶尔的寻找
// and report them as a hard-to-explain stack-buffer-overflow.将它们报告为难以解释的堆栈缓冲区溢出。

// You need to run the test with ASAN_OPTIONS=detect_stack_use_after_return=1，打开ASAN_OPTIONS=detect_stack_use_after_return=1即可

#include <stdlib.h>
#include <stdio.h>

int *ptr;
__attribute__((noinline))

void FunctionThatEscapesLocalObject(){
    int local[1000];
    ptr = &local[100];
}

int main(int argc, char** argv){
    FunctionThatEscapesLocalObject();//函数调用完，local内存已经释放
    return ptr[argc];
}