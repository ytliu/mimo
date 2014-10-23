#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/ioctl.h>

#include "aqmo.h"

int main(int argc, char **argv) {
  int pid, fd;
  long ret;

  if (argc < 2) {
    return -1;
  }

  pid = atoi(argv[1]);
  if ((fd = open("/dev/aqmo", O_RDONLY)) < 0) {
    perror("open");
    return -1;
  }

  if ((ret = ioctl(fd, AQMO_IOCQUERY, (unsigned long) pid)) < 0) {
    perror("ioctl_query");
  }

  close(fd);
  return 0;
}
