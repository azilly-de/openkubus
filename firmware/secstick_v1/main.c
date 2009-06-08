#define F_CPU 16000000UL  // 16 MHz

#include <avr/interrupt.h>
#include <avr/io.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include "main.h"
#include "../addresses.h"
#include "base64.h"
#include "eeprom.h"
#include "usbn2mc.h"

#include "../crypto-lib/aes.h"
#include "../crypto-lib/aes256_dec.h"
#include "../crypto-lib/aes256_enc.h"
#include "../crypto-lib/aes_dec.h"
#include "../crypto-lib/aes_enc.h"
#include "../crypto-lib/aes_invsbox.h"
#include "../crypto-lib/aes_keyschedule.h"
#include "../crypto-lib/aes_sbox.h"
#include "../crypto-lib/gf256mul.h"


struct packet_t
{
  unsigned long id; // 4 byte 
  unsigned long otp_call; //4 byte
  unsigned long timestamp; //4 byte
  unsigned char key[48];
  unsigned short reserce; //2 Byte
  unsigned short crc16; //2 Byte
} packet;

volatile int tx1togl=0;     // inital value of togl bit
/* report descriptor keyboard */

char ReportDescriptorKeyboard[] = 
{ 
  5, 1,      // Usage_Page (Generic Desktop) 
  9, 6,      // Usage (Keyboard) 
  0xA1, 1,   // Collection (Application) 
  5, 7,      // Usage page (Key Codes) 
  0x19, 224, // Usage_Minimum (224) 
  0x29, 231, // Usage_Maximum (231) 
  0x15, 0,   // Logical_Minimum (0) 
  0x25, 1,   // Logical_Maximum (1) 
  0x75, 1,   // Report_Size (1) 
  0x95, 8,   // Report_Count (8) 
  0x81, 2,   // Input (Data,Var,Abs) = Modifier Byte 

  0x95, 0x01,  //   REPORT_COUNT (1)
  0x75, 0x08,  //   REPORT_SIZE (8)
  0x25, 0x65,  //   LOGICAL_MAXIMUM (101)
  0x19, 0x00,  //   USAGE_MINIMUM (Reserved (no event indicated))
  0x29, 0x65,  //   USAGE_MAXIMUM (Keyboard Application)
  0x81, 0x00,  //   INPUT (Data,Ary,Abs)
  0xc0         // END_COLLECTION
/*
  0x81, 1, // Input (Constant) = Reserved Byte 
  0x19, 0, // Usage_Minimum (0) 
  0x29, 101, // Usage_Maximum (101) 
  0x15, 0, // Logical_Minimum (0) 
  0x25, 101, // Logical_Maximum (101) 
  0x75, 8, // Report_Size (8) 
  0x95, 6, // Report_Count (6) 
  0x81, 0, // Input (Data,Array) = Keycode Bytes(6) 
  5, 8, // Usage Page (LEDs) 
  0x19, 1, // Usage_Minimum (1) 
  0x29, 5, // Usage_Maximum (5) 
  0x15, 0, // Logical_Minimum (0) 
  0x25, 1, // Logical_Maximum (1) 
  0x75, 1, // Report_Size (1) 
  0x95, 5, // Report_Count (5) 
  0x91, 2, // Output (Data,Var,Abs) = LEDs (5 bits) 
  0x95, 3, // Report_Count (3) 
  0x91, 1, // Output (Constant) = Pad (3 bits) 
  0xC0 // End_Collection 
*/
};


/* Device Descriptor */

const unsigned char easyavrDevice[] =
{ 
  0x12,       // 18 length of device descriptor
  0x01,       // descriptor type = device descriptor
  0x10,0x01,  // version of usb spec. ( e.g. 1.1)
  0x00,       // device class
  0x00,       // device subclass
  0x00,       // protocol code
  0x08,       // deep of ep0 fifo in byte (e.g. 8)
  0x00,0x04,  // vendor id
  0x5D,0xC3,  // product id
  0x03,0x01,  // revision id (e.g 1.02)
  0x00,       // index of manuf. string
  0x00,       // index of product string
  0x00,       // index of ser. number
  0x01        // number of configs
};

/* Configuration descriptor */

const unsigned char easyavrConf[] =
{ 
  0x09,       // 9 length of this descriptor
  0x02,       // descriptor type = configuration descriptor
  0x22,0x00,  // total length with first interface ...
  0x01,       // number of interfaces
  0x01,       // number if this config. ( arg for setconfig)
  0x00,       // string index for config
  0xA0,       // attrib for this configuration ( bus powerded, remote wakup support)
  0x1A,       // power for this configuration in mA (e.g. 50mA)
  //InterfaceDescriptor
  0x09,       // 9 length of this descriptor
  0x04,       // descriptor type = interface descriptor
  0x00,       // interface number
  0x00,       // alternate setting for this interface
  0x01,       // number endpoints without 0
  0x03,       // class code
  0x01,       // sub-class code
  0x01,       // protocoll code
  0x00,       // string index for interface
  // HID Descriptor Keyboard
  0x09,       // length ot this descriptor
  0x21,       // HID Descriptortype
  0x10,0x01,  // hid class spec
  0x09,       //country
  0x01,       // number of hid descriptors to follow
  0x22,       // descriptor type
  0x23,       // total length of report descriptor 59
  0x00,
  //EP1 Descriptor
  0x07,       // length of ep descriptor
  0x05,       // descriptor type= endpoint
  0x81,       // endpoint address (e.g. in ep1)
  0x03,       // transfer art ( bulk )
  0x08,0x00,  // fifo size
  0x0A,       // polling intervall in ms
};


SIGNAL(SIG_INTERRUPT0)
{
  USBNInterrupt();
}


/*************** usb class HID requests  **************/

// reponse for requests on interface
void USBNInterfaceRequests(DeviceRequest *req,EPInfo* ep)
{
  // 81 06 22 get report descriptor
  switch(req->bRequest)
  {
      case GET_DESCRIPTOR:
        ep->Index=0;
        ep->DataPid=1;
        ep->Size=35; //statt 59
        ep->Buf=ReportDescriptorKeyboard;
      break;

      default:
        //UARTWrite("unkown interface request");
      ;
  }
}



// vendor requests
void USBNDecodeVendorRequest(DeviceRequest *req)
{
  uint8_t rtype = (req->bmRequestType >> 5) & 3; // 3: 11

  // check if request is vender specific and it is our request
  if(rtype != 2) return;
    
  if(req->bRequest == USB_WRITE_EEPROM)
  {
    uint8_t lock;
    eeprom_read(ADDR_LOCK, &lock, 1);

    //if(lock != 0 && lock != 255) return;

    uint8_t addr  = req->wValue >> 8;
    uint8_t value = (uint8_t) req->wValue;

    eeprom_write(addr, &value, 1);
  }
  else if(req->bRequest == USB_CHANGE_KEY)
  {
    uint8_t entry = (uint8_t) req->wValue;
  }
}


// class requests
void USBNDecodeClassRequest(DeviceRequest *req)
{
}

inline uint8_t debounce(volatile uint8_t *port, uint8_t pin)
{
  if( ! (*port & (1 << pin)) )
  {
    /* Pin wurde auf Masse gezogen, 100ms warten   */
    _delay_ms(50);  // max. 262.1 ms / F_CPU in MHz
    _delay_ms(50); 
    if ( *port & (1 << pin) )
    {
      /* Anwender Zeit zum Loslassen des Tasters geben */
      _delay_ms(50);
      _delay_ms(50); 
      return 1;
    }
  }
  return 0;
}


void write_pad(void)
{
  uint16_t n;
  char out[64];
  char data[16];
  char key[32];

  aes256_ctx_t ctx;

  // read_into_ram:
  eeprom_read(ADDR_KEY,  &key,  32);
  eeprom_read(ADDR_DATA, &data[2], 14);

  // increase counter
  eeprom_read(ADDR_COUNTER, data, 2);
  n = data[0] + (data[1] << 8);
  n++;
  data[0] = (uint8_t) n;
  data[1] = (uint8_t) (n >> 8);
  eeprom_write(ADDR_COUNTER, data, 2);

  memset(&ctx, 0, sizeof(aes256_ctx_t));
  memset(out, 0, 64);

  aes256_init(key, &ctx);
  aes256_enc(data, &ctx);

  usbWriteChar("z");
  raw_to_base64(data, 16, out, 60);
  usbWriteChar(out);
}

/* function for sending strings over usb hid device 
 * please use max size of 64 in this version
 */
void usbWriteChar(char *str)
{
  while(*str)
  {
    char modifier = 0;
    char hex = *str++;
   
    switch(hex)
    {
      case ' ': hex = 0x2C; break;
      case '-': hex = 0x2d; break;
      case '0': hex = 0x27; break;
      case '/': modifier = 1 << 5; hex = 0x1E; break;
      case '=': hex = 0x37; break; // = => 3
      case '+': hex = 0x38; break; // = => 3
   
      default:
        if(hex <= 'Z' && hex >= 'A')
        {
          modifier = 1 << 5; // right shift
          hex = hex - 65 + 0x04;
        }
        else if(hex <= 'z' && hex >= 'a')
          hex = hex - 97 + 0x04;
        else if(hex >= '1' && hex <= '9')
          hex = hex - 49 + 0x1E; // 49: '1'
    }

    usbHIDWrite(modifier, hex);
    _delay_ms(11);

    usbHIDWrite(modifier,0);
    _delay_ms(11);
  }

    usbHIDWrite(0,0);
    _delay_ms(11);
}

void usbHIDWrite(char modifier, char hex)
{

  USBNWrite(TXC1,FLUSH);  //enable the TX (DATA1)

  USBNWrite(TXD1,modifier);
  USBNWrite(TXD1,hex);

  /* control togl bit of EP1 */
  if(tx1togl)
  {
    USBNWrite(TXC1,TX_TOGL+TX_EN+TX_LAST);  //enable the TX (DATA1)
    tx1togl=0;
  }
  else
  {
    USBNWrite(TXC1,TX_EN+TX_LAST);  //enable the TX (DATA1)
    tx1togl=1;
  }
}



/*************** main function  **************/
int main(void)
{
  sei();      // activate global interrupts

  DDRA   =  (1 << DDA4);
  DDRB  &= ~(1 << DDB2); //input
  PORTB |=  (1<<PB2);  //pullup on

  PORTA &= ~(1<<PA4); //off

  // setup usbstack with your descriptors
  USBNInit(easyavrDevice,easyavrConf);

  USBNInitMC();   // start usb controller
  USBNStart();    // start device stack
  usbHIDWrite(0, 0x00);
  _delay_ms(20);
  usbHIDWrite(0, 0x00);

  uint16_t i = 0;
  while(1)
  {
    i++;

    if(i == 10000)
      PORTA |= (1 << PA4);  //on
    else if(i == 20000)
    {
      PORTA &= ~(1 << PA4); //off
      i = 0;
    }

    _delay_us(100);

    if(debounce(&PINB, PB2)) 
    {
      PORTA |= (1 << PA4);  //on
      write_pad();
      PORTA &= ~(1 << PA4); //off
    }
  }
}
