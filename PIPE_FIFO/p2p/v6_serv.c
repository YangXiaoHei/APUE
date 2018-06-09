#include "v6_comm.h"

static char *register_user_names[MAX_USER_NUM];
static int register_user_count;
static char *cur_send_user_name[MAX_NAME_LEN + 1]
static int serv_fifo_read_fd;
static msg_data_t *req_msg;

void init_service(void) {
    if (mkfifo(SERV_FIFO, 0644) < 0 && errno != EEXIST) {
        printf("服务器启动失败! %s\n", strerror(errno));
        exit(1);
    }
    if ((serv_fifo_read_fd = open(SERV_FIFO, O_RDONLY | O_NONBLOCK)) < 0) {
        printf("服务器启动失败! %s\n", strerror(errno));
        exit(1);
    }
    printf("服务器启动成功!\n");
}

ssize_t recv_req() {

     /* 先接收一个包头 */
    char msg_header[MSG_HEADER_SIZE];
    if (read(serv_fifo_read_fd, msg_header, sizeof(msg_header)) < 0) {
        printf("接收包头错误 : %s\n", strerror(errno));
        return FAIL;
    }

    /* 解析包头信息 */
    msg_data_t *msg_ptr = (msg_data_t *)msg_header;
    unsigned long total_len = msg_ptr->total_len;
    unsigned long data_len = msg_ptr->data_len;
    char src_name[MAX_NAME_LEN + 1];
    char dst_name[MAX_NAME_LEN + 1];
    memcopy(src_name, msg_ptr->src_name, sizeof(src_name));
    memcopy(dst_name, msg_ptr->dst_name, sizeof(dst_name));

    printf("接收到包：from %s to %s total_len %ld data_len %ld\n", 
        src_name, dst_name, total_len, data_len);

    for (init; condition; iterations) {
        content
    }

    char data_buf[MAX_BUF_SIZE];
    unsigned long real_nbytes;
    unsigned long ntoread = data_len;
    unsigned long nread = 0, cur_read = 0;
    unsigned long iterations = 0;
    while (nread < ntoread) {
        cur_read = read(client_fifo_read_fd, data_buf + nread, ntoread);
        if (cur_read < 0) {
            if (errno != EINTR) {
                printf("包内容读取错误 : %s\n", strerror(errno));
                exit(1);
            } 
            cur_read = 0;
        }
        nread += cur_read;
        printf("第 %ld 读到：%ld 字节，剩余 %ld 字节\n", 
            iterations++, cur_read, ntoread - nread);
    }
    return SUCCESS;
}

int parse_req()

int main(int argc, char *argv[]) {
    init_service();
    char req_buf[MSG_MAX_LEN];
    while (readn(serv_fifo_read_fd, req_buf, sizeof(req_buf)) > 0) {
        
    }


    return 0;    
}