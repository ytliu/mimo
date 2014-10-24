#ifndef _MIMO_H_
#define _MIMO_H_

#define MIMO_IOC_MAGIC 14

#define MAX_ADDR_SIZE 10

struct aq_st {
  unsigned long pid;
  unsigned long count;
  unsigned long *filled_count;
  unsigned long *addr;
};

#define MIMO_IOCAQ  _IOWR(MIMO_IOC_MAGIC, 0, struct aq_st)

#endif
