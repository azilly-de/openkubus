#define ADDR_LOCK        1  // [1-1]:    1 byte
#define ADDR_TIMESTAMP   2  // [2-5]:    4 bytes; Y2K38 bug
#define ADDR_ID          6  // [6-9]:    4 bytes
#define ADDR_COMPANY     10 // [10-40]:  30 chars + NUL
#define ADDR_OWNER       41 // [41-72]:  30 chars + NUL
#define ADDR_COUNTER     73 // [73-74]:  2 bytes
#define ADDR_SEED        75 // [75-122]: 48 bytes

#define USB_CHANGE_KEY    20
#define USB_GET_COMPANY   21
#define USB_GET_ID        22
#define USB_GET_OWNER     23
#define USB_GET_TIMESTAMP 24
#define USB_WRITE_EEPROM  25

#define LEN_COMPANY       31
#define LEN_ID            4
#define LEN_OWNER         31
#define LEN_TIMESTAMP     4
#define LEN_SEED          46
