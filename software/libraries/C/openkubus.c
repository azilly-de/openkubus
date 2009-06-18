#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"
#include "rijndael.h"

#define BLOCKSIZE 16
#define KEYSIZE   32
#define DATASIZE  14
#define MAXPAD    33
#define KEYBITS   256


// uses gcrypt to encrypt 16-byte to a crypted block (SHA256) with given key
unsigned char *encrypt(unsigned char *ciphertext, unsigned char *plaintext, const unsigned char *key)
{
  unsigned long rk[RKLENGTH(KEYBITS)];

  int nrounds = rijndaelSetupEncrypt(rk, key, KEYBITS);

  rijndaelEncrypt(rk, nrounds, ciphertext, plaintext);

  return ciphertext;
}

// uses gcrypt to decrypt a crypted block (SHA256) with given key
unsigned char *decrypt(unsigned char *crypted, const unsigned char *key)
{
  unsigned long rk[RKLENGTH(KEYBITS)];
  unsigned char plain[BLOCKSIZE];

  int nrounds = rijndaelSetupDecrypt(rk, key, KEYBITS);

  rijndaelDecrypt(rk, nrounds, crypted, plain);

  int i;
  for(i = 0; i < 16; i++)
    crypted[i] = plain[i];

  crypted[i] = '\0';

  return crypted;
}

// converts real base64 to pad
void base642pad(char *str)
{
  int i;

  for(i = 0; i < strlen(str); i++)
  {
    switch(str[i])
    {
      case '/': str[i] = '!'; break;
      case '=': str[i] = '.'; break;
      case '+': str[i] = '-'; break;
    }
  }
}

// converts string to real base64
void pad2base64(char *str)
{
  char a = str[0];
  int i;

  for(i = 0; i < strlen(str)-1; i++)
  {
    str[i] = str[i+1];

    switch(str[i])
    {
      case '!': str[i] = '/'; break;
      case '.': str[i] = '='; break;
      case '-': str[i] = '+'; break;
    }

    if(a == 'y' || a == 'Y')
    {
      switch(str[i])
      {
        case 'y': str[i] = 'z'; break;
        case 'Y': str[i] = 'Z'; break;
        case 'z': str[i] = 'y'; break;
        case 'Z': str[i] = 'Y'; break;
      }
    }
  }
  str[i] = '\0';
}

void openkubus_gen_pad(const char *pw, int offset, int num, char *pad)
{
  char data[17];
  char aes[33];
  char crypted[33];
  uint8_t i;

  num += offset;

  data[0] = (uint8_t) num;
  data[1] = (uint8_t) (num >> 8);

  for(i = 0; i < 14; i++)
    data[2+i] = pw[32+i];
  data[16] = 0;

  for(i = 0; i < 32; i++)
    aes[i] = pw[i];
  aes[32] = 0;

  encrypt((unsigned char *)data, (unsigned char *)crypted, (unsigned char *)aes);
  crypted[33] = 0;

  pad[0] = 'z';
  i = raw_to_base64(crypted, 16, &pad[1], 60);
  pad[i+1] = 0;
  base642pad(pad);
}

int openkubus_authenticate(const char *pad, const char *pw, uint16_t offset, uint16_t num)
{
  char *plain = NULL;
  int len;
  uint16_t k;
  char key[KEYSIZE+1];
  char data[DATASIZE+1];
  char str[MAXPAD+1];

  if((len = strlen(pad)) < BLOCKSIZE)
    return -1;

  if(strlen(pw) != 32+14)
    return -2;

  if(strlen(pad) > MAXPAD)
    return -3;

  for(k = 0; k < KEYSIZE; k++)
    key[k] = pw[k];
  key[k] = '\0';

  for(k = 0; k < DATASIZE; k++)
    data[k] = pw[k+KEYSIZE];
  data[k] = '\0';

  memcpy(str, pad, strlen(pad)+1);

  pad2base64(str);

  if(base64_to_raw_inplace(str, len) != BLOCKSIZE)
    return -5;

  plain = (char *)decrypt((unsigned char *)str, (unsigned char *)key);

  k  =  (uint8_t)  plain[0];
  k |= ((uint16_t) plain[1] << 8);

  if(strncmp(plain+2, data, 14) != 0)
    return -12;

  if((k+1-offset) <= num)
    return -124;

  return k;
}
