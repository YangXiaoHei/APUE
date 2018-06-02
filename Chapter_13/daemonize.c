

void daemonize(const char *proc_name) {

    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction new_act;

    umask(0);

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        perror("can't get file limit");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid > 0)
        exit(0);

    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &new_act, NULL) < 0) {
        perror("can't ignore SIGHUP");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid > 0) 
        exit(0);

    if (chdir("/") < 0) {
        perror("can't change directory to /");
        exit(1);
    }

    if (rl.rlimit == RLIM_INFINITY) 
        rl.rlim_max = 1024;
    
    for (int i = 0; i < rl.rlim_max; i++) 
        close(i);
    
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    openlog(proc_name, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    }
}



int main(int argc, char *argv[]) {
    
    
    
    return 0;    
}