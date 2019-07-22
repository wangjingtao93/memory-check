/*******************
 * 内存error：memory_leaks.c
 * ***************/
#include <stdlib.h>
#include <stdio.h>

void *p;

int main() {
  p = malloc(7);
  p = 0; // The memory is leaked here.
  printf("%s\n","end");
  return 0;
}