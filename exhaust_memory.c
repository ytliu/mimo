#include <stdlib.h>
#include <string.h>

int main()
{
  while (1) {
    void *m = malloc(1024*1024);
    memset(m,1,1024*1024);
  }
  return 0;
}
