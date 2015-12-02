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

struct pm_st {
  unsigned long pid;
  unsigned long count;
  unsigned long *addr;
  unsigned long *ent;
};

struct iq_st {
  unsigned int type;
  void *in;
  void *out;
};

#define MIMO_IOCAQ  _IOWR(MIMO_IOC_MAGIC, 0, struct aq_st)
#define MIMO_IOCPM  _IOWR(MIMO_IOC_MAGIC, 1, struct pm_st)
#define MIMO_IOCIQ  _IOWR(MIMO_IOC_MAGIC, 2, struct iq_st)

/* PM related macro */
#define PM_READ        0
#define PM_SET_ACC_BIT 1
#define PM_CLR_ACC_BIT 2
#define PM_READ_ERR    3
#define PM_SET_ERR     4

/* IQ related macro */
#define IQ_INFO_RDMSR 0

#endif
