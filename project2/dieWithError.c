#include <stdio.h>
#include <stdlib.h>

void dieWithError(char *error) {
  printf("Error: %s, exiting\n",error);
  exit(2);
}
