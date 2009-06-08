#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <usb.h>

#include "../addresses.h"

//0400:c35d
#define VID 0x0400
#define PID 0xc35d

char *PROGNAME;

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
  char *password = NULL;
  int c;

  PROGNAME = argv[0];

  opterr = 0;
     
  while ((c = getopt (argc, argv, "dhlp:")) != -1)
  {
    switch (c)
    {
      case 'd': dflag = 1; break;
      case 'l': lflag = 1; break;
      case 'h': hflag = 1; break;
      case 'p': password = optarg; break;
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

  if(strlen(password) != 46)
  {
    fprintf(stderr, "Password must have 46 characters. (password omitted: %d)\n\n", strlen(password));
    usage();
    exit(1);
  }
  
  if(dflag)
    usb_set_debug (2);

  if(usb_handle == NULL)
  {
    fprintf(stderr, "USB device not found.\n");
    exit(4);
  }
  
  // key
  eeprom_write(usb_handle, ADDR_KEY,  &password[0], 32);
  // data
  eeprom_write(usb_handle, ADDR_DATA, &password[32], 14);
  // counter
  eeprom_write(usb_handle, ADDR_COUNTER, "\0\0", 2);
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

  fprintf(stderr, "-p password: password (must have 48 characters)\n");
  fprintf(stderr, "-d: show debugging information\n");
  fprintf(stderr, "-l: lock stick; stick will then ignore USB-vendor-requests\n");
  fprintf(stderr, "-h: show this help\n");
}
