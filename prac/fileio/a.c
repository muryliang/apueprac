#include <apue.h>
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
	log_msg("the dirname is %s", dirname);
	
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

void read_and_trunk(int ac, char *av[]) {
	int fd;
	char *str = "hello world";
	char buf[BUFSIZ];

	if (ac != 2) 
		log_sys("need a file name");
	if ((fd = open(av[1], O_WRONLY|O_CREAT, 0660)) == -1)
		log_sys("can not open %s", av[1]);
	if (write(fd, str, strlen(str)) != strlen(str))
		log_msg("write not enough");
	close(fd);

	if ((fd = open(av[1], O_RDONLY)) == -1)
		log_sys("can not open %s", av[1]);
	if (read(fd, buf, BUFSIZ) != strlen(str))
		log_msg("read error");

	printf("string: %s\n", buf);
	close(fd);

	if ((fd = open(av[1], O_RDWR|O_TRUNC)) == -1) 
		log_msg("truncate error");
	memset(buf, 0, sizeof(buf));
	if (read(fd, buf, sizeof(buf)) != 0)
		log_msg("unknown msg: %s", buf);
	close(fd);
}

void namemax(char *path) {
	int fd;
	long res ;
//	char namebuf[BUFSIZ];
//	memset(namebuf, 'a',257);
//	namebuf[254] = '\0';
	errno = 0;
//	res = pathconf(path, _PC_NAME_MAX);
	res = pathconf(path, _PC_NO_TRUNC);
	if (res == -1) {
		if (errno == 0) {
			printf("no defined namemax\n");
			return;
		}
		else
			err_sys("get pathconf error");
	}
	printf("got namemax %lx\n", res);
#ifdef _POSIX_NAME_MAX
	printf("we have defined posix name max %d\n", _POSIX_NAME_MAX);
#endif
#ifdef _POSIX_NO_TRUNC
	printf("we have defined posix no trunc %d\n", _POSIX_NO_TRUNC);
#endif
//	printf("now check too long name\nname: %s\n", namebuf);
//	if ((fd = open(namebuf, O_RDWR|O_CREAT, 0660)) == -1)
//		log_sys("error open file ");
	return;
	close(fd);
}

static void getsysconf(int ac, char *av[]) {
	long value;
	struct rlimit rlp;
	if (ac < 2) 
		log_sys("conf name needed");

	memset(&rlp, 0, sizeof(rlp));
	value = sysconf(_SC_OPEN_MAX);
	printf("the value of %s is %ld\n", av[1], value);
	if (-1 == getrlimit(RLIMIT_NOFILE, &rlp))
			printf("error getlimit\n");
	printf("nofile of rlimit is %lu %lu\n", rlp.rlim_cur, rlp.rlim_max);
  #ifdef _FILE_OFFSET_BITS
	   printf("_FILE_OFFSET_BITS defined: %d\n", _FILE_OFFSET_BITS);
  #else
	   printf("nothing\n");
  #endif
  #ifdef __USE_FILE_OFFSET64
	   printf("__USE_FILE_OFFSET64 defined: %d\n", __USE_FILE_OFFSET64);
  #else
	   printf("nothing2\n");
  #endif

	return;
}

static int openfile(char *name, int flags, mode_t mode) {
	int fd;
	if (name == NULL)
		log_sys("not valid file name specified in %s", __FUNCTION__);
	if ((fd = open(name, flags, mode)) == -1)
		err_sys("can not open file %s", name);
	return fd;
}

static void closefile(int fd) {
	int ret = close(fd);
	if (ret < 0)
		log_ret("error close fd %d", fd);
}

void tryseek(int ac, char *av[]) {
	int fd;

	if (ac != 3)
		log_quit("should have a filename and a size");

	fd = openfile(av[1], O_CREAT|O_RDWR|O_TRUNC, 0660);
	lseek(fd, strtol(av[2],NULL, 10), SEEK_SET);
	write(fd, "stub", 4);
	printf("extend success file: %s\n", av[1]);
	closefile(fd);
}

int main(int ac, char *av[])
{

	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("siganl install error");
//	getsysconf(ac, av);
//	namemax(av[1]);
//	tryseek(ac, av);
	printf("size of size_t and ssize_t is %lu %lu\n", sizeof(size_t), sizeof(ssize_t));
	printf("SIZE_MAX %lu\n", SIZE_MAX);
	return 0;
}

