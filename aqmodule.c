/*
 * Based on liangpig gamo 
 *
 * written by mctrain
 *
 */


//#define DEBUG

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/cdev.h>

#include <linux/mmzone.h>
#include <linux/mm.h>
#include <linux/rmap.h>
#include <linux/pagemap.h>
#include <linux/hugetlb.h>
#include <linux/delay.h>

#include "aqmo.h"

MODULE_LICENSE("GPL");

static int aqmo_init(void);
static void aqmo_exit(void);

static int aqmo_dev_open(struct inode *, struct file *);
static int aqmo_dev_release(struct inode *, struct file *);
static ssize_t aqmo_dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t aqmo_dev_write(struct file *, const char *, size_t, loff_t *);

static long aqmo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

#define DEVICE_NAME "aqmodev"

static struct cdev *aqmo_cdev;

static int aqmo_devno;

static int device_open = 0;

static struct file_operations aqmo_fops = {
  .owner = THIS_MODULE,
  .read = aqmo_dev_read,
  .write = aqmo_dev_write,
  .open = aqmo_dev_open,
  .release = aqmo_dev_release,
  .unlocked_ioctl = aqmo_ioctl,
};

typedef struct anon_vma * (*plav)(struct page *);

typedef struct zoneref * (*nzz)(struct zoneref *, enum zone_type,
					nodemask_t *, struct zone **);

typedef struct anon_vma_chain * (*avitif)(struct rb_root *, unsigned long, unsigned long);

typedef struct hstate * (*sth)(unsigned long);

plav my_page_lock_anon_vma;
nzz my_next_zones_zonelist;
avitif my_anon_vma_interval_tree_iter_first;
sth my_size_to_hstate;

static inline struct hstate *my_page_hstate(struct page *page)
{
	return my_size_to_hstate(PAGE_SIZE << compound_order(page));
}

unsigned long
my_vma_address(struct page *page, struct vm_area_struct *vma)
{
  pgoff_t pgoff = page->index << (PAGE_CACHE_SHIFT - PAGE_SHIFT);
  unsigned long address;

  if (unlikely(is_vm_hugetlb_page(vma)))
    pgoff = page->index << huge_page_order(my_page_hstate(page));
  address = vma->vm_start + ((pgoff - vma->vm_pgoff) << PAGE_SHIFT);
  if (unlikely(address < vma->vm_start || address >= vma->vm_end)) {
    return -EFAULT;
  }
  return address;
}


static inline struct zoneref *my_first_zones_zonelist(struct zonelist *zonelist,
    enum zone_type highest_zoneidx,
    nodemask_t *nodes,
    struct zone **zone)
{
  return my_next_zones_zonelist(zonelist->_zonerefs, highest_zoneidx, nodes,
      zone);
}

#define NR_TO_SCAN 10
#define my_lru_to_page(_head) (list_entry((_head)->prev, struct page, lru))

#define my_for_each_zone_zonelist_nodemask(zone, z, zlist, highidx, nodemask) \
	for (z = my_first_zones_zonelist(zlist, highidx, nodemask, &zone);	\
		zone;							\
		z = my_next_zones_zonelist(++z, highidx, nodemask, &zone))	\

#define my_for_each_zone_zonelist(zone, z, zlist, highidx) \
	my_for_each_zone_zonelist_nodemask(zone, z, zlist, highidx, NULL)

int get_inactivity_addr_list(unsigned long pid)
{
  struct zoneref *z;
  struct zone *zone;
  struct zonelist *zonelist;
  struct list_head *list;
  struct anon_vma *anon_vma;
  struct anon_vma_chain *avc;
  unsigned long scan;
  unsigned int mapcount;
  int i = 0;

  /* init non-exported kernel function pointer */
  my_page_lock_anon_vma = (plav)0xffffffff810d7b3d;
  my_next_zones_zonelist = (nzz)0xffffffff810c85f0;
  my_size_to_hstate = (sth)0xffffffff810e109b;

  zonelist = NODE_DATA(numa_node_id())->node_zonelists;

  my_for_each_zone_zonelist(zone, z, zonelist,
      gfp_zone(GFP_KERNEL)) {
    spin_lock_irq(&zone->lru_lock);
    printk(KERN_ALERT "pages_scanned %lx inactive_ratio %x\n", zone->pages_scanned, zone->inactive_ratio);
    printk(KERN_ALERT "spanned %lx present %lx\n", zone->spanned_pages, zone->present_pages);
    //lruvec = &zone->lruvec;
    list = (struct list_head *)&zone->lru[LRU_INACTIVE_ANON];
    if (list_empty(list)) {
      //printk(KERN_ALERT "empty list %p\n", list);
      continue;
    }
    for (scan = 0; scan < NR_TO_SCAN && !list_empty(list); scan++, list = list->prev) {
      struct page *page;
      page = (struct page *)my_lru_to_page(list);
      printk(KERN_ALERT "scan %lx list %p page %p\n", scan, list, page);
      anon_vma = my_page_lock_anon_vma(page);
      mapcount = page_mapcount(page);
      if (!anon_vma || !mapcount)
        continue;

      list_for_each_entry(avc, &anon_vma->head, same_anon_vma) {
        struct vm_area_struct *vma = avc->vma;
        printk(KERN_ALERT "[%d] %x:%x\n", i, vma->vm_mm->owner->pid, current->pid);
        if (vma->vm_mm->owner == current) {
          unsigned long address = my_vma_address(page, vma);
          printk(KERN_ALERT "[%d] address %lx\n", i, address);
          i++;
        }
      }
    }
    spin_unlock_irq(&zone->lru_lock);
  }
  return 0;
}
static int aqmo_init(void)
{
  int ret;

  if ((ret = alloc_chrdev_region(&aqmo_devno, 0, 1, DEVICE_NAME))) {
    printk(KERN_ALERT "AQModulE: Failed allocating major: %d\n", ret);
    return ret;
  }
  printk(KERN_ALERT "AQModulE: Assigned with device major %d\n", MAJOR(aqmo_devno));

  aqmo_cdev = cdev_alloc();
  aqmo_cdev->ops = &aqmo_fops;
  aqmo_cdev->owner = THIS_MODULE;

  if ((ret = cdev_add(aqmo_cdev, aqmo_devno, 1))) {
    printk(KERN_ALERT "AQModulE: Failed adding character device: %d\n", ret);
    return ret;
  }

  printk(KERN_ALERT "AQModulE: Initialized\n");
  return 0;
}

static void aqmo_exit(void)
{
  printk(KERN_ALERT "AQModulE: Waiting for the aqmod to exit.\n");

  cdev_del(aqmo_cdev);
  unregister_chrdev_region(aqmo_devno, 1);
  printk(KERN_ALERT "AQModulE: Exit\n");
}

/*
 * Methods
 */

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int aqmo_dev_open(struct inode *inode, struct file *file)
{
  device_open++;
  try_module_get(THIS_MODULE);
  return 0;
}

static int aqmo_dev_release(struct inode *inode, struct file *file)
{
  device_open--;    /* We're now ready for our next caller */

  /*
   * Decrement the usage count, or else once you opened the file, you'll
   * never get rid of the module.
   */

  module_put(THIS_MODULE);

  return 0;
}

static ssize_t aqmo_dev_read(struct file *filp, /* see include/linux/fs.h */
    char *buffer,  /* buffer to fill with data */
    size_t length, /* length of the buffer     */
    loff_t * offset)
{
  printk(KERN_ALERT "AQModulE: Read operation isn't supported.\n");
  return -EINVAL;
}

static ssize_t aqmo_dev_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
  printk(KERN_ALERT "AQModulE: Write operation isn't supported.\n");
  return -EINVAL;
}

static long
aqmo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
  printk(KERN_ALERT "AQModulE: IOCTL called by user.\n");
  switch (cmd) {
    case AQMO_IOCQUERY:
      {
        int ret;
        unsigned long pid = (unsigned long) arg;
        printk(KERN_ALERT "AQModulE: QUERY IOCTL : pid = %ld.\n", pid);
        //ret = get_inactivity_addr_list(pid);
      }
      break;
    default:
      printk(KERN_ALERT "AQModulE: IOCTL unknown operation\n");
      return -EINVAL;
  }
  return 0;
}

module_init(aqmo_init);
module_exit(aqmo_exit);
