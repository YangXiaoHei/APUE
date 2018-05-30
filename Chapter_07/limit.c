#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>

static void pr_limits(char *name, int resource) {
	struct rlimit limit;
	unsigned long long lim;

	if (getrlimit(resource, &limit) < 0) {
		printf("getrlimit error\n");
		exit(1);
	}

	printf("%-14s ", name);

	if (limit.rlim_cur == RLIM_INFINITY) {
		printf("(infinite) ");
	} else {
		lim = limit.rlim_cur;
		printf("%10lld ", lim);
	}

	if (limit.rlim_max == RLIM_INFINITY) {
		printf("(infinite)");
	} else {
		lim = limit.rlim_max;
		printf("%10lld", lim);
	}
	printf("\n");
}

#define doit(name) pr_limits(#name, name);

int main(int argc, char *argv[]) {
	
#ifdef RLIMIT_AS
	doit(RLIMIT_AS);
#endif

	doit(RLIMIT_CORE);
	doit(RLIMIT_CPU);
	doit(RLIMIT_DATA);
	doit(RLIMIT_FSIZE);

#ifdef RLIMIT_AS
	doit(RLIMIT_AS);
#endif

#ifdef RLIMIT_MEMLOCK
	doit(RLIMIT_MEMLOCK);
#endif

#ifdef RLIMIT_MSGQUEUE
	doit(RLIMIT_MSGQUEUE);
#endif

#ifdef RLIMIT_NICE
	doit(RLIMIT_NICE);
#endif

	doit(RLIMIT_NOFILE);

#ifdef RLIMIT_NPROC
	doit(RLIMIT_NPROC);
#endif

#ifdef RLIMIT_NPTS
	doit(RLIMIT_NPTS);
#endif

#ifdef RLIMIT_RSS
	doit(RLIMIT_RSS);
#endif

#ifdef RLIMIT_SBSIZE
	doit(RLIMIT_SBSIZE);
#endif

#ifdef RLIMIT_SIGPENDING
	doit(RLIMIT_SIGPENDING);
#endif

#ifdef RLIMIT_SWAP
	doit(RLIMIT_SWAP);
#endif

#ifdef RLIMIT_VMEM
	doit(RLIMIT_VMEM);
#endif
	return 0;	
}