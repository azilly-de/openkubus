#include "common.h"

void char2usb(char *hex, char *modifier)
{
  switch(*hex)
  {
    case '\n': *hex = 0x28; break;
    case ' ': *hex = 0x2C; break;
    case '-': *hex = 0x2d; break;
    case '0': *hex = 0x27; break;
    case '/': *modifier = 1 << 5; *hex = 0x1E; break;
    case '=': *hex = 0x37; break; // = => 3
    case '+': *hex = 0x38; break; // = => 3

    default:
      if(*hex <= 'Z' && *hex >= 'A')
      {
        *modifier = 1 << 5; // right shift
        *hex = *hex - 65 + 0x04;
      }
      else if(*hex <= 'z' && *hex >= 'a')
        *hex = *hex - 97 + 0x04;
      else if(*hex >= '1' && *hex <= '9')
        *hex = *hex - 49 + 0x1E; // 49: '1'
  }
}
