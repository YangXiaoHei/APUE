
int main(int argc, char *argv[]) {
    
    setbuf(stdout, NULL);
    
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    
    char a[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char o[] = "0123456789";
    
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        
        for (int i = 0; i < sizeof(a) - 1; ++i) {
            write(fd, a + i, 1);
        }
        
    } else {
        
        for (int i = 0; i < sizeof(b) - 1; ++i) {
            write(fd, a + i, 1);
        }
        
    }
    
    return 0;
}
