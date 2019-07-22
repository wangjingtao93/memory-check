#include <stdlib.h>
#include "fun1.h"
#include "fun2.h"

int main(){
    function1();
    function2();

    char* ptmp = (char*)malloc(10*sizeof(char*));
    
    free(ptmp);

    return ptmp[5];

}