#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/ioctl.h>

#include "aqmo.h"

int main(int argc, char **argv) {
  int i, fd;
  long ret;
  struct aq_st *aqdata;

  if (argc < 3) {
    printf("Usage: ./aqclient pid addr_count\n");
    return -1;
  }

  aqdata = (struct aq_st *)malloc(sizeof(struct aq_st));
  aqdata->pid = (unsigned long) atoi(argv[1]);
  aqdata->count = (unsigned long) atoi(argv[2]);
  aqdata->filled_count = (unsigned long *) malloc(sizeof(unsigned long));
  aqdata->addr = (unsigned long *) malloc(sizeof(unsigned long) * aqdata->count);
  memset((void *) aqdata->filled_count, 0, sizeof(unsigned long));
  memset((void *) aqdata->addr, 0, sizeof(unsigned long) * aqdata->count);

  if ((fd = open("/dev/aqmo", O_RDONLY)) < 0) {
    perror("open");
    return -1;
  }

  printf("===== Before IOCTL: =====\n");
  printf("filled_count: %ld\n", *aqdata->filled_count);
  for (i = 0; i < aqdata->count; i++) {
    printf("%016lx ", aqdata->addr[i]);
  }
  printf("\n");

  if ((ret = ioctl(fd, AQMO_IOCQUERY, aqdata)) < 0) {
    perror("ioctl_query");
  }

  printf("===== After IOCTL: =====\n");
  printf("filled_count: %ld\n", *aqdata->filled_count);
  for (i = 0; i < aqdata->count; i++) {
    printf("%016lx ", aqdata->addr[i]);
  }
  printf("\n");

  close(fd);
  return 0;
}
