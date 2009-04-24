#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "openkubus.h"

int main(int argc, char **argv)
{
  uint32_t r = 0;

  if(argc != 5)
  {
    printf("USAGE: %s pad pw offset num\n", argv[0]);
    return 1;
  }
  
  printf("pad: >%s<\npw: >%s<\n", argv[1], argv[2]);
  r = openkubus_authenticate(argv[1], argv[2], 0, 0);//atoi(argv[3]), atoi(argv[4]));
  if(r >= 0)
    printf("Authentifizierung erfolgreich (%d).\n", r);
  else
    printf("Authentifizierung nicht erfolgreich (%d).\n", r);

  return 0;
}
