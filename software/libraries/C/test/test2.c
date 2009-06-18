#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../openkubus.h"

#define PASSWD_FILE "test.txt"
#define BUFFER 500

char line[500];
char pad[100];

int main(void)
{
  FILE *file = NULL;
  const char *delim = " ";
  char *user   = NULL;
  char *pw     = NULL;
  char *offset = NULL;
  char *num    = NULL;

  if((file = fopen(PASSWD_FILE, "r")) == NULL)
  {
    fprintf(stderr, "Can't open %s for reading.\n", PASSWD_FILE);
    exit(1);
  }

  if(fgets(line, BUFFER, file) == NULL)
  {
    fprintf(stderr, "error reading %s (fgets returned NULL).\n", PASSWD_FILE);
    exit(1);
  }

  user   = strtok(line, delim);
  pw     = strtok(NULL, delim);
  offset = strtok(NULL, delim);
  num    = strtok(NULL, delim);

  if(user == NULL || pw == NULL || offset == NULL || num == NULL)
  {
    fprintf(stderr, "error parsing %s\n.", PASSWD_FILE);
    exit(1);
  }

  openkubus_gen_pad(pw, atoi(offset), atoi(num), pad);

  if(fclose(file) != 0)
  {
    fprintf(stderr, "Can't close file %s.\n", PASSWD_FILE);
    exit(1);
  }

  if((file = fopen(PASSWD_FILE, "w")) == NULL)
  {
    fprintf(stderr, "Can't open %s for writing.\n", PASSWD_FILE);
    exit(1);
  }

  fprintf(file, "%s %s %s %d\n", user, pw, offset, atoi(num)+1);

  if(fclose(file) != 0)
  {
    fprintf(stderr, "Can't close file %s.\n", PASSWD_FILE);
    exit(1);
  }

  printf("%s %s\n", user, pad);

  exit(0);
}
