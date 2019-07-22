/***********************************************************************
 * 内存error:all kinds of memory errors
 * memory leak,stack buffer overflow,heap buffer overflow, use after free
 * usr after retrun
 * 另外还有 init order bugs, use after scope(下面的例子不包括这两种)
 * *********************************************************************/

#include <stdlib.h>
#include <stdio.h>

int global_array[100] = {-1};//global buffer overflow

int main(int argc, char **argv)
{
    //memory leak
    void *p;
    p = malloc(7);
    p = 0; // The memory is leaked here.
    printf("%s\n", "memory leak dector");

    //stack buffer overflow
    int stack_array[100];
    stack_array[1] = 0;
    printf("%d\n", stack_array[argc + 100]); //BOOM

    //heap buffer overflow
    int *array = new int[100];
    array[0] = 0;
    int res = array[argc + 100]; // BOOM,argc default is 1
    delete[] array;

    //use after free
    char *x = (char *)malloc(10 * sizeof(char *));
    free(x);
    printf("%d\n", x[5]);


    //global buffer overflow
    printf("%d\n",global_array[argc + 100]);  // BOOM

    return 0;
}

/***************usr after retrun***************
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
***********************************************/

