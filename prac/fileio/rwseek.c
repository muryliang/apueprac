#include <apue.h>
#include <fcntl.h>
#include <errno.h>

int log_to_stderr = 1;

int main(int ac, char *av[])
{
	int fd, n, wcnt;
	char buf[BUFSIZ];

	if (ac == 1) {
		err_quit("we must have one param");
	}

	log_msg("open %s", av[1]);
	if ((fd = open(av[1], O_RDONLY)) == -1)
			err_sys("can not open %s", av[1]);

	while((n = read(fd, buf, BUFSIZ)) > 0) {
		if ((wcnt=write(STDOUT_FILENO, buf, n)) != n)
				err_exit(errno, "write error");
	}

	if (n < 0)
		err_cont(errno, "have error when read");
	close(fd);
	return 0;
}
