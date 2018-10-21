/*====================================================================== 
    A I2C driver as an example of char device drivers   
======================================================================*/  
#include <linux/module.h>   
#include <linux/types.h>   
#include <linux/fs.h>   
#include <linux/errno.h>   
#include <linux/mm.h>   
#include <linux/sched.h>   
#include <linux/init.h>   
#include <linux/cdev.h>   
#include <asm/io.h>   
#include <asm/system.h>   
#include <asm/uaccess.h>   
#include "mv_i2c.h"
#include "twsi/mvTwsi.h"
  
#define GLOBALMEM_SIZE  0x1000  
#define MV_I2C_MAJOR 220    
  
static mv_i2c_major = MV_I2C_MAJOR;  
/*globalmem struct*/  
struct mv_i2c_dev                                       
{                                                          
    struct cdev cdev; /*cdev struct*/                         
    unsigned char mem[GLOBALMEM_SIZE]; /*ȫglobal memory */          
};  
  
struct mv_i2c_dev *mv_i2c_devp; /* global struct pointer */  
 
int mv_i2c_open(struct inode *inode, struct file *filp)  
{  
 
    filp->private_data = mv_i2c_devp;  
    return 0;  
}  
  
int mv_i2c_release(struct inode *inode, struct file *filp)  
{  
    return 0;  
}  

void mv_i2c_setup_init(MV_U8 chanNum, int speed, int slaveaddr)
{
	MV_TWSI_ADDR slave;
	slave.type = ADDR7_BIT;
	slave.address = slaveaddr;
	mvTwsiInit(chanNum, speed, CFG_TCLK, &slave, 0);
}
  
/* ioctl */  
static int mv_i2c_ioctl(struct inode *inodep, struct file *filp, unsigned  
  int cmd, unsigned long arg)  
{  
    struct mv_i2c_dev *dev = filp->private_data;
  
    unsigned int error = 0;
    MV_I2C_IOCTL_PARMS ctrlParms;
    MV_TWSI_SLAVE	twsiSlave;
	unsigned int i2cFreq = CFG_I2C_SPEED;
	MV_TWSI_ADDR eepromAddress;
	unsigned int status = 0;
    
    //printk(KERN_INFO "%s()\n", __FUNCTION__);

    switch (cmd) {
        case BOARD_IOCTL_I2C_CTRL:
		    if (copy_from_user((void*)&ctrlParms, (void*)arg, sizeof(ctrlParms)) == 0)
            {
                switch (ctrlParms.action)
                {
                    case I2C_READ:
	                    twsiSlave.slaveAddr.type = ADDR7_BIT;
	                    twsiSlave.slaveAddr.address = ctrlParms.dev_addr;
	                    if(ctrlParms.alen != 0){
	                    	twsiSlave.validOffset = MV_TRUE;
	                    	twsiSlave.offset = ctrlParms.offset;
	                    	if(ctrlParms.alen == 2)
	                    	{
	                    		twsiSlave.moreThen256 = MV_TRUE;
	                    	}
	                    	else
	                    	{
	                    		twsiSlave.moreThen256 = MV_FALSE;
	                    	}
	                    }	                    
	                    mv_i2c_setup_init(ctrlParms.chanNum, i2cFreq,0); /* set the i2c frequency */
	                    mvTwsiRead (ctrlParms.chanNum, &twsiSlave, ctrlParms.data, ctrlParms.len);
		                copy_to_user((void*)arg, (void*)&ctrlParms, sizeof(ctrlParms));
                        break;
                    case I2C_WRITE:
	                    twsiSlave.slaveAddr.type = ADDR7_BIT;
	                    twsiSlave.slaveAddr.address = ctrlParms.dev_addr;
	                    if(ctrlParms.alen != 0){
	                    	twsiSlave.validOffset = MV_TRUE;
	                    	twsiSlave.offset = ctrlParms.offset;
	                    	if(ctrlParms.alen == 2)
	                    	{
	                    		twsiSlave.moreThen256 = MV_TRUE;
	                    	}
	                    	else
	                    	{
	                    		twsiSlave.moreThen256 = MV_FALSE;
	                    	}
	                    }
	                    mv_i2c_setup_init(ctrlParms.chanNum, i2cFreq,0); /* set the i2c frequency */	
                        mvTwsiWrite (ctrlParms.chanNum, &twsiSlave, ctrlParms.data, ctrlParms.len);
                        break;
                    case I2C_PROBE:
	                        mv_i2c_setup_init(ctrlParms.chanNum, i2cFreq,0); /* set the i2c frequency */
                            
	                        status = mvTwsiStartBitSet(ctrlParms.chanNum);
                            
	                        if (status) {
	                        	//DP(printf("Transaction start failed: 0x%02x\n", status));
	                        	mvTwsiStopBitSet(ctrlParms.chanNum);
	                        	return (int)status;
	                        }
                            
	                        eepromAddress.type = ADDR7_BIT;
	                        eepromAddress.address = ctrlParms.dev_addr;
                            
	                        status = mvTwsiAddrSet(ctrlParms.chanNum, &eepromAddress, MV_TWSI_WRITE); /* send the slave address */
	                        if (status) {
	                        	//DP(printf("Failed to set slave address: 0x%02x\n", status));
	                        	mvTwsiStopBitSet(ctrlParms.chanNum);
	                        	return (int)status;
	                        }
	              
                            /* issue a stop bit */
                            mvTwsiStopBitSet(ctrlParms.chanNum);
                            break;
                    default:
                        error = -EINVAL;
                        printk("ioctl_command: invalid command\n");
                        break;
                }
            }
            else
                error = -EFAULT;
            break;
        default:
                dprintk("%s(unknown ioctl 0x%x)\n", __FUNCTION__, cmd);
                error = EINVAL;
                break;
    }
    return(-error);
}  
  

static ssize_t mv_i2c_read(struct file *filp, char __user *buf, size_t size,  
  loff_t *ppos)  
{  
    unsigned long p =  *ppos;  
    unsigned int count = size;  
    int ret = 0;  
    struct mv_i2c_dev *dev = filp->private_data; 
    
    
    if (p >= GLOBALMEM_SIZE)  
      return count ?  - ENXIO: 0;  
    if (count > GLOBALMEM_SIZE - p)  
      count = GLOBALMEM_SIZE - p;  
    
    
    if (copy_to_user(buf, (void*)(dev->mem + p), count))  
    {  
      ret =  - EFAULT;  
    }  
    else  
    {  
      *ppos += count;  
      ret = count;  
        
      printk(KERN_INFO "read %d bytes(s) from %d\n", count, p);  
    }  
    
    return ret;  
}  
  
 
static ssize_t mv_i2c_write(struct file *filp, const char __user *buf,  
  size_t size, loff_t *ppos)  
{  
    unsigned long p =  *ppos;  
    unsigned int count = size;  
    int ret = 0;  
    struct mv_i2c_dev *dev = filp->private_data; 
      
    if (p >= GLOBALMEM_SIZE)  
      return count ?  - ENXIO: 0;  
    if (count > GLOBALMEM_SIZE - p)  
      count = GLOBALMEM_SIZE - p;  
         
    if (copy_from_user(dev->mem + p, buf, count))  
      ret =  - EFAULT;  
    else  
    {  
      *ppos += count;  
      ret = count;  
        
      printk(KERN_INFO "written %d bytes(s) from %d\n", count, p);  
    }  
    
    return ret;  
}  
    
static loff_t mv_i2c_llseek(struct file *filp, loff_t offset, int orig)  
{  
  loff_t ret = 0;  
  switch (orig)  
  {  
    case 0:   
      if (offset < 0)  
      {  
        ret =  - EINVAL;  
        break;  
      }  
      if ((unsigned int)offset > GLOBALMEM_SIZE)  
      {  
        ret =  - EINVAL;  
        break;  
      }  
      filp->f_pos = (unsigned int)offset;  
      ret = filp->f_pos;  
      break;  
    case 1:     
      if ((filp->f_pos + offset) > GLOBALMEM_SIZE)  
      {  
        ret =  - EINVAL;  
        break;  
      }  
      if ((filp->f_pos + offset) < 0)  
      {  
        ret =  - EINVAL;  
        break;  
      }  
      filp->f_pos += offset;  
      ret = filp->f_pos;  
      break;  
    default:  
      ret =  - EINVAL;  
      break;  
  }  
  return ret;  
}  
  
  
static const struct file_operations mv_i2c_fops =  
{  
  .owner = THIS_MODULE,  
  .llseek = mv_i2c_llseek,  
  .read = mv_i2c_read,  
  .write = mv_i2c_write,  
  .ioctl = mv_i2c_ioctl,  
  .open = mv_i2c_open,  
  .release = mv_i2c_release,  
};  
  
  
static void mv_i2c_setup_cdev(struct mv_i2c_dev *dev, int index)  
{  
  int err, devno = MKDEV(mv_i2c_major, index);  
  
  cdev_init(&dev->cdev, &mv_i2c_fops);  
  dev->cdev.owner = THIS_MODULE;  
  dev->cdev.ops = &mv_i2c_fops;  
  err = cdev_add(&dev->cdev, devno, 1);  
  if (err)  
    printk(KERN_NOTICE "Error %d adding I2C", err, index);  
}  
  
int mv_i2c_init(void)  
{  
  int result;  
  dev_t devno = MKDEV(mv_i2c_major, 0);  
  
  
  if (mv_i2c_major)  
    result = register_chrdev_region(devno, 1, "mv_i2c");  
  else    
  {  
    result = alloc_chrdev_region(&devno, 0, 1, "mv_i2c");  
    mv_i2c_major = MAJOR(devno);  
  }    
  if (result < 0)  
    return result;  
      
  
  mv_i2c_devp = kmalloc(sizeof(struct mv_i2c_dev), GFP_KERNEL);  
  if (!mv_i2c_devp)      
  {  
    result =  - ENOMEM;  
    goto fail_malloc;  
  }  
  memset(mv_i2c_devp, 0, sizeof(struct mv_i2c_dev));  
    
  mv_i2c_setup_cdev(mv_i2c_devp, 0);  
  return 0;  
  
  fail_malloc: unregister_chrdev_region(devno, 1);  
  return result;  
}  
  
 
void mv_i2c_exit(void)  
{  
  cdev_del(&mv_i2c_devp->cdev);     
  kfree(mv_i2c_devp);      
  unregister_chrdev_region(MKDEV(mv_i2c_major, 0), 1);  
}  
  
MODULE_AUTHOR("Tony W.Y. Wang");  
MODULE_LICENSE("Dual BSD/GPL");  
  
module_param(mv_i2c_major, int, S_IRUGO);  
  
module_init(mv_i2c_init);  
module_exit(mv_i2c_exit);  
