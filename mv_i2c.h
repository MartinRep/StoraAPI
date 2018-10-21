/*******************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
*******************************************************************************/
/*
 * common debug for all
 */
#undef DEBUG

#ifdef DEBUG
#define dprintk   printk
#else
#define dprintk(a...)
#endif

#define CFG_I2C_SPEED   100000		/* I2C speed default */
#define CFG_TCLK                mvBoardTclkGet()
#define BOARD_IOCTL_I2C_CTRL 0x1 

typedef enum 
{
    I2C_READ=0,
    I2C_WRITE,
    I2C_PROBE
}MV_I2C_ACTION;

typedef struct MVI2CIoctlParams
{
    MV_I2C_ACTION action;
    char chanNum;
    char dev_addr;
    int offset;
    int alen;
    char* data;
    int len;
} MV_I2C_IOCTL_PARMS;





