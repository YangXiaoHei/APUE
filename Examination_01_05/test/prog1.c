#include "exam_prog1.c"

int main(int argc, char *argv[]) {

    char buf[10 << 10] = { 0 };
    setvbuf(stdout, buf, _IOFBF, sizeof(buf));
    printf(" ShenZheng, ");
    printf("Nice to meet you\n");
    
    LOG("Good morning, ");
    LOG("I'm %s, and I live in", "Hanson");
    char *ptr = NULL;
    LOG(ptr);

    fflush(stdout);
    _exit(0);   
}