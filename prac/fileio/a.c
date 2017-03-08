#include <apue.h>
#include <mydef.h>

int log_to_stderr = 1;

static void sig_int(int);

int rw(int ac, char *av[])
{
	int n, bufsiz;
	char *buf;

	if (ac < 2)
		log_quit("need bufsize");
		
	bufsiz = strtol(av[1], NULL, 10);
	buf = malloc(bufsiz * sizeof(char));
	if (buf == NULL)
		log_sys("can not malloc for buf");

	log_msg("buf size now is %d\n", bufsiz);
	while ((n = read(STDIN_FILENO, buf, bufsiz )) > 0)
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

void tryappend(int ac, char *av[]) {
	int fd, n;
	char buf[BUFSIZ];
	if (ac != 2)
		log_quit("should have a filename");
	fd = openfile(av[1], O_APPEND|O_RDWR, 0660);
	if (-1 == lseek(fd, SEEK_SET, 4))
		log_sys("error seek");
	write(fd, "first string", 12);
	if (-1 == lseek(fd, 4, SEEK_SET))
		log_sys("error seek");
	if (-1 == (n = read(fd, buf, 5)))
		log_quit("read error");
	buf[strlen(buf)] = '\0';
	printf("we have %d count string: %s\n",n, buf);
	pwrite(fd, "hello", 5, 6);
	if (-1 == pread(fd, buf, 5, 6))
		log_msg("can not pread");
	buf[strlen(buf)] = '\0';
	printf("we have %d count pread string: %s\n",n, buf);
	close(fd);
}
		
void tryreopen(int ac, char *av[]) {
	int fd, fd2;
	if (ac < 2)
		log_quit("should have a filename");
	fd = openfile(av[1], O_RDWR|O_CREAT|O_EXCL, 0660);
	printf("success a\n");
	write(fd, "first", 5);
	close(fd);
	fd2 = openfile(av[1], O_RDWR|O_CREAT|O_EXCL, 0660);
	printf("success b\n");
}

void trydup(int ac, char *av[]) {
	int fd1, fd2, fd3=8;

	if (ac < 2)
		log_quit("need a filename");
	fd1 = open(av[1], O_CREAT|O_RDWR, 0660);
	printf("open success\n");
	fd2 = dup(fd1);
	write(fd2, "dup in a file", 13);
	dup2(STDOUT_FILENO, fd3);
	dup2(fd2, STDOUT_FILENO );
	printf("now in a file\n");
	dup2(fd3, STDOUT_FILENO);
	printf("fd1 %d fd2 %d fd3 %d\n", fd1, fd2, fd3);
}

void testfcntl(int ac, char *av[]) {
	int fd1, fd2, res;
	char buf[BUFSIZ];

	if (ac != 2)
		log_quit("need file name");

	fd1 = openfile(av[1], O_CREAT|O_RDWR|O_CLOEXEC, 0660);
//	fd2 = dup(fd1);
	fd2 = fcntl(fd1, F_DUPFD_CLOEXEC, 0);
	printf("in parent, fd1 %d fd2 %d\n", fd1, fd2);

	if (-1 == (res = fork()))
		log_sys("can not fork");
	else if (res == 0) { //child 
		printf("in child");
//		snprintf(buf, 100, "prac/b.exe"
		execl("./fileio/b.exe", "b.exe", (char*)0);
		log_sys("can not reach here");
	} else {
		printf("in paretn\n");
		if (-1 == write(fd1, "hello", 5))
			log_ret("can not parent write fd1");
		else 
			printf("success parent write fd1\n");
		if (-1 == write(fd2, "second", 6))
			log_ret("can not parent write fd2");
		else 
			printf("success parent write fd2\n");
	}
}

void getown(int ac, char *av[]) {
	int fd;
	if (ac < 2)
		log_quit("should have a param");
	fd = openfile(av[1], O_CREAT|O_TRUNC|O_RDWR, 0660);
	printf("the own is %d\n", fcntl(fd, F_GETOWN));
	fcntl(fd, F_SETOWN, getpid());
	printf("the own is %d\n", fcntl(fd, F_GETOWN));
}

void fcntlflag(int ac, char *av[]) {
	int val;
	assert(ac >=2);
	
	val = fcntl(strtol(av[1], NULL, 10), F_GETFL);	
	switch(val & O_ACCMODE) {
	case O_RDONLY: printf("rdonly "); break;
	case O_WRONLY: printf("wronly "); break;
	case O_RDWR:   printf("rdwr ");   break;
	default: break;
	}

	if (val & O_APPEND) printf("append ");
	else if (val & O_NONBLOCK) printf("nonblock ");
	else if (val & O_SYNC) printf("sync ");
	putchar('\n');
	return;
}


int main(int ac, char *av[])
{

	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("siganl install error");
//	getsysconf(ac, av);
//	namemax(av[1]);
//	tryseek(ac, av);
//	printf("size of size_t and ssize_t is %lu %lu\n", sizeof(size_t), sizeof(ssize_t));
//	printf("SIZE_MAX %lu\n", SIZE_MAX);
//	rw(ac, av);
//	tryappend(ac, av);
//	tryreopen(ac, av);
//	trydup(ac, av);
//	testfcntl(ac, av);
//	getown(ac, av);
	fcntlflag(ac, av);
	return 0;
}

