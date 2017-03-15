#ifndef SELFDEF_H
#define SELFDEF_H

#include <assert.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <stdint.h>
#include <ftw.h>
#define DTPATH "/home/sora/git/apue/data"
#define BUFFSIZE 4096
#define FDATASYNC 1
#define FSYNC    2
#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

#endif 
