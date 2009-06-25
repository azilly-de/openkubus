#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../openkubus.h"

int main(int argc, char **argv)
{
  uint32_t r = 0;

  if(argc != 4)
  {
    printf("USAGE: %s pw\n", argv[0]);
    return 1;
  }

  char *pw = argv[1];
  char pad[500];
  

  while(1)
  {
    fgets(pad, 400, stdin);

    pad[strlen(pad)-1] = 0;
    printf(">%s<\n", pad);
    int r = openkubus_authenticate(pad, pw, 0, 0);
    printf("> %d\n", r);
  }

  return 0;
}
