#include <sys/stat.h>

int _close_r( int fd )
{
    (void) fd;
    return -1;
}

int _read_r( int fd, char* ptr, int len )
{
    (void) fd, (void) ptr, (void) len;
    return -1;
}

#ifndef USE_SEGGER_RTT
int _write_r( int fd, char* ptr, int len )
{
    (void) fd, (void) ptr, (void) len;
    return -1;
}
#endif

int _lseek_r( int fd, int ptr, int dir )
{
    (void) fd, (void) ptr, (void) dir;
    return 0;
}

int _fstat(int file, struct stat *st)
{
  (void)file;
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int file)
{
  (void)file;
  return 1;
}
