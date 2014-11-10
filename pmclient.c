#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/ioctl.h>

#include "mimo.h"

int main(int argc, char **argv) {
  int i, fd;
  long ret;
  struct pm_st *pmdata;

  if (argc < 2) {
    printf("Usage: ./aqclient pid\n");
    return -1;
  }

  pmdata = (struct pm_st *)malloc(sizeof(struct pm_st));
  pmdata->pid = (unsigned long) atoi(argv[1]);
  pmdata->count = 1;
  pmdata->addr = (unsigned long *) malloc(sizeof(unsigned long) * pmdata->count);
  pmdata->ent = (unsigned long *) malloc(sizeof(unsigned long) * pmdata->count);

  if ((fd = open("/dev/mimo", O_RDONLY)) < 0) {
    perror("open");
    return -1;
  }

  printf("===== Initialize pmdata: =====\n");
  for (i = 0; i < pmdata->count; i++) {
    pmdata->addr[i] = 0x600db8;
    pmdata->ent[i] = 0;
    printf("%016lx ", pmdata->ent[i]);
  }
  printf("\n");

  if ((ret = ioctl(fd, MIMO_IOCPM, pmdata)) < 0) {
    perror("ioctl_pm");
  }

  printf("===== After IOCTL: =====\n");
  for (i = 0; i < pmdata->count; i++) {
    printf("%016lx ", pmdata->ent[i]);
  }
  printf("\n");

  close(fd);
  return 0;
}
