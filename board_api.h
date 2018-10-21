typedef enum
{
    USB_LED=30,
    POWER_LED=31,
    PHY100M_LED=43,
    PHY1000M_LED=44
}BOARD_LED_NAME;

typedef enum 
{
    LED_OFF=0,
    LED_ON,
    LED_BLINK
}BOARD_LED_STATE;

typedef enum 
{
    POWER_OFF=0,
    POWER_ON
}BOARD_POWER_STATE;

typedef enum 
{
    SPEEDUP=0,
    SLOWDOWN
}FAN_ACTION;

typedef enum 
{
    GETCURTIME=0,
    SETCURTIME,
    SETALARMTIME
}RTC_ACTION;

typedef enum
{
    SPEED_MIN = 0x0,
    SPEED_1   = 0x1,
    SPEED_DEFAULT = 0x2,
    SPEED_2   = 0x3,
    SPEED_3   = 0x4,
    SPEED_4   = 0x5,
    SPEED_5   = 0x6,
    SPEED_6   = 0x7,
    SPEED_7   = 0x8,
    SPEED_8   = 0x9,
    SPEED_9   = 0xa,
    SPEED_10  = 0xb,
    SPEED_11  = 0xc,
    SPEED_12  = 0xd,
    SPEED_13  = 0xe,
    SPEED_MAX  = 0xf,
}FAN_SPEED;

void sysLedCtrl(BOARD_LED_NAME, BOARD_LED_STATE);
void sysBtnStatusGet();
void sysPWROFF();
void sysGetTemp(float*);
void sysAdjustFANSpeed(FAN_ACTION);
void sysGetCurTime();
void sysSetCurTime(unsigned char*);
void sysSetAlmTime(unsigned char*);
