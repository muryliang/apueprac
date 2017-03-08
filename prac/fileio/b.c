#include <apue.h>
#include <mydef.h>

int log_to_stderr = 1;

int main(void) {
	if (write(3, "child", 5) == -1)
		log_ret("can not write fd 3 in child");
	else
		printf("success write 3 fd in child\n");
	if (write(4, "child", 5) == -1)
		log_ret("can not write fd 4 in child");
	else
		printf("success write 4 fd in child\n");
	return 0;
}
