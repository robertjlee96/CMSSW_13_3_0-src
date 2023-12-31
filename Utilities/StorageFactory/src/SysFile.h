#ifndef STORAGE_FACTORY_SYS_FILE_H
#define STORAGE_FACTORY_SYS_FILE_H

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utime.h>
#include <climits>
#include <cerrno>
#include <cstdlib>
#include <sys/types.h>
#include <sys/uio.h>

#if !defined O_SYNC && defined O_SYNCIO
#define O_SYNC O_SYNCIO
#endif

#if !defined O_NONBLOCK && defined O_NDELAY
#define O_NONBLOCK O_NDELAY
#endif

#ifndef O_NONBLOCK
#define O_NONBLOCK 0
#endif

#endif  // STORAGE_FACTORY_SYS_FILE_H
