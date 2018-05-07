#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#import <setjmp.h>

#define TOK_ADD 5

jmp_buf jmpbuffer;

void do_line(char *);
void cmd_add(void);
int get_token(void);

int main() {
    
    char line[4096];
    
    if (setjmp(jmpbuffer) != 0) {
        printf("error");
    }
    
    while (fgets(line, sizeof(line), stdin) != NULL) {
        do_line(line);
    }
    
    return 0;
}

char *tok_ptr;

void do_line(char *ptr) {
    
    int cmd;
    
    tok_ptr = ptr;
    while ((cmd = get_token()) > 0) {
        switch (cmd) {
            case TOK_ADD:
                cmd_add();
                break;
        }
    }
}

void cmd_add(void) {
    
    int token;
    
    token = get_token();
    if (token < 0) {
        longjmp(jmpbuffer, 1);
    }
}

int get_token() {
    
    
}
