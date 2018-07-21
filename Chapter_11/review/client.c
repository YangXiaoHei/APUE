#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, const char * argv[]) {
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(1);
    }
    
    struct sockaddr_in addr;
    addr.sin_port = htons(8000);
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, "127.0.0.1", (struct sockaddr *)&addr.sin_addr) < 0) {
        perror("inet_pton ");
        exit(1);
    }
    
    int ret;
    if ((ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
        perror("connect error");
        exit(1);
    }
    printf("connect succ\n");

    char buf[1024];
    int n = 0;
    if (( n = read(sockfd, buf, sizeof(buf))) < 0) {
        perror("read from server\n");
        exit(1);
    }

    printf("receive from server : %s\n", buf);

    if ((ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
        perror("connect error");
        exit(1);
    }
    printf("connect succ\n");
    
    return 0;
}