#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>

//#include <_ansi.h>
#include <errno.h>
#include "btos_stubs.h"

static char stdout_device[255]={'D', 'E', 'V', ':', '/'};
static bt_filehandle stdout=0;

// --- Process Control ---

int
_exit(int val){
  bt_exit(val);
  return (-1);
}

int
execve(char *name, char **argv, char **env) {
	errno = ENOMEM;
	return bt_spawn(name, 0, NULL);
}

/*
 * getpid -- only one process, so just return 1.
 */
int
getpid()
{
  return bt_getpid();
}


int 
fork(void) {
	errno = ENOTSUP;
	return -1;
}


/*
 * kill -- go out via exit...
 */
int
kill(pid, sig)
     int pid;
     int sig;
{
  if(pid == getpid())
    _exit(sig);


  bt_kill(pid);
  return -1;
}

int
wait(int *status) {
	errno = ECHILD;
	return -1;
}

// --- I/O ---

/*
 * isatty -- returns 1 if connected to a terminal device,
 *           returns 0 if not. Since we're hooked up to a
 *           serial port, we'll say yes and return a 1.
 */
int
isatty(fd)
     int fd;
{
  return (1);
}


int
close(int file) {
	return -1;
}

int
link(char *old, char *new) {
	errno = EMLINK;
	return -1;
}

int
lseek(int file, int ptr, int dir) {
	return 0;
}

int
open(const char *name, int flags, ...) {
	return -1;
}

int
read(int file, char *ptr, int len) {
	// XXX: keyboard support

	return 0;
}

int 
fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

int
stat(const char *file, struct stat *st){
	st->st_mode = S_IFCHR;
	return 0;
}

int
unlink(char *name) {
	errno = ENOENT;
	return -1;
}


int
write(int file, char *ptr, int len) {
	if(file==1){
		if(!stdout){
			if(!bt_getenv("DISPLAY_DEVICE", &stdout_device[5], 250)) return;
			stdout=bt_fopen(stdout_device, 0);
		}
		bt_write(stdout, len, ptr);
	}
	return -1;
}

// --- Memory ---

/* _end is set in the linker command file */
extern caddr_t _end;

#define PAGE_SIZE 4096ULL
#define PAGE_MASK 0xFFFFFFFFFFFFF000ULL
#define HEAP_ADDR (((unsigned long long)&_end + PAGE_SIZE) & PAGE_MASK)

/*
 * sbrk -- changes heap size size. Get nbytes more
 *         RAM. We just increment a pointer in what's
 *         left of memory on the board.
 */
caddr_t
sbrk(int nbytes){
  static climit=0;
  static calloced=0;
  void *cbase=0;
  caddr_t ret;
  if(climit - calloced > nbytes){
	ret=cbase+calloced;
	calloced+=cbase;
  }else{
	unsigned int pages=(nbytes/4096)+1;
	cbase=bt_alloc(pages);
	climit=pages*4096;
	calloced=nbytes;
	ret=cbase;
  }

  return ret;
}


// --- Other ---
 int gettimeofday(struct timeval *p, void *z){
	 return -1;
 }

