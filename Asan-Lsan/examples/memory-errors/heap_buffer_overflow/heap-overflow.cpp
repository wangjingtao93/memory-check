/*******************
 * 内存error：heap buffer overflow
 * 
 * ***************/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  int *array = new int[100];
  array[0] = 0;
  int res = array[argc + 100];  // BOOM,argc default is 1
  delete [] array;
  
  return 0;
}