#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <usb.h>

#include "../addresses.h"

//0400:c35d
#define VID 0x1781
#define PID 0x0c66

char *PROGNAME;

int strtoi(char *str);
void usage(void);

struct usb_dev_handle *easyAVR_Open ()
{
  usb_init ();
  usb_find_busses ();
  usb_find_devices ();

  struct usb_bus *bus = usb_get_busses ();
  for ( ; bus; bus = bus->next)
  {
    struct usb_device *dev;
    for (dev = bus->devices; dev; dev = dev->next)
    {
      if (dev->descriptor.idVendor == VID)
      {
        struct usb_dev_handle *usb_handle;
        //printf ("vendor: %i\n",dev->descriptor.idVendor);
        usb_handle = usb_open (dev);
        return usb_handle;
      }
    }
  }
  return NULL;
}

static void easyAVR_Close (struct usb_dev_handle *usb_handle)
{
  usb_close (usb_handle);
}

void eeprom_write(struct usb_dev_handle *usb_handle, uint8_t addr, char *s, uint8_t len)
{
  uint16_t data,i,j;

  for(i = 0; i < len; i ++)
  {
    //printf("%c\n", s[i]);
    data  = (addr+i) << 8;
    data |= (s[i]);
    
    // int usb_control_msg(usb_dev_handle *dev, int requesttype, int request, int value, int index, char *bytes, int size, int timeout);
    // requesttype: 01000000: vendor specific request from host to device
    // value: high byte: address, lower byte: data
    j = usb_control_msg(usb_handle, 64, USB_WRITE_EEPROM, data, 0, NULL, 0, 100);

    if(j != 0)
    {
      fprintf(stderr, "USB error: %d\n", j);
      exit(10);
    }
  }
}

int main (int argc, char **argv)
{
  struct usb_dev_handle *usb_handle = easyAVR_Open ();
  int lflag = 0, hflag = 0, dflag = 0;
  char offset[] = "\0\0";
  char *password      = NULL;
  char *offset_arg    = NULL;
  char *owner_arg     = NULL;
  char *company_arg   = NULL;
  char *timestamp_arg = NULL;
  char timestamp[4];
  char *id_arg        = NULL;
  int c;

  PROGNAME = argv[0];

  opterr = 0;
     
  while ((c = getopt (argc, argv, "dhlp:o:O:C:T:I:")) != -1)
  {
    switch (c)
    {
      case 'd': dflag = 1; break;
      case 'l': lflag = 1; break;
      case 'h': hflag = 1; break;
      case 'p': password   = optarg; break;
      case 'o': offset_arg = optarg; break;

      case 'O': owner_arg     = optarg; break;
      case 'C': company_arg   = optarg; break;
      case 'T': timestamp_arg = optarg; break;
      case 'I': id_arg        = optarg; break;
    }
  }

  if(hflag)
  {
    usage();
    exit(0);
  }

  if(password == NULL)
  {
    fprintf(stderr, "No password omitted.\n\n");
    usage();
    exit(1);
  }

  if(strlen(password) != LEN_SEED)
  {
    fprintf(stderr, "Password must have %d characters. (password omitted: %d)\n\n", LEN_SEED, strlen(password));
    usage();
    exit(1);
  }

  if(offset_arg != NULL)
  {
    int o = strtoi(offset_arg);

    if(o < 0)
    {
      fprintf(stderr, "offset must be a positive number.\n\n");
      usage();
      exit(1);
    }
    else if(o < 0 || o > 65536)
    {
      fprintf(stderr, "offset in wrong range.\n\n");
      usage();
      exit(1);
    }

    offset[0] = o % 256;
    offset[1] = o / 256;
  }
 
  if(dflag)
    usb_set_debug (2);

  if(usb_handle == NULL)
  {
    fprintf(stderr, "USB device not found.\n");
    exit(4);
  }
  
  if(owner_arg != NULL)
  {
    int len = strlen(owner_arg);

    if(len > LEN_OWNER-1)
    {
      fprintf(stderr, "argument for owner mustn't be longer than %d characters.\n\n", LEN_OWNER-1);
      usage();
      exit(1);
    }
  }

  if(company_arg != NULL)
  {
    int len = strlen(company_arg);

    if(len > LEN_COMPANY-1)
    {
      fprintf(stderr, "argument for company mustn't be longer than %d characters.\n\n", LEN_COMPANY-1);
      usage();
      exit(1);
    }
  }

  {
    int t = time(NULL);

    if(timestamp_arg != NULL)
    {
      t = strtoi(timestamp_arg);

      if(t <= 0)
      {
        fprintf(stderr, "argument for timestamp must be a positive number.\n\n");
        usage();
        exit(1);
      }
      else if(t > 2147483647)
      {
        fprintf(stderr, "argument for timestamp mustn't be larger than 2147483647.\n\n");
        usage();
        exit(1);
      }
    }

    timestamp[0] =  t           % 256;
    timestamp[1] = (t/     256) % 256;
    timestamp[2] = (t/   65536) % 256;
    timestamp[3] = (t/16777216) % 256;
  }

  // owner
  if(owner_arg != NULL)
    eeprom_write(usb_handle, ADDR_OWNER, owner_arg, strlen(owner_arg)+1);

  // company
  if(company_arg != NULL)
    eeprom_write(usb_handle, ADDR_COMPANY, company_arg, strlen(company_arg)+1);

  // timestamp
  eeprom_write(usb_handle, ADDR_TIMESTAMP, timestamp, LEN_TIMESTAMP);

  // key
  eeprom_write(usb_handle, ADDR_SEED, password, LEN_SEED);

  // counter
  eeprom_write(usb_handle, ADDR_COUNTER, offset, 2);

  // lock
  if(lflag)
    eeprom_write(usb_handle, ADDR_LOCK, "l", 1);

  easyAVR_Close (usb_handle);

  return 0;
}

void usage(void)
{
  fprintf(stderr, "Usage: %s [-l] password\n\n", PROGNAME);

  fprintf(stderr, "%s: writes password into EEPROM of OpenKubus-Stick.\n", PROGNAME);
  fprintf(stderr, "USB-vendor requests will be used to send the password to the stick.\n");
  fprintf(stderr, "Attention: run this script as root!\n\n");

  fprintf(stderr, "required arguments:\n");
  fprintf(stderr, "  -p password: password (must have %d characters)\n\n", LEN_SEED);

  fprintf(stderr, "useful arguments:\n");
  fprintf(stderr, "  -o offset: offset value\n");
  fprintf(stderr, "  -O owner: owner of the stick (max. %d characters)\n", LEN_OWNER-1);
  fprintf(stderr, "  -C company: company-name (max. %d characters)\n", LEN_COMPANY-1);
  fprintf(stderr, "  -T timestamp: timestamp (4 bytes)\n");
  fprintf(stderr, "  -I ID: arbitrary id (4 bytes)\n\n");

  fprintf(stderr, "other arguments:\n");
  fprintf(stderr, "  -d: show debugging information\n");
  fprintf(stderr, "  -l: lock stick; stick will then ignore USB-vendor-requests\n");
  fprintf(stderr, "  -h: show this help\n");
}

int strtoi(char *str)
{
  int o = 0;
  char *endptr;
  o = strtol(str, &endptr, 10);

  // see: $ man strtol
  if((errno == ERANGE && (o == LONG_MAX || o == LONG_MIN)) || (errno != 0 && o == 0) || endptr == str)
    return -1;
  else
    return o;
}
