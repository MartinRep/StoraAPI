#include <stdio.h>
#include <time.h>
#include <fcntl.h>      /* open */    
#include <unistd.h>     /* exit */    
#include <sys/ioctl.h>  /* ioctl */   
#include "mv_i2c.h"
#include "board_api.h"

unsigned char str[256];
unsigned int reg_value = 0x0;

void gt_reg_read(unsigned int rgst) {
    unsigned int reg = rgst, value;
    char ch;
    FILE *resource_dump;
    resource_dump = fopen ("/proc/resource_dump" , "w");
    if (!resource_dump) {
      printf ("Error opening file /proc/resource_dump\n");
      exit(-1);
    }
    fprintf (resource_dump,"register  r %08x",reg);
    fclose (resource_dump);
    resource_dump = fopen ("/proc/resource_dump" , "r");
    if (!resource_dump) {
      printf ("Error opening file /proc/resource_dump\n");
      exit(-1);
    }
    fscanf (resource_dump , "%x" , &value);
    fclose (resource_dump);
    reg_value = value;
}

void gt_reg_write(unsigned int rgst, unsigned int vlue) {
    unsigned int reg = rgst, value = vlue;
    unsigned int element;
    char ch;
    FILE *resource_dump;
    resource_dump = fopen ("/proc/resource_dump" , "w");
    if (!resource_dump) {
      printf ("Eror opening file /proc/resource_dump\n");
      exit(-1);
    }
    fprintf (resource_dump,"register  w %08x %08x",reg,value);
    fclose (resource_dump);
}

/****************************************************************************************
* sysLedCtrl - Control the Status of LED
*
* DESCRIPTION:
*       This function can control the POWER LED, PHY 10/100Mbps LED, PHY 1000Mbps LED.
*       The status of POWER LED: ON, OFF, Blink
*       The status of PHY 10/100Mbps LED: ON, OFF
*       The status of PHY 1000Mbps LED: ON, OFF
*
* INPUT:
*		ledName - POWER_LED, PHY100M_LED, PHY1000M_LED 
*       ledState - LED_ON, LED_OFF, LED_BLINK
* OUTPUT:
*       None.
*
* EXAMPLE:
*       Switch on POWER_LED
*       sysLedCtrl(POWER_LED, LED_ON);
*       Switch off POWER_LED
*       sysLedCtrl(POWER_LED, LED_OFF);
*       POWER_LED Blink
*       sysLedCtrl(POWER_LED, LED_BLINK);
* RETURN:
*       None.
*
*****************************************************************************************/
void sysLedCtrl(BOARD_LED_NAME ledName, BOARD_LED_STATE ledState)
{
    int reg, value;
    if(ledName == POWER_LED)
    {   
        reg = 0x1000c;
        gt_reg_read(reg);
        value = reg_value & 0x0FFFFFFF;
        gt_reg_write(reg, value);
        reg = 0x10010;
        gt_reg_read(reg);
        value = reg_value & 0xFFFFFFF0;
        gt_reg_write(reg, value);
        switch(ledState)
        {
            case LED_OFF:
                reg = 0x10100;
                gt_reg_read(reg);
                value = reg_value | 0x80000000;
                gt_reg_write(reg, value);
                reg = 0x10140;
                gt_reg_read(reg);
                value = reg_value | 0x00000001;
                gt_reg_write(reg, value);
                reg = 0x10108;
                gt_reg_read(reg);
                value = reg_value & 0x7FFFFFFF;
                gt_reg_write(reg, value);
                reg = 0x10148;
                gt_reg_read(reg);
                value = reg_value & 0xFFFFFFFE;
                gt_reg_write(reg, value);
                break;
            case LED_ON:
                reg = 0x10108;
                gt_reg_read(reg);
                value = reg_value & 0x7FFFFFFF;
                gt_reg_write(reg, value);
                reg = 0x10148;
                gt_reg_read(reg);
                value = reg_value & 0xFFFFFFFE;
                gt_reg_write(reg, value);
                reg = 0x10100;
                gt_reg_read(reg);
                value = reg_value & 0x7FFFFFFF;
                gt_reg_write(reg, value);
                reg = 0x10140;
                gt_reg_read(reg);
                value = reg_value & 0xFFFFFFFE;
                gt_reg_write(reg, value);
                break;
            case LED_BLINK:
                reg = 0x10108;
                gt_reg_read(reg);
                value = reg_value | 0x80000000;
                gt_reg_write(reg, value);
                reg = 0x10148;
                gt_reg_read(reg);
                value = reg_value | 0x00000001;
                gt_reg_write(reg, value);
                break;
            default:
                printf("Unknown Action!\n");
                break;
        }
    }
    else if(ledName == PHY100M_LED)
    {   
        reg = 0x10014;
        gt_reg_read(reg);
        value = reg_value & 0xFFFF0FFF;
        gt_reg_write(reg, value);
        switch(ledState)
        {
            case LED_OFF:
                reg = 0x10140;
                gt_reg_read(reg);
                value = reg_value & 0xFFFFF7FF;
                gt_reg_write(reg, value);
                break;
            case LED_ON:
                reg = 0x10140;
                gt_reg_read(reg);
                value = reg_value & 0xFFFFE7FF;
                gt_reg_write(reg, value);
                reg = 0x10140;
                gt_reg_read(reg);
                value = reg_value | 0x00000800;
                gt_reg_write(reg, value);
                break;
            default:
                printf("Unknown Action!\n");
                break;
        }
    }
    else if(ledName == PHY1000M_LED)
    {   
        reg = 0x10014;
        gt_reg_read(reg);
        value = reg_value & 0xFFF0FFFF;
        gt_reg_write(reg, value);
        switch(ledState)
        {
            case LED_OFF:
                reg = 0x10140;
                gt_reg_read(reg);
                value = reg_value & 0xFFFFEFFF;
                gt_reg_write(reg, value);
                break;
            case LED_ON:
                reg = 0x10140;
                gt_reg_read(reg);
                value = reg_value & 0xFFFFE7FF;
                gt_reg_write(reg, value);
                reg = 0x10140;
                gt_reg_read(reg);
                value = reg_value | 0x00001000;
                gt_reg_write(reg, value);
                break;
            default:
                printf("Unknown Action!\n");
                break;
        }
    }
    else
    {
        printf("Unknown LED Name!\n");
    }
}

/****************************************************************************************
* sysBtnStatusGet - Detect the Button Status
*
* DESCRIPTION:
*       This function is used to detect the status of Power Button and Reset Button
*
* INPUT:
*		None
* OUTPUT:
*       None.
*
* EXAMPLE:
*       If you call this function sysBtnStatusGet, please press the button and realease,
        the function will caculate the time for which you hold the button. 
* RETURN:
*       None.
*
*****************************************************************************************/
void sysBtnStatusGet()
{
    int reg, value;
    int reset_press_flag = 0;
    int power_press_flag = 0;
    time_t reset_press_time = 0;
    time_t power_press_time = 0;
    reg = 0x10010;
    gt_reg_read(reg);
    value = reg_value & 0xF0F0FFFF;
    gt_reg_write(reg, value);
    reg = 0x10144;
    gt_reg_read(reg);
    value = reg_value | 0x00000050;
    gt_reg_write(reg, value);
    while(1)
    {
        reg = 0x10150;
        gt_reg_read(reg);
        value = 0;
        value = reg_value & 0x00000040;
        if((value > 0) && (reset_press_flag == 0))
        {
            printf("Reset Button is pressed!\n");
            reset_press_flag = 1;
            reset_press_time = time(NULL);
        }
        else if((value == 0) && (reset_press_flag == 1))
        {
            printf("Reset Button is released!\n");
            printf("Using %lds\n", time(NULL) - reset_press_time);
            break;
        }    
        value = 0;
        value = reg_value & 0x00000010;
        if((value == 0) && (power_press_flag == 0))
        {
            printf("Power Button is pressed!\n");
            power_press_flag = 1;
            power_press_time = time(NULL);
        }
        else if((value > 0) && (power_press_flag == 1))
        {
            printf("Power Button is released!\n");
            printf("Using %lds\n", time(NULL) - power_press_time);
            break;
        }
    }
}

/****************************************************************************************
* sysPWROFF - Software power off
*
* DESCRIPTION:
*       This function is used to power off the device by software.
*
* INPUT:
*		None
* OUTPUT:
*       None.
*
* EXAMPLE:
*       Before you call this function, please make sure that all the processes and 
*       services have been stopped.
*       sysPWROFF();  
* RETURN:
*       None.
*
*****************************************************************************************/
void sysPWROFF()
{
    int reg, value;
    reg = 0x10014;
    gt_reg_read(reg);
    value = reg_value & 0xFFFFFFF0;
    gt_reg_write(reg, value);            //MPPSel40-->GPP[40]
    reg = 0x10144;
    gt_reg_read(reg);
    value = reg_value & 0xFFFFFEFF;
    gt_reg_write(reg, value);            //Enable GPP[40] Data Out
    reg = 0x10140;
    gt_reg_read(reg);
    value = reg_value | 0x00000100;
    gt_reg_write(reg, value);            //GPP[40] is Set 1.
}

/****************************************************************************************
* sysGetTemp - Get the temperature of the internal part of the device
*
* DESCRIPTION:
*       This function is used to get the temperature of the internal part of the device
*       from the LM75 digital sensor.
*
* INPUT:
*		real_temp - the pointer to the temperature parameter (float)
* OUTPUT:
*       the value of *real_temp.
*
* EXAMPLE:
*       float curr_temp = 0.0;
*       sysGetTemp(&curr_temp); 
*       printf("%3.2f\n", curr_temp);
* RETURN:
*       None.
*
*****************************************************************************************/
void sysGetTemp(float* real_temp)
{
    MV_I2C_IOCTL_PARMS IoctlParms;
    int boardFd = 0;
    int board_ioctl = BOARD_IOCTL_I2C_CTRL;
    unsigned char linebuf[16];
    unsigned char *cp;
    int temp = 0;
    
    boardFd = open("/dev/mv_i2c", O_RDWR);
           
    if ( boardFd != -1 ) {
        IoctlParms.action = I2C_READ;
        IoctlParms.chanNum = 0;
        IoctlParms.dev_addr = 0x48;
        IoctlParms.offset    = 0;
        IoctlParms.alen    = 1;
        IoctlParms.data    = linebuf;
        IoctlParms.len    = 16;
        ioctl(boardFd, board_ioctl, &IoctlParms);
        close(boardFd);
        cp = IoctlParms.data;
		temp = *cp;
		temp = temp << 8;
		cp++;
		temp = temp | *cp;
		temp = temp >> 7;
        *real_temp = temp * 0.5;
    } else
        printf("Unable to open device file.\n");
}

/****************************************************************************************
* sysAdjustFANSpeed - Adjust the speed of FAN
*
* DESCRIPTION:
*       This function is used to adjust the FAN speed by setting the DutyCycle in FAN 
*       Control Chip register.
*
* INPUT:
*		action - SPEEDUP, SLOWDOWN
* OUTPUT:
*       None
*
* EXAMPLE:
*       SPEEDUP
*       sysAdjustFANSpeed(SPEEDUP);
*       SLOWDOWN
*       sysAdjustFANSpeed(SLOWDOWN);
* RETURN:
*       None.
*
*****************************************************************************************/
void sysAdjustFANSpeed(FAN_ACTION action)
{
    MV_I2C_IOCTL_PARMS IoctlParms;
    int boardFd = 0;
    int board_ioctl = BOARD_IOCTL_I2C_CTRL;
    unsigned char linebuf[16];
    unsigned char *speed_set = NULL;
    float temp = 0;
    boardFd = open("/dev/mv_i2c", O_RDWR);
           
    if ( boardFd != -1 ) {
        IoctlParms.action = I2C_READ;
        IoctlParms.chanNum = 0;
        IoctlParms.dev_addr = 0x1B;
        IoctlParms.offset    = 0x6;
        IoctlParms.alen    = 1;
        IoctlParms.data    = linebuf;
        IoctlParms.len    = 16;
        ioctl(boardFd, board_ioctl, &IoctlParms);
        speed_set = IoctlParms.data;
        printf("DutyCycle = %02x\n", *speed_set);
        if ((*speed_set <= SPEED_MAX) && (*speed_set >= SPEED_MIN))
        {
            if (action == SPEEDUP)
            {
                *speed_set += 0x4;
                if (*speed_set > SPEED_MAX)
                {
                    *speed_set = 0xf;
                }                
            }
            else if (action == SLOWDOWN)
            {
                if (*speed_set == SPEED_MIN)
                {
                    *speed_set = 0x0;
                }
                else
                {
                    *speed_set -= 0x1;
                }
            }
        }
        else
        {
            printf("FAN Fault!\n");
        } 
        IoctlParms.action = I2C_WRITE;
        IoctlParms.chanNum = 0;
        IoctlParms.dev_addr = 0x1B;
        IoctlParms.offset    = 0x6;
        IoctlParms.alen    = 1;
        IoctlParms.data    = speed_set;
        IoctlParms.len    = 1; 
        ioctl(boardFd, board_ioctl, &IoctlParms);
        close(boardFd);
    } else
        printf("Unable to open device file.\n");
}

/****************************************************************************************
* sysGetCurTime - Get current time and date.
*
* DESCRIPTION:
*       This function is used to get current time and date from RTC chip.
*
* INPUT:
*		None
* OUTPUT:
*       Current Time and Date: hh/mm/ss weekday Month/Day/Year
*
* EXAMPLE:
*       sysGetCurTime();
* RETURN:
*       None.
*
*****************************************************************************************/
void sysGetCurTime()
{
    MV_I2C_IOCTL_PARMS IoctlParms;
    int boardFd = 0;
    int board_ioctl = BOARD_IOCTL_I2C_CTRL;
    unsigned char linebuf[16];
    unsigned char *cp;
    boardFd = open("/dev/mv_i2c", O_RDWR);
           
    if ( boardFd != -1 ) {
        IoctlParms.action = I2C_READ;
        IoctlParms.chanNum = 0;
        IoctlParms.dev_addr = 0x68;
        IoctlParms.offset    = 0;
        IoctlParms.alen    = 1;
        IoctlParms.data    = linebuf;
        IoctlParms.len    = 16;
        ioctl(boardFd, board_ioctl, &IoctlParms);
        close(boardFd);
        cp = IoctlParms.data;
        printf("Current Time: %02x:%02x:%02x %02x %02x/%02x/%02x\n", *(cp+2),*(cp+1),*cp,*(cp+3),*(cp+5),*(cp+4),*(cp+6));
    } else
        printf("Unable to open device file.\n");
}

/****************************************************************************************
* sysGetCurTime - Set current time and date.
*
* DESCRIPTION:
*       This function is used to set current time and date into RTC chip.
*
* INPUT:
*		cp - the pointer to the address of Time array.
* OUTPUT:
*       Time and Date to be set: hh/mm/ss weekday Month/Day/Year
* EXAMPLE:
*       char curTime[7];       //The value of time and date should be Hex.
        printf("Second(0~59):");
        scanf("%02x",&curTime[0]);
        printf("Minute(0~59):");
        scanf("%02x",&curTime[1]);
        printf("Hour(0~23):");
        scanf("%02x",&curTime[2]);
        printf("Day(1~7):");
        scanf("%02x",&curTime[3]);
        printf("Date(1~31):");
        scanf("%02x",&curTime[4]);
        printf("Month(1~12):");
        scanf("%02x",&curTime[5]);
        printf("Year(0~99):");
        scanf("%02x",&curTime[6]);
        sysSetCurTime(curTime);
* RETURN:
*       None.
*
*****************************************************************************************/
void sysSetCurTime(unsigned char* cp)
{
    MV_I2C_IOCTL_PARMS IoctlParms;
    int boardFd = 0;
    int board_ioctl = BOARD_IOCTL_I2C_CTRL;
    int i = 0;
    boardFd = open("/dev/mv_i2c", O_RDWR);

    if ( boardFd != -1 ) {
        for (i=0;i<7;i++)
        {
            IoctlParms.action = I2C_WRITE;
            IoctlParms.chanNum = 0;
            IoctlParms.dev_addr = 0x68;
            IoctlParms.offset    = i;
            IoctlParms.alen    = 1;
            IoctlParms.data    = cp;
            IoctlParms.len    = 1;
            ioctl(boardFd, board_ioctl, &IoctlParms);
            cp++;
        }
        close(boardFd);
    } else
        printf("Unable to open device file.\n");
}

/****************************************************************************************
* sysGetCurTime - Set wake up time.
*
* DESCRIPTION:
*       This function is used to set wake up time and date into RTC chip.
*
* INPUT:
*		cp - the pointer to the address of Time array.
* OUTPUT:
*       None
* EXAMPLE:
*       char almTime[7];       //The value of time and date should be Hex.
        printf("Second(0~59):");
        scanf("%02x",&almTime[0]);
        printf("Minute(0~59):");
        scanf("%02x",&almTime[1]);
        printf("Hour(0~23):");
        scanf("%02x",&almTime[2]);
        printf("Day(1~7):");
        scanf("%02x",&almTime[3]);
        almTime[3] = almTime[3] | 0x40;
        sysSetAlmTime(almTime);
        sysPWROFF();           //Power off and wait for the wake time up, then the device 
                                 will power on.
* RETURN:
*       None.
*
*****************************************************************************************/
void sysSetAlmTime(unsigned char* cp)
{
    MV_I2C_IOCTL_PARMS IoctlParms;
    int boardFd = 0;
    int board_ioctl = BOARD_IOCTL_I2C_CTRL;
    int i = 0;
    unsigned char configure_set;
    boardFd = open("/dev/mv_i2c", O_RDWR);

    if ( boardFd != -1 ) {
        for (i=0;i<4;i++)
        {
            IoctlParms.action = I2C_WRITE;
            IoctlParms.chanNum = 0;
            IoctlParms.dev_addr = 0x68;
            IoctlParms.offset    = i + 7;
            IoctlParms.alen    = 1;
            IoctlParms.data    = cp;
            IoctlParms.len    = 1;
            ioctl(boardFd, board_ioctl, &IoctlParms);
            cp++;
        }
        configure_set = 0x07;
        IoctlParms.action = I2C_WRITE;
        IoctlParms.chanNum = 0;
        IoctlParms.dev_addr = 0x68;
        IoctlParms.offset    = 0x0e;
        IoctlParms.alen    = 1;
        IoctlParms.data    = &configure_set;
        IoctlParms.len    = 1;
        ioctl(boardFd, board_ioctl, &IoctlParms);
        
        configure_set = 0x0;
        IoctlParms.action = I2C_WRITE;
        IoctlParms.chanNum = 0;
        IoctlParms.dev_addr = 0x68;
        IoctlParms.offset    = 0x0f;
        IoctlParms.alen    = 1;
        IoctlParms.data    = &configure_set;
        IoctlParms.len    = 1;
        ioctl(boardFd, board_ioctl, &IoctlParms);
        close(boardFd);
    } else
        printf("Unable to open device file.\n");
}

int main(void)
{
	float curr_temp = 0;
	while(1)
	{
		sysGetTemp(&curr_temp);
		printf("Current CPU temp = %3.2f\n", curr_temp);
		sysAdjustFANSpeed(SLOWDOWN);
		sleep(3);
	}
}
