#ifndef _AQMO_H_
#define _AQMO_H_

#define AQMO_IOC_MAGIC 14

#define MAX_ADDR_SIZE 10

struct aq_st {
  unsigned long pid;
  unsigned long count;
  unsigned long *filled_count;
  unsigned long *addr;
};

#define AQMO_IOCQUERY    _IOWR(AQMO_IOC_MAGIC, 0, struct aq_st)

#endif
