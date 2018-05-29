#include <stdio.h>
#include <stdlib.h>
#include <shadow.h>

int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("./a.out name\n");
		exit(1);
	}
	
	struct spwd *pwd;

	if ((pwd = getspnam(argv[1])) == NULL) {
		printf("getspnam fail\n");
		exit(1);
	}
	printf("sp_namp = %s\n"
		   "sp_pwdp = %s\n"
		   "sp_lstchg = %d\n",
		   pwd->sp_namp,
		   pwd->sp_pwdp,
		   pwd->sp_lstchg);
	
	return 0;	
}