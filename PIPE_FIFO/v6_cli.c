#include "v6_comm.h"


static int serv_fifo_write_fd;
static char user_name[MAX_NAME_LEN + 1];
static char cur_dst_name[MAX_NAME_LEN + 1];
static int client_fifo_read_fd;

int check_serv_support_service(void) {
    return access(SERV_FIFO, F_OK) == 0 ? SUCCESS : FAIL;
}

void login_req(void) {

    /* 打开服务器的写端 */
    if (serv_fifo_write_fd = open(SERV_FIFO, O_WRONLY) < 0) {
        printf("client %s login fail : %s\n", user_name, strerror(errno));
        exit(1);
    }

    /* 把登陆名告诉服务器 */
    ssize_t n = strlen(user_name);
    if (write(serv_fifo_write_fd, user_name, n) != n) {
        printf("client %s login fail : %s\n", user_name, sterror(errno));
        exit(1);
    }

    /* 创建客户端 */
    char fifo_path[MAX_NAME_LEN];
    snprintf(fifo_path, sizeof(fifo_path), "/tmp/%s.fifo", user_name);
    if (mkfifo(fifo_path, 0644) < 0 && errno != EEXIST) {
        printf("client %s create fifo error : %s\n", user_name, strerror(errno));
        exit(1);
    }

    /* 打开客户端读端，并设为非阻塞 */
    if ((client_fifo_read_fd = open(fifo_path, O_RDONLY | O_NONBLOCK)) < 0) {
        printf("client %s open fifo error : %s\n", user_name, strerror(errno));
        exit(1);
    }

    printf("用户 %s 成功登陆\n", user_name);
}

void get_user_name() {
    printf("请输入登录用户名> ");
    if (fgets(user_name, sizeof(user_name), stdin) == NULL) {
        printf("获取登录用户名失败\n");
        exit(1);
    }
} 

void wait_till_service(void) {
     while (1) {
        if (check_serv_support_service() == SUCCESS) break;
        else {
            printf("serv service not avaliable now...\n");
            sleep(1);
        }
    }
}

msg_data_t *create_msg(const char *dst_name,
                       const char *data, 
                       ssize_t data_len) {
    msg_data_t *msg = malloc(sizeof(msg_data_t));
    if (msg == NULL)  return NULL;
    snprintf(msg->src_user_name, MAX_NAME_LEN, "%s", user_name);
    snprintf(msg->dst_user_name, MAX_NAME_LEN, "%s", dst_name);
    msg->src_user_name_len = strlen(user_name);
    msg->dst_user_name_len = strlen(dst_name);
    memcpy(msg->data, data, data_len);
    msg->data_len = data_len;
    msg->total_len = MSG_TOTAL_LEN(msg);
    return msg;
}

int send_msg_data() {
    printf("目的> ");
    char dst[MAX_NAME_LEN + 1];
    if (fgets(dst, sizeof(dst), stdin) == NULL) {
        printf("fgets 获取目的失败\n");
        return FAIL;
    }
    snprintf(cur_dst_name, sizeof(cur_dst_name), "%s", dst);

    printf("内容> ");
    char data[MAX_BUF_SIZE];
    if (fgets(data, sizeof(data), stdin) == NULL) {
        printf("fgets 获取发送内容失败\n");
        return FAIL;
    }

    msg_data_t *msg = create_msg(dst, data, strlen(data));
    if (msg == NULL) {
        printf("创建发送包失败\n");
        return FAIL;
    }

    if (write(serv_fifo_write_fd, msg, msg->total_len) != msg->total_len) {
        printf("client %s send msg data error", user_name);
        exit(1);
    }
    return SUCCESS;
}

int recv_msg_data(void) {

    /* 先接收一个包头 */
    char msg_header[MSG_HEADER_SIZE];
    if (read(client_fifo_read_fd, msg_header, sizeof(msg_header)) < 0) {
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

    /* 校验是否是该包的目的 */
    if (strncmp(user_name, dst_name, strlen(user_name)) != 0) {
        printf("包发送目的错误：当前 %s  目的 %s\n", user_name, dst_name);
        return FAIL;
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

int main(int argc, char *argv[]) {
    wait_till_service();
    get_user_name();
    login_req();
    int op;
    while (1) {
        printf("接收 or 发送 ? (r/s)> ");
        if ((op = getchar()) == EOF ||
            !(op == 'r' || op == 's')) continue;

        if (op == 'r') recv_msg_data();
        if (op == 's') send_msg_data();
    }
    return 0;    
}