#include "openkubus.h"

#include <util/delay.h>
#include <util/atomic.h>

#define PB4 4
#define PE6 6

#include "base64.h"
#include "../addresses.h"
#include "../common.h"
#include "eeprom.h"

#include "../crypto-lib/aes.h"
#include "../crypto-lib/aes256_dec.h"
#include "../crypto-lib/aes256_enc.h"
#include "../crypto-lib/aes_dec.h"
#include "../crypto-lib/aes_enc.h"
#include "../crypto-lib/aes_invsbox.h"
#include "../crypto-lib/aes_keyschedule.h"
#include "../crypto-lib/aes_sbox.h"
#include "../crypto-lib/gf256mul.h"

void sleep(uint16_t i)
{
  while(i--)
    _delay_ms(1);
}

volatile uint8_t send = 0;
volatile char *tosend;

void SendString(char *str);

/* Global Variables */
/** Indicates what report mode the host has requested, true for normal HID reporting mode, false for special boot
 *  protocol reporting mode.
 */
bool UsingReportProtocol = true;

/** Current Idle period. This is set by the host via a Set Idle HID class request to silence the device's reports
 *  for either the entire idle duration, or until the report status changes (e.g. the user moves the mouse).
 */
uint8_t IdleCount = 0;

/** Current Idle period remaining. When the IdleCount value is set, this tracks the remaining number of idle
 *  milliseconds. This is seperate to the IdleCount timer and is incremented and compared as the host may request 
 *  the current idle period via a Get Idle HID class request, thus its value must be preserved.
 */
uint16_t IdleMSRemaining = 0;


/** Main program entry point. This routine configures the hardware required by the application, then
 *  starts the scheduler to run the USB management task.
 */
int main(void)
{
#if defined(__AVR_ATmega16U4__ )

  DDRB = (1 << PB4);
  PORTB = 0;
  DDRE = 0;
  PORTE = 0;

  #define PORTLED PORTB
  #define LED PB4

  #define PORTBUTTON PINE
  #define BUTTON PE6
#elif defined(__AVR_AT90USB1287__)
  DDRD = (1 << PD6);
  PORTD = 0;

  DDRE = 0;
  PORTE = 0;

  #define PORTLED PORTD
  #define LED PD6

  #define PORTBUTTON PINE
  #define BUTTON PE2
#elif defined(__AVR_AT90USB646__)
  DDRD = (1 << PD6);
  PORTD = 0;

  DDRE = 0;
  PORTE = 0;

  #define PORTLED PORTD
  #define LED PD6

  #define PORTBUTTON PINE
  #define BUTTON PE2
#elif defined(__AVR_AT90USB162__)
  DDRD = (1 << PD6);
  PORTD = 0;
  DDRD = 0;
  PORTD = 0;

  #define PORTLED PORTD
  #define LED PD6

  #define PORTBUTTON PIND
  #define BUTTON PD7
#else
#  error Unsupported MCU type
#endif

	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	//clock_prescale_set(clock_div_1);
	
	/* Indicate USB not ready */
	UpdateStatus(Status_USBNotReady);

	/* Initialize USB Subsystem */
	USB_Init();
	
  uint16_t i = 0;
  while(1)
  {
    // blink
    if(i++ == 10000)
      PORTLED |= (1 << LED);  //on
    else if(i == 20000)
    {
      PORTLED &= ~(1 << LED); //off
      i = 0;
    }

    _delay_us(100);

#ifdef DEBUG
    if ((PORTBUTTON & (1 << BUTTON)))
#else
    if (!(PORTBUTTON & (1 << BUTTON)))
#endif
    {
      aes256_ctx_t ctx;
      char out[31];
      char seed[49];
      char data[16];
      char counter[2];
      uint16_t n;
      uint8_t i;

      // read_into_ram:
      eeprom_read(ADDR_SEED, seed, 48);

      // increase counter
      eeprom_read(ADDR_COUNTER, counter, 2);
      n = counter[0] + (counter[1] << 8);
      n++;

      counter[0] = (uint8_t) n;
      counter[1] = (uint8_t) (n >> 8);

      eeprom_write(ADDR_COUNTER, counter, 2);

      memset(&ctx, 0, sizeof(aes256_ctx_t));
      memset(out, 0, 31);

      data[0] = counter[0];
      data[1] = counter[1];

      for(i = 0; i < 14; i++)
        data[i+2] = seed[32+i];

      aes256_init(seed, &ctx);
      aes256_enc(data, &ctx);
      
      SendString("z");
      raw_to_base64(data, 16, out, 30);
      SendString(out);

#ifdef DEBUG
      SendString("\n");
      sleep(500);
#endif

      // wait 500ms
      sleep(500);
    }
  }
}

/* this is a bit unclean:
 * Q: Why do we use a global flag (volatile) flag instead of a clean FIFO?
 * A: This saves RAM, because no string has to be copied into a FIFO! The
 *    atmega16u4 has 1,25kB RAM, so RAM should be no problem. But this firmware
 *    was originally written for the at90usb162 with only 512 bytes of RAM.
 */
void SendString(char *str)
{
  // wait untill send == 0
  while(send);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    tosend = str;
  }

  send = 1;

  // wait till send == 0
  while(send);
}


/* ------------------------------------------------------------------------- */
/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
EVENT_HANDLER(USB_Connect)
{
	/* Indicate USB enumerating */
	UpdateStatus(Status_USBEnumerating);

	/* Default to report protocol on connect */
	UsingReportProtocol = true;
}

/** Event handler for the USB_Reset event. This fires when the USB interface is reset by the USB host, before the
 *  enumeration process begins, and enables the control endpoint interrupt so that control requests can be handled
 *  asynchronously when they arrive rather than when the control endpoint is polled manually.
 */
EVENT_HANDLER(USB_Reset)
{
	/* Select the control endpoint */
	Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);

	/* Enable the endpoint SETUP interrupt ISR for the control endpoint */
	USB_INT_Enable(ENDPOINT_INT_SETUP);
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs.
 */
EVENT_HANDLER(USB_Disconnect)
{
	/* Indicate USB not ready */
	UpdateStatus(Status_USBNotReady);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the keyboard device endpoints.
 */
EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup Keyboard Keycode Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Enable the endpoint IN interrupt ISR for the report endpoint */
	USB_INT_Enable(ENDPOINT_INT_IN);

	/* Setup Keyboard LED Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_LEDS_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_OUT, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(DATA_IN_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_IN, 8,
	                         ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(DATA_OUT_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, 8,
	                         ENDPOINT_BANK_SINGLE);

	/* Enable the endpoint OUT interrupt ISR for the LED report endpoint */
	USB_INT_Enable(ENDPOINT_INT_OUT);

	/* Indicate USB connected and ready */
	UpdateStatus(Status_USBReady);
}

/** Event handler for the USB_UnhandledControlPacket event. This is used to catch standard and class specific
 *  control requests that are not handled internally by the USB library (including the HID commands, which are
 *  all issued via the control endpoint), so that they can be handled appropriately for the application.
 */
EVENT_HANDLER(USB_UnhandledControlPacket)
{
  if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_DEVICE))
  {
    if(bRequest == USB_GET_COMPANY || bRequest == USB_GET_ID || bRequest == USB_GET_OWNER || bRequest == USB_GET_TIMESTAMP)
    {
				Endpoint_ClearSetupReceived();
				
        if(bRequest == USB_GET_COMPANY)
        {
          char response[LEN_COMPANY];
          memset(response, 0, LEN_COMPANY);
          eeprom_read(ADDR_COMPANY, response, LEN_COMPANY);
          Endpoint_Write_Control_Stream_LE(response, LEN_COMPANY);
        }
        else if(bRequest == USB_GET_ID)
        {
          char response[LEN_ID];
          memset(response, 0, LEN_ID);
          eeprom_read(ADDR_ID, response, LEN_ID);
          Endpoint_Write_Control_Stream_LE(response, LEN_ID);
        }
        else if(bRequest == USB_GET_OWNER)
        {
          char response[LEN_OWNER];
          memset(response, 0, LEN_OWNER);
          eeprom_read(ADDR_OWNER, response, LEN_OWNER);
          Endpoint_Write_Control_Stream_LE(response, LEN_OWNER);
        }
        else if(bRequest == USB_GET_TIMESTAMP)
        {
          char response[LEN_TIMESTAMP];
          memset(response, 0, LEN_TIMESTAMP);
          eeprom_read(ADDR_TIMESTAMP, response, LEN_TIMESTAMP);
          Endpoint_Write_Control_Stream_LE(response, LEN_TIMESTAMP);
        }
				
				// Send the flag to the host
				Endpoint_ClearSetupIN();

				// Acknowledge status stage
				while (!(Endpoint_IsSetupOUTReceived()));
				Endpoint_ClearSetupOUT();
    }
  }

  if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_VENDOR | REQREC_DEVICE))
  {
    if(bRequest == USB_WRITE_EEPROM)
    {
      char l;
      uint8_t lock;
      eeprom_read(ADDR_LOCK, &l, 1);

      lock = l;

      uint16_t wValue  = Endpoint_Read_Word_LE();
      //uint16_t wLength = Endpoint_Read_Word_LE();

      uint8_t addr  = wValue >> 8;
      char value = wValue;

      Endpoint_ClearSetupReceived();

      // Acknowledge status stage
      while (!(Endpoint_IsSetupINReady()));
      Endpoint_ClearSetupIN();

      // if stick is _not_ locked
      if(lock == 0 || lock == 255)
        eeprom_write(addr, &value, 1);
    }
    /*
    else if(bRequest == USB_CHANGE_KEY)
    {
      uint8_t entry = (uint8_t) req->wValue;
    }
    */
  }

	// Handle HID Class specific requests
	switch (bRequest)
	{
		case REQ_GetReport:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				USB_KeyboardReport_Data_t KeyboardReportData;

				// Create the next keyboard report for transmission to the host
        CreateKeyboardReport(&KeyboardReportData);

				// Ignore report type and ID number value
				Endpoint_Discard_Word();
				
				// Ignore unused Interface number value
				Endpoint_Discard_Word();

				// Read in the number of bytes in the report to send to the host
				uint16_t wLength = Endpoint_Read_Word_LE();
				
				// If trying to send more bytes than exist to the host, clamp the value at the report size
				if (wLength > sizeof(KeyboardReportData))
				  wLength = sizeof(KeyboardReportData);

				Endpoint_ClearSetupReceived();
	
				// Write the report data to the control endpoint
				Endpoint_Write_Control_Stream_LE(&KeyboardReportData, wLength);
				
				// Finalize the stream transfer to send the last packet or clear the host abort
				Endpoint_ClearSetupOUT();
			}
		
			break;
		case REQ_SetReport:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();
				
				// Wait until the LED report has been sent by the host
				while (!(Endpoint_IsSetupOUTReceived()));

				// Read in the LED report from the host
				uint8_t LEDStatus = Endpoint_Read_Byte();

				// Process the incomming LED report
				ProcessLEDReport(LEDStatus);
			
				// Clear the endpoint data
				Endpoint_ClearSetupOUT();

				// Acknowledge status stage
				while (!(Endpoint_IsSetupINReady()));
				Endpoint_ClearSetupIN();
			}
			
			break;
		case REQ_GetProtocol:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();
				
				// Write the current protocol flag to the host
				Endpoint_Write_Byte(UsingReportProtocol);
				
				// Send the flag to the host
				Endpoint_ClearSetupIN();

				// Acknowledge status stage
				while (!(Endpoint_IsSetupOUTReceived()));
				Endpoint_ClearSetupOUT();
			}
			
			break;
		case REQ_SetProtocol:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				// Read in the wValue parameter containing the new protocol mode
				uint16_t wValue = Endpoint_Read_Word_LE();
								
				Endpoint_ClearSetupReceived();

				// Set or clear the flag depending on what the host indicates that the current Protocol should be
				UsingReportProtocol = (wValue != 0x0000);

				// Acknowledge status stage
				while (!(Endpoint_IsSetupINReady()));
				Endpoint_ClearSetupIN();
			}
			
			break;
		case REQ_SetIdle:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				// Read in the wValue parameter containing the idle period
				uint16_t wValue = Endpoint_Read_Word_LE();
				
				Endpoint_ClearSetupReceived();
				
				// Get idle period in MSB
				IdleCount = (wValue >> 8);
				
				// Acknowledge status stage
				while (!(Endpoint_IsSetupINReady()));
				Endpoint_ClearSetupIN();
			}
			
			break;
		case REQ_GetIdle:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{		
				Endpoint_ClearSetupReceived();
				
				// Write the current idle duration to the host
				Endpoint_Write_Byte(IdleCount);
				
				// Send the flag to the host
				Endpoint_ClearSetupIN();

				// Acknowledge status stage
				while (!(Endpoint_IsSetupOUTReceived()));
				Endpoint_ClearSetupOUT();
			}

			break;
	}
}

/** ISR for the timer 0 compare vector. This ISR fires once each millisecond, and increments the
 *  scheduler elapsed idle period counter when the host has set an idle period.
 */
ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	/* One millisecond has elapsed, decrement the idle time remaining counter if it has not already elapsed */
	if (IdleMSRemaining)
	  IdleMSRemaining--;
}

/** Fills the given HID report data structure with the next HID report to send to the host.
 *
 *  \param ReportData  Pointer to a HID report data structure to be filled
 */
void CreateKeyboardReport(USB_KeyboardReport_Data_t* ReportData)
{
  static uint8_t toggle = 1;
  toggle = !toggle;

  char data = 0;
  char modifier = 0;

	memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));

  if(toggle && send)
  {
    data = *tosend;
    if(data == '\0') 
      send = 0;
    else
    {
      tosend++;
      char2usb(&data, &modifier);

      /*
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        int count = outFIFO.count;
        if(count)
          data    = fifo_get_wait(&outFIFO);
      }*/
    }
  }

  ReportData->Modifier = modifier;
  ReportData->KeyCode[0] = data;
}

/** Processes a received LED report, and updates the board LEDs states to match.
 *
 *  \param LEDReport  LED status report from the host
 */
void ProcessLEDReport(uint8_t LEDReport)
{
}

/** Sends the next HID report to the host, via the keyboard data endpoint. */
static inline void SendNextReport(void)
{
	static USB_KeyboardReport_Data_t PrevKeyboardReportData;
	USB_KeyboardReport_Data_t        KeyboardReportData;
	bool                             SendReport = true;
	
	/* Create the next keyboard report for transmission to the host */
	CreateKeyboardReport(&KeyboardReportData);

	/* Check if the idle period is set */
	if (IdleCount)
	{
		/* Check if idle period has elapsed */
		if (!(IdleMSRemaining))
		{
			/* Reset the idle time remaining counter, must multiply by 4 to get the duration in milliseconds */
			IdleMSRemaining = (IdleCount << 2);
		}
		else
		{
			/* Idle period not elapsed, indicate that a report must not be sent unless the report has changed */
			SendReport = (memcmp(&PrevKeyboardReportData, &KeyboardReportData, sizeof(USB_KeyboardReport_Data_t)) != 0);
		}
	}
	
	/* Save the current report data for later comparison to check for changes */
	PrevKeyboardReportData = KeyboardReportData;

	/* Select the Keyboard Report Endpoint */
	Endpoint_SelectEndpoint(KEYBOARD_EPNUM);

	/* Check if Keyboard Endpoint Ready for Read/Write, and if we should send a report */
	if (Endpoint_ReadWriteAllowed() && SendReport)
	{
		/* Write Keyboard Report Data */
		Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData));

		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearCurrentBank();
	}
}

/** Reads the next LED status report from the host from the LED data endpoint, if one has been sent. */
static inline void ReceiveNextReport(void)
{
}

/** Function to manage status updates to the user. This is done via LEDs on the given board, if available, but may be changed to
 *  log to a serial port, or anything else that is suitable for status updates.
 *
 *  \param CurrentStatus  Current status of the system, from the Keyboard_StatusCodes_t enum
 */
void UpdateStatus(uint8_t CurrentStatus)
{
}

/** ISR for the general Pipe/Endpoint interrupt vector. This ISR fires when an endpoint's status changes (such as
 *  a packet has been received) on an endpoint with its corresponding ISR enabling bits set. This is used to send
 *  HID packets to the host each time the HID interrupt endpoints polling period elapses, as managed by the USB
 *  controller. It is also used to respond to standard and class specific requests send to the device on the control
 *  endpoint, by handing them off to the LUFA library when they are received.
 */
ISR(ENDPOINT_PIPE_vect, ISR_BLOCK)
{
	/* Check if the control endpoint has received a request */
	if (Endpoint_HasEndpointInterrupted(ENDPOINT_CONTROLEP))
	{
		/* Clear the endpoint interrupt */
		Endpoint_ClearEndpointInterrupt(ENDPOINT_CONTROLEP);

		/* Process the control request */
		USB_USBTask();

		/* Handshake the endpoint setup interrupt - must be after the call to USB_USBTask() */
		USB_INT_Clear(ENDPOINT_INT_SETUP);
	}

	/* Check if keyboard endpoint has interrupted */
	if (Endpoint_HasEndpointInterrupted(KEYBOARD_EPNUM))
	{
		/* Select the Keyboard Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_EPNUM);

		/* Clear the endpoint IN interrupt flag */
		USB_INT_Clear(ENDPOINT_INT_IN);

		/* Clear the Keyboard Report endpoint interrupt */
		Endpoint_ClearEndpointInterrupt(KEYBOARD_EPNUM);

		/* Send the next keypress report to the host */
		SendNextReport();
	}

	/* Check if Keyboard LED status Endpoint has interrupted */
	if (Endpoint_HasEndpointInterrupted(KEYBOARD_LEDS_EPNUM))
	{
		/* Select the Keyboard LED Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_LEDS_EPNUM);

		/* Clear the endpoint OUT interrupt flag */
		USB_INT_Clear(ENDPOINT_INT_OUT);

		/* Clear the Keyboard LED Report endpoint interrupt */
		Endpoint_ClearEndpointInterrupt(KEYBOARD_LEDS_EPNUM);

		/* Process the LED report sent from the host */
		ReceiveNextReport();
	}
}
