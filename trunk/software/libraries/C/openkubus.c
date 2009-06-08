#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <gcrypt.h>
#include <string.h>

#include "base64.h"

#define BLOCKSIZE 16
#define KEYSIZE   32
#define DATASIZE  14
#define MAXPAD    33


// initializes gcrypt
void crypt_initialize(void)
{
  // from: http://www.gnupg.org/documentation/manuals/gcrypt/Initializing-the-library.html#Initializing-the-library
  /* Version check should be the very first call because it
  makes sure that important subsystems are intialized. */
  if (!gcry_check_version (GCRYPT_VERSION))
  {
    fputs ("libgcrypt version mismatch\n", stderr);
    exit (2);
  }
     
  /* We don't want to see any warnings, e.g. because we have not yet
  parsed program options which might be used to suppress such
  warnings. */
  gcry_control(GCRYCTL_SUSPEND_SECMEM_WARN);
     
  /* ... If required, other initialization goes here.  Note that the
  process might still be running with increased privileges and that
  the secure memory has not been intialized.  */
  
  /* Allocate a pool of 16k secure memory.  This make the secure memory
  available and also drops privileges where needed.  */
  gcry_control(GCRYCTL_INIT_SECMEM, 16384, 0);
     
  /* It is now okay to let Libgcrypt complain when there was/is
  a problem with the secure memory. */
  gcry_control(GCRYCTL_RESUME_SECMEM_WARN);
     
  /* ... If required, other initialization goes here.  */
     
  /* Tell Libgcrypt that initialization has completed. */
  gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
}

// uses gcrypt to encrypt a crypted block (SHA256) with given key
char *encrypt(char *in, char *out, const char *key)
{
  gcry_cipher_hd_t hd;

  if (!gcry_control (GCRYCTL_INITIALIZATION_FINISHED_P))
    crypt_initialize();

  gcry_cipher_open(&hd, GCRY_CIPHER_RIJNDAEL256, GCRY_CIPHER_MODE_ECB, 0);
  gcry_cipher_setkey(hd, key, KEYSIZE);
  gcry_cipher_encrypt(hd, out, 32, in, 16);
  gcry_cipher_close(hd);

  return out;
}

// uses gcrypt to decrypt a crypted block (SHA256) with given key
char *decrypt(char *crypted, const char *key)
{
  gcry_cipher_hd_t hd;

  if (!gcry_control (GCRYCTL_INITIALIZATION_FINISHED_P))
    crypt_initialize();

  gcry_cipher_open(&hd, GCRY_CIPHER_RIJNDAEL256, GCRY_CIPHER_MODE_ECB, 0);
  gcry_cipher_setkey(hd, key, KEYSIZE);
  gcry_cipher_decrypt(hd, crypted, BLOCKSIZE, NULL, 0);
  gcry_cipher_close(hd);

  return crypted;
}

// converts string to real base64
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

void openkubus_gen_pad(const char *pw, uint16_t offset, uint16_t num, char *pad)
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

  encrypt(data, crypted, aes);

  pad[0] = 'z';
  i = raw_to_base64(crypted, 16, &pad[1], 60);
  printf("noch\n");
  pad[i+1] = 0;
  printf("noch\n");
  base642pad(pad);
  printf("noch\n");
}

int32_t openkubus_authenticate(const char *pad, const char *pw, uint16_t offset, uint16_t num)
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

  plain = decrypt(str, key);

  k  =  (uint8_t)  plain[0];
  k |= ((uint16_t) plain[1] << 8);

  if(strncmp(plain+2, data, 14) != 0)
    return -12;

  if((k+1-offset) <= num)
    return -124;

  return k;
}
