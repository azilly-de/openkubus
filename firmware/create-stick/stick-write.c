#include <stdio.h>
#include <stdlib.h>
#include <usb.h>
#include <string.h> 

#include "../addresses.h"

//0400:c35d
#define VID 0x0400
#define PID 0xc35d

struct usb_dev_handle *easyAVR_Open ()
{
//  usb_set_debug (2);
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
    //printf("%u %c\n", addr+i, s[i]);
    data  = (addr+i) << 8;
    data |= (s[i]);
    
    // int usb_control_msg(usb_dev_handle *dev, int requesttype, int request, int value, int index, char *bytes, int size, int timeout);
    // requesttype: 01000000: vendor specific request from host to device
    // value: high byte: address, lower byte: data
    j = usb_control_msg(usb_handle, 64, 30, data, 0, NULL, 0, 100);
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
  uint8_t lock = 0;

  if(argc < 3)
  {
    fprintf(stderr, "Usage: %s key data [lock]\n", argv[0]);
    exit(1);
  }
  else if(argc > 3)
    lock = 1;

  if(strlen(argv[1]) < 32)
  {
    fprintf (stderr, "\nKey too short.\n");
    exit(2);
  }
  else if(strlen(argv[1]) > 32)
    fprintf(stderr, "Warning: Key too long.\n");

  if(strlen(argv[2]) < 14)
  {
    fprintf(stderr, "\nData too short.\n");
    exit(3);
  }
  else if(strlen(argv[2]) > 14)
    fprintf(stderr, "Warning: Data too long.\n");

  if(usb_handle == NULL)
  {
    fprintf(stderr, "USB device not found.\n");
    exit(4);
  }
  
  // key
  eeprom_write(usb_handle, ADDR_KEY,  argv[1], 32);
  // data
  eeprom_write(usb_handle, ADDR_DATA, argv[2], 14);
  // counter
  eeprom_write(usb_handle, ADDR_COUNTER, "\0\0", 2);
  // lock
  if(lock)
    eeprom_write(usb_handle, ADDR_LOCK, "l", 1);

  easyAVR_Close (usb_handle);

  return 0;
}
