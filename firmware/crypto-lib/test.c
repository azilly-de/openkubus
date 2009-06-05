#include <stdio.h>

void main(void)
{
  int i = 0;
  char str[] = "HaLlO WeLt!";

  while(str[i])
  {
    if(str[i] < 'Z' && str[i] > 'A')
      putchar(str[i] + 32);
    else
      putchar(str[i]);

    i++;
  }
}
