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
    printf("Usage: ./pmclient pid\n");
    return -1;
  }

  pmdata = (struct pm_st *)malloc(sizeof(struct pm_st));
  pmdata->pid = (unsigned long) atoi(argv[1]);
  pmdata->count = 2;
  pmdata->addr = (unsigned long *) malloc(sizeof(unsigned long) * pmdata->count);
  pmdata->ent = (unsigned long *) malloc(sizeof(unsigned long) * pmdata->count);

  if ((fd = open("/dev/mimo", O_RDONLY)) < 0) {
    perror("open");
    return -1;
  }

  pmdata->addr[0] = 0x601260;
  pmdata->addr[1] = 0x400650;

  printf("===== 1st IOCTL read: =====\n");
  for (i = 0; i < pmdata->count; i++) {
    pmdata->ent[i] = PM_READ;
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
  printf("\n\n");


  printf("===== IOCTL set: =====\n");
  for (i = 0; i < pmdata->count; i++) {
    pmdata->ent[i] = PM_CLR_ACC_BIT;
    printf("%lx ", pmdata->ent[i]);
  }
  printf("\n");

  if ((ret = ioctl(fd, MIMO_IOCPM, pmdata)) < 0) {
    perror("ioctl_pm");
  }

  printf("===== After IOCTL: =====\n");
  for (i = 0; i < pmdata->count; i++) {
    printf("%lx ", pmdata->ent[i]);
  }
  printf("\n\n");

  printf("===== 2nd IOCTL read: =====\n");
  for (i = 0; i < pmdata->count; i++) {
    pmdata->ent[i] = PM_READ;
    printf("%lx ", pmdata->ent[i]);
  }
  printf("\n");

  if ((ret = ioctl(fd, MIMO_IOCPM, pmdata)) < 0) {
    perror("ioctl_pm");
  }

  printf("===== After IOCTL: =====\n");
  for (i = 0; i < pmdata->count; i++) {
    printf("%lx ", pmdata->ent[i]);
  }
  printf("\n");


  close(fd);
  return 0;
}
