#include <stdio.h>
#include "openkubus.h"

int main(void)
{
  char pad[60];
  char *pw = "Bie9aiTeengieCh7Zeigh2shohngi0peib1EiChofeeShang";


  openkubus_gen_pad(pw, 0, 4, pad);
  printf(">%s<\n", pad);

  printf("\nshould:\n>z7s!8wb1nidB05ahRE2Oqag..<\n");


  return 0;
}
