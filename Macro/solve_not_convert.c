
#define PATH "/usr/local/bin"

#define _YH_STR(_x_) # _x_
#define YH_STR(_x_) _YH_STR(_x_)

int main(int argc, char *argv[]) {

    _YH_STR(PATH);
    YH_STR(PATH);
    
    return 0;    
}