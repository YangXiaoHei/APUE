#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef void (*sig_handler_info)(int, siginfo_t *, void *);
typedef void (*sig_handler)(int);

sig_handler signal_info(int signo, sig_handler_info handler) {
	struct sigaction old_act, new_act;
	bzero(&old_act, sizeof(old_act));
	bzero(&new_act, sizeof(new_act));
	new_act.sa_flags |= SA_SIGINFO;	   /* 调用第二种函数指针 */
	new_act.sa_flags |= SA_NOCLDSTOP;  /* 子进程停止时，不产生此信号，终止时，仍旧产生此信号 */
	new_act.sa_sigaction = handler;
	sigemptyset(&new_act.sa_mask);
	if (sigaction(signo, &new_act, &old_act) != 0) 
		return SIG_ERR;
	return old_act.sa_handler;
}

#ifndef SIG_CODE_STR
#define SIG_CODE_STR(_code_macro_) (#_code_macro_)
#else
#error "SIG_CODE_STR already defined ?!"
#endif

const char *sig_code_str(int si_code) {
	const char *str = NULL;
#ifdef CLD_EXITED
	if (si_code == CLD_EXITED) 
		str = SIG_CODE_STR(CLD_EXITED);
#endif

#ifdef CLD_KILLED
	if (si_code == CLD_KILLED) 
		str = SIG_CODE_STR(CLD_KILLED);
#endif

#ifdef CLD_DUMPED
	if (si_code == CLD_DUMPED) 
		str = SIG_CODE_STR(CLD_DUMPED);
#endif

#ifdef CLD_TRAPPED
	if (si_code == CLD_TRAPPED) 
		str = SIG_CODE_STR(CLD_TRAPPED);
#endif

#ifdef CLD_STOPPED
	if (si_code == CLD_STOPPED) 
		str = SIG_CODE_STR(CLD_STOPPED);
#endif

#ifdef CLD_CONTINUED
	if (si_code == CLD_CONTINUED) 
		str = SIG_CODE_STR(CLD_CONTINUED);
#endif
	return str;
}


void handle_sig_chld(int signo, siginfo_t *info, void *arg) {
	char buf[100] = { 0 };
	if (info->si_code != CLD_EXITED &&
		info->si_code != CLD_TRAPPED &&
		info->si_code != CLD_STOPPED &&
		info->si_code != CLD_CONTINUED) {
		const char *str = sig_code_str(info->si_code);
		snprintf(buf, 
				 sizeof(buf), 
				 "pid %ld abnormal exit by signo %d\n",
		   		 (long)info->si_pid,
		   		 info->si_status);
	} else {
		snprintf(buf, 
				 sizeof(buf), 
				 "pid %ld normal exit with code : %d\n",
				 (long)info->si_pid,
				 info->si_status);
	}
	printf("%s", buf);
}

int main(int argc, char *argv[]) {

	sigset_t chld_sig_mask, old_mask;
	sigemptyset(&chld_sig_mask);
	sigaddset(&chld_sig_mask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &chld_sig_mask, &old_mask); /* 阻塞 SIGCHLD */

	/* 父进程捕获子进程的退出信号 */
	if (signal_info(SIGCHLD, handle_sig_chld) == SIG_ERR) {
		perror("signal_info error");
		exit(1);
	}
	
	pid_t pid;
	int i = 3;
	int status;
	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(1);
	} else if (pid == 0)
		exit(7);

	sigsuspend(&old_mask);
	if (waitpid(pid, &status, 0) < 0) {
		perror("waitpid error");
		exit(1);
	}
	printf("waitpid succ\n");

	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(1);
	} else if (pid == 0) 
		abort();

	sigsuspend(&old_mask);
	if (waitpid(pid, &status, 0) < 0) {
		perror("waitpid error");
		exit(1);
	}
	printf("waitpid succ\n");

	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(1);
	} else if (pid == 0)
		i /= 0;

	sigsuspend(&old_mask);
	if (waitpid(pid, &status, 0) < 0) {
		perror("waitpid error");
		exit(1);
	}
	printf("waitpid succ\n");
	
	return 0;	
}