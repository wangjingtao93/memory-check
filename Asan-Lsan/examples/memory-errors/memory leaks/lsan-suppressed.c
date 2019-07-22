/*******************
 * 内存error：suppressions
 * 通过传入抑制文件suppr.txt,忽略FooBar的泄漏
 * leak:FooBar
 * ***************/
#include <stdlib.h>

void FooBar(){
    malloc(7);
}

void Baz(){
    malloc(5);
}

int main(){
    FooBar();
    Baz();

    return 0;
}