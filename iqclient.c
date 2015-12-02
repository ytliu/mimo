#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/ioctl.h>

#include "mimo.h"

int main(int argc, char **argv) {
  int fd;
  long ret;
  struct iq_st *iqdata;

  if (argc < 2) {
    printf("Usage: ./iqclient [type] [arg]\n");
    return -1;
  }

  iqdata = (struct iq_st *)malloc(sizeof(struct iq_st));
  iqdata->type = (unsigned int) atoi(argv[1]);
  iqdata->in = (void *) malloc(sizeof(unsigned long));
  
  *(unsigned long *)iqdata->in = (unsigned long) strtol(argv[2], NULL, 16);
  iqdata->out = (void *) malloc(sizeof(unsigned long));

  if ((fd = open("/dev/mimo", O_RDONLY)) < 0) {
    perror("open");
    return -1;
  }

  iqdata->type = IQ_INFO_RDMSR;

  if ((ret = ioctl(fd, MIMO_IOCIQ, iqdata)) < 0) {
    perror("ioctl_iq");
  }
  
  ret = *(unsigned long *)iqdata->out;
  ret = (ret & (1 << 14)) >> 14;

  printf("IOCIQ out bit 14 is: %lx\n", ret);


  close(fd);
  return 0;
}
