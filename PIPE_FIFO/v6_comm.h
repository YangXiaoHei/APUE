#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>

#define SERV_FIFO "/tmp/v6_serv.fifo"
#define MAX_BUF_SIZE 8192
#define MAX_NAME_LEN 64
#define MAX_USER_NUM 10

#define MSG_HEADER_SIZE     \
(sizeof(long) + 2 * (MAX_NAME_LEN + 1 + sizeof(long)) + sizeof(long))

#define MSG_TOTAL_LEN(_msg_) \
(MSG_HEADER_SIZE + _msg_->data_len)

#define MSG_MAX_LEN() sizeof(msg_data_t)

#define SUCCESS 0
#define FAIL 1

typedef struct msg_data_t {
    /*
     * 数据包总长度
     */
    unsigned long total_len;
    /*
     * 发起对话的目标用户
     */
    char dst_user_name[MAX_NAME_LEN + 1];
    unsigned long dst_user_name_len;
    /*
     * 发起对话的用户
     */
    char src_user_name[MAX_NAME_LEN + 1];
    unsigned long dst_user_name_len;
    /*
     * 对话内容长度
     */
    unsigned long data_len;
    /*
     * 对话内容
     */
    unsigned char data[MAX_BUF_SIZE];
} msg_data_t;


