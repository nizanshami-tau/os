// Declare what kind of code we want
// from the header files. Defining __KERNEL__
// and MODULE allows us to access kernel-level
// code not usually available to userspace programs.
#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE


#include <linux/kernel.h>   /* We're doing kernel work */
#include <linux/module.h>   /* Specifically, a module */
#include <linux/fs.h>       /* for register_chrdev */
#include <linux/uaccess.h>  /* for get_user and put_user */
#include <linux/string.h>   /* for memset. NOTE - not string.h!*/
#include <linux/errno.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");

//Our custom definitions of IOCTL operations
#include "message_slot.h"

struct message_slots
{
  int minor;
  struct slot *slot;
  struct message_slots *next;
};

struct slot{
    unsigned int id;
    char *massage;
    int length;
    struct slot *next;
};

static struct message_slots *root;

/* return the message_slot we the maching minor or crate new one*/
struct message_slots *search_slot(int minor){
  if(!root){
    root = kmalloc(sizeof(struct message_slots), GFP_KERNEL);
    if(!root){
      return NULL;
    }
    root->minor = minor;
    root->slot = NULL;
    root->next = NULL;
    return root;
  }
  while(root->next != NULL){
    if(root->minor == minor){
      return root;
    }
    root = root->next;
  }
  if(root->minor == minor){
    return root;
  }
  root->next = kmalloc(sizeof(struct message_slots), GFP_KERNEL);
  if(!root->next){
    return NULL;
  }
  root = root->next;
  root->minor = minor;
  root->slot = NULL;
  root->next = NULL;
  return root;   

}

//================== DEVICE FUNCTIONS ===========================
static int device_open( struct inode* inode,
                        struct file*  file )
{
  int minor;
  struct message_slots *slots;

  
  printk("Invoking device_open(%p)\n", file);
  
  minor = iminor(inode);
  slots = search_slot(minor);
  if(!slots){
    return -EIO;
  }

  return SUCCESS;
}

//---------------------------------------------------------------
static int device_release( struct inode* inode,
                           struct file*  file)
{
  return SUCCESS;
}
//---------------------------------------------------------------
// a process which has already opened
// the device file attempts to read from it
static ssize_t device_read( struct file* file,
                            char __user* buffer,
                            size_t       length,
                            loff_t*      offset )
{
  // read doesnt really do anything (for now)
  printk( "Invocing device_read(%p,%ld)"
          ,file, length);
  //invalid argument error
  return SUCCESS;
}

//---------------------------------------------------------------
// a processs which has already opened
// the device file attempts to write to it
static ssize_t device_write( struct file*       file,
                             const char __user* buffer,
                             size_t             length,
                             loff_t*            offset)
{
  int i;
  printk("Invoking device_write(%p,%ld)\n", file, length);
  i = 0;


  // return the number of input characters used
  return i;
}

//----------------------------------------------------------------
static long device_ioctl( struct   file* file,
                          unsigned int   ioctl_command_id,
                          unsigned long  ioctl_param )
{
  if(ioctl_command_id == MSG_SLOT_CHANNEL && ioctl_param > 0 && file){
    file->private_data = (void *) ioctl_param;
    return SUCCESS;
  }
  return -EINVAL;
}

//==================== DEVICE SETUP =============================

// This structure will hold the functions to be called
// when a process does something to the device we created
struct file_operations Fops =
{
  .owner	  = THIS_MODULE, 
  .read           = device_read,
  .write          = device_write,
  .open           = device_open,
  .unlocked_ioctl = device_ioctl,
  .release        = device_release,
};

//---------------------------------------------------------------
// Initialize the module - Register the character device
static int __init simple_init(void)
{
  int rc = -1;
  
  // Register driver capabilities. Obtain major num
  rc = register_chrdev( MAJOR_NUM, DEVICE_RANGE_NAME, &Fops );

  // Negative values signify an error
  if( rc < 0 )
  {
    printk( KERN_ALERT "%s registraion failed for  %d\n",
                       DEVICE_FILE_NAME, MAJOR_NUM );
    return rc;
  }

  printk( "Registeration is successful. ");
  printk( "If you want to talk to the device driver,\n" );
  printk( "you have to create a device file:\n" );
  printk( "mknod /dev/%s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM );
  printk( "You can echo/cat to/from the device file.\n" );
  printk( "Dont forget to rm the device file and "
          "rmmod when you're done\n" );

  return 0;
}

//---------------------------------------------------------------
static void __exit cleanup(void)
{
  // Unregister the device
  // Should always succeed
  unregister_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME);
}

//---------------------------------------------------------------
module_init(simple_init);
module_exit(cleanup);

//========================= END OF FILE =========================
