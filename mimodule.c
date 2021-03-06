/*
 * My Ioctl Module (MIMO)
 * used by user for invoking IOCTL to interact with kernel
 * 
 * Written by mctrain based on liangpig gamo 
 */


//#define DEBUG

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/tty.h>

#include <linux/mmzone.h>
#include <linux/mm.h>
#include <linux/rmap.h>
#include <linux/pagemap.h>
#include <linux/hugetlb.h>
#include <linux/delay.h>

#include "mimo.h"

MODULE_LICENSE("GPL");

static int mimo_init(void);
static void mimo_exit(void);

static int mimo_dev_open(struct inode *, struct file *);
static int mimo_dev_release(struct inode *, struct file *);
static ssize_t mimo_dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t mimo_dev_write(struct file *, const char *, size_t, loff_t *);

static long mimo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

#define DEVICE_NAME "mimodev"

static struct cdev *mimo_cdev;

static int mimo_devno;

static int device_open = 0;

static struct file_operations mimo_fops = {
  .owner = THIS_MODULE,
  .read = mimo_dev_read,
  .write = mimo_dev_write,
  .open = mimo_dev_open,
  .release = mimo_dev_release,
  .unlocked_ioctl = mimo_ioctl,
};

typedef struct anon_vma * (*plavr)(struct page *);

typedef struct zoneref * (*nzz)(struct zoneref *, enum zone_type,
					nodemask_t *, struct zone **);

typedef struct anon_vma_chain * (*avitif)(struct rb_root *, unsigned long, unsigned long);

typedef struct hstate * (*sth)(unsigned long);

typedef struct anon_vma_chain * (*avitin)(struct anon_vma_chain *, unsigned long, unsigned long);

plavr my_page_lock_anon_vma_read;
nzz my_next_zones_zonelist;
avitif my_anon_vma_interval_tree_iter_first;
sth my_size_to_hstate;
avitin my_anon_vma_interval_tree_iter_next;

static inline struct hstate *my_page_hstate(struct page *page)
{
	return my_size_to_hstate(PAGE_SIZE << compound_order(page));
}

unsigned long
my_vma_address(struct page *page, struct vm_area_struct *vma)
{
  pgoff_t pgoff = page->index << (PAGE_CACHE_SHIFT - PAGE_SHIFT);

  if (unlikely(is_vm_hugetlb_page(vma)))
    pgoff = page->index << huge_page_order(my_page_hstate(page));

  return vma->vm_start + ((pgoff - vma->vm_pgoff) << PAGE_SHIFT);
}

static inline struct zoneref *my_first_zones_zonelist(struct zonelist *zonelist,
    enum zone_type highest_zoneidx,
    nodemask_t *nodes,
    struct zone **zone)
{
  return my_next_zones_zonelist(zonelist->_zonerefs, highest_zoneidx, nodes,
      zone);
}

#define my_lru_to_page(_head) (list_entry((_head)->prev, struct page, lru))

#define my_anon_vma_interval_tree_foreach(avc, root, start, last)		 \
	for (avc = my_anon_vma_interval_tree_iter_first(root, start, last); \
	     avc; avc = my_anon_vma_interval_tree_iter_next(avc, start, last))

#define my_for_each_zone_zonelist_nodemask(zone, z, zlist, highidx, nodemask) \
	for (z = my_first_zones_zonelist(zlist, highidx, nodemask, &zone);	\
		zone;							\
		z = my_next_zones_zonelist(++z, highidx, nodemask, &zone))	\

#define my_for_each_zone_zonelist(zone, z, zlist, highidx) \
	my_for_each_zone_zonelist_nodemask(zone, z, zlist, highidx, NULL)


int write_console(char *str)
{
  struct tty_struct *my_tty;
  if ((my_tty = current->signal->tty) != NULL) {
    (my_tty->driver->ops->write)(my_tty, str, strlen(str));
    return 0;
  }
  return -1;
}

static int mimo_init(void)
{
  int ret;

  if ((ret = alloc_chrdev_region(&mimo_devno, 0, 1, DEVICE_NAME))) {
    printk(KERN_ALERT "MIModulE: Failed allocating major: %d\n", ret);
    return ret;
  }
  printk(KERN_ALERT "MIModulE: Assigned with device major %d\n", MAJOR(mimo_devno));

  mimo_cdev = cdev_alloc();
  mimo_cdev->ops = &mimo_fops;
  mimo_cdev->owner = THIS_MODULE;

  if ((ret = cdev_add(mimo_cdev, mimo_devno, 1))) {
    printk(KERN_ALERT "MIModulE: Failed adding character device: %d\n", ret);
    return ret;
  }

  printk(KERN_ALERT "MIModulE: Initialized\n");
  return 0;
}

static void mimo_exit(void)
{
  printk(KERN_ALERT "MIModulE: Waiting for the mimod to exit.\n");

  cdev_del(mimo_cdev);
  unregister_chrdev_region(mimo_devno, 1);
  printk(KERN_ALERT "MIModulE: Exit\n");
}

/*
 * Methods
 */

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int mimo_dev_open(struct inode *inode, struct file *file)
{
  device_open++;
  try_module_get(THIS_MODULE);
  return 0;
}

static int mimo_dev_release(struct inode *inode, struct file *file)
{
  device_open--;    /* We're now ready for our next caller */

  /*
   * Decrement the usage count, or else once you opened the file, you'll
   * never get rid of the module.
   */

  module_put(THIS_MODULE);

  return 0;
}

static ssize_t mimo_dev_read(struct file *filp, /* see include/linux/fs.h */
    char *buffer,  /* buffer to fill with data */
    size_t length, /* length of the buffer     */
    loff_t * offset)
{
  printk(KERN_ALERT "MIModulE: Read operation isn't supported.\n");
  return -EINVAL;
}

static ssize_t mimo_dev_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
  printk(KERN_ALERT "MIModulE: Write operation isn't supported.\n");
  return -EINVAL;
}

int get_inactivity_addr(struct aq_st * aqdata)
{
  struct zoneref *z;
  struct zone *zone;
  struct lruvec *lruvec;
  struct zonelist *zonelist;
  struct list_head *list, *first;
  struct anon_vma *anon_vma;
  pgoff_t pgoff;
  struct anon_vma_chain *avc;
  unsigned long scan;
  unsigned int mapcount;
  
  unsigned long pid, count;
  unsigned long *k_addr;
 
  pid = aqdata->pid;
  count = aqdata->count;

  k_addr = (unsigned long *)kmalloc(sizeof(unsigned long) * count, GFP_KERNEL);
  memset(k_addr, 0, sizeof(unsigned long) * count);

  /* init non-exported kernel function pointer */
  my_page_lock_anon_vma_read = (plavr)0xffffffff81158d20;
  my_next_zones_zonelist = (nzz)0xffffffff8113fcb0;
  my_anon_vma_interval_tree_iter_first = (avitif)0xffffffff81147770;
  my_size_to_hstate = (sth)0xffffffff81165ce0;
  my_anon_vma_interval_tree_iter_next = (avitin)0xffffffff811477a0;

  zonelist = NODE_DATA(numa_node_id())->node_zonelists;

  scan = 3;

  my_for_each_zone_zonelist(zone, z, zonelist,
      gfp_zone(GFP_KERNEL)) {
    spin_lock_irq(&zone->lru_lock);
    lruvec = &zone->lruvec;
    list = &lruvec->lists[LRU_INACTIVE_ANON];
    if (list_empty(list)) {
#ifdef DEBUG
      printk(KERN_ALERT "MIModulE-AQ: empty list %p\n", list);
#endif
      continue;
    }
    first = list;
    for (scan = 0; scan < count && !list_empty(list) && first != list->prev; list = list->prev) {
      struct page *page;
      page = (struct page *) my_lru_to_page(list);
      if (page == NULL) {
        continue;
      }
      anon_vma = my_page_lock_anon_vma_read(page);
      mapcount = page_mapcount(page);
      if (!anon_vma || !mapcount)
        continue;
      
      pgoff = page->index << (PAGE_CACHE_SHIFT - PAGE_SHIFT);
      my_anon_vma_interval_tree_foreach(avc, &anon_vma->rb_root, pgoff, pgoff) {
        struct vm_area_struct *vma = avc->vma;
#ifdef DEBUG
        printk(KERN_ALERT "MIModulE-AQ: [%ld] %d:%ld\n", scan, vma->vm_mm->owner->pid, pid);
#endif
        if (vma->vm_mm->owner->pid == pid) {
          unsigned long address = my_vma_address(page, vma);
          k_addr[scan] = address;
#ifdef DEBUG
          printk(KERN_ALERT "MIModulE-AQ: [%ld] address %lx\n", scan, address);
#endif
          scan++;
        }
      }
    }
    spin_unlock_irq(&zone->lru_lock);
  }

#ifdef DEBUG
  printk(KERN_ALERT "MIModulE-QUERY: finish scan : %ld %p\n", scan, aqdata->filled_count);
#endif

  if (copy_to_user(aqdata->filled_count, &scan, sizeof(unsigned long))) {
    printk(KERN_ALERT "MIModulE: copy_to_user filled_count error\n");
    return -EFAULT;
  }
  if (scan > 0) {
    if (copy_to_user(aqdata->addr, k_addr, sizeof(unsigned long) * scan)) {
      printk(KERN_ALERT "MIModulE: copy_to_user addr error\n");
      return -EFAULT;
    }
  }
  kfree(k_addr);
  return 0;
}

static long
mimo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
#ifdef DEBUG
  printk(KERN_ALERT "MIModulE: IOCTL called by user.\n");
#endif
  switch (cmd) {
    case MIMO_IOCAQ:
      {
        struct aq_st k_aqdata;
        
#ifdef DEBUG
        printk(KERN_ALERT "MIModulE: ACTIVITY QUERY IOCTL.\n");
#endif
        
        if (copy_from_user(&k_aqdata, (struct aq_st *) arg, sizeof(struct aq_st))) {
          printk(KERN_ALERT "MIModulE-AQ: copy_from_user error\n");
          return -EFAULT;
        }

#ifdef DEBUG
        printk(KERN_ALERT "MIModulE-AQ: pid %ld count %ld addr %p\n", k_aqdata.pid, k_aqdata.count, k_aqdata.addr);
#endif

        if (get_inactivity_addr(&k_aqdata)) {
          printk(KERN_ALERT "MIModulE-AQ: get_inactivity_addr error\n");
          return -EFAULT;
        }
      }
      break;
    default:
      printk(KERN_ALERT "MIModulE: IOCTL unknown operation\n");
      return -EINVAL;
  }
  return 0;
}

module_init(mimo_init);
module_exit(mimo_exit);
