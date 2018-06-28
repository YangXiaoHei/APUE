  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <string.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <sys/wait.h>
  #include <errno.h>
  #include <signal.h>
  #include <limits.h>
  #include <pthread.h>
  #include <semaphore.h>
  #include <sys/mman.h>
  #if defined(__gnu_linux__)
  #include <mqueue.h>
  #endif

  int main() {
     char str[] = "hello world";
     char buf[12] = { 0 };
     setvbuf(stdout, buf, _IOFBF, sizeof(buf));
     printf("%s", str);
     return 0;
  }