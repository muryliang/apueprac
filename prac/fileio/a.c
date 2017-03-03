#include <apue.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include <mydef.h>

int log_to_stderr = 1;

static void sig_int(int);

int rw(int ac, char *av[])
{
	int n;
	char buf[BUFSIZ];

	while ((n = read(STDIN_FILENO, buf, BUFSIZ)) > 0)
		if (write(STDOUT_FILENO ,buf, n) != n)
				err_sys("write error");

	if (n < 0)
		err_sys("read error");
	return 0;
}
int listdir(int ac, char *av[])
{
	DIR *dp;
	struct dirent *dirp;
	char *dirname;

	if ( ac != 2)
		err_quit("need a dir name\n");

	dirname = av[1];
	log_msg("the dirname is %s\n", dirname);
	
	if ((dp = opendir(dirname)) == NULL)
		log_quit("can not opendir %s %s\n", dirname, strerror(errno));

	while((dirp = readdir(dp)) != NULL) {
		printf("%s\n", dirp->d_name);
	}

	closedir(dp);
	return 0;
}

int crw(int ac, char *av[])
{
	int c;

	while ((c = getc(stdin)) != EOF)
		if (putc(c, stdout) == EOF)
			err_sys("output error");
	if (ferror(stdin))
		err_sys("input error");
	exit(0);
}

int bare_shell(int ac, char *av[])
{
	char buf[MAXLINE];
	pid_t pid;
	int status;
again:
	printf("%% ");
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		if (buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = 0;

		if ((pid = fork()) < 0)
			err_sys("fork error");
		else if (pid == 0) {
			execlp(buf, buf, (char *)0);
			err_sys("couldn't execute %s", buf);
			exit(127);
		}
		if ((pid = waitpid(pid, &status, 0)) < 0)
			err_sys("waitpid error");
		printf("%% ");
	}
	if (ferror(stdin)) {
		if (errno == EINTR)
			goto again;
		log_sys("stdin error");
	} else
		printf("normal exit\n");
	exit(0);
}

int gid(int ac, char *av[])
{
	printf("uid is %d, git is %d\n", getuid(), getgid());
	return 0;
}

int err(int ac, char *av[])
{
	fprintf(stderr, "EACCESS: %s\n", strerror(EACCES));
	errno = ENOENT;
	perror(av[0]);
	exit(0);
}

static void sig_int(int sigio)
{
	printf("interrupted\n");
}

int main(int ac, char *av[])
{

	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("siganl install error");
	bare_shell(ac, av);
	return 0;
}

