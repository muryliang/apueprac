#include <apue.h>
#include <mydef.h>

int log_to_stderr = 1;

int main(void) {
	if (write(4, "child", 5) == -1)
		log_ret("can not write fd 4 in child");
	else
		printf("success write 4 fd in child\n");
	if (write(5, "child", 5) == -1)
		log_ret("can not write fd 5 in child");
	else
		printf("success write 5 fd in child\n");
	return 0;
}
