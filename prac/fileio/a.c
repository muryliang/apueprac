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
	assert (ac >= 2);
	fd = openfile(av[1], O_APPEND|O_WRONLY, 0660);
	if (-1 == lseek(fd, 4, SEEK_SET))
		log_sys("error seek");
	if (-1 == write(fd, "first string", 12))
		log_sys("write error");
	if (-1 == lseek(fd, 4, SEEK_SET))
		log_sys("error seek2");
	if (-1 ==read(fd, buf, 3))
		log_sys("can not read");
	buf[strlen(buf)] = '\0';
	printf("buf is :%s:\n", buf);
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

void do_rw_test(char *from, char *to, int flag, int op){
	int fdf, fdt, n;
	char buf[BUFFSIZE];
#ifdef _POSIX_SYNCHRONIZED_IO
	printf("_POSIX_SYNCHRONIZED_IO isdefined so fdatasync == fsync\n");
#endif
	fdf = openfile(from, O_RDONLY, 0660);
	if (to == NULL)
		to = "/dev/null";
	fdt = openfile(to, O_CREAT|O_WRONLY|flag, 0660);

	errno = 0;
	while (0 != (n = read(fdf, buf, BUFFSIZE))) {
//		printf("read ");
		if (n != write(fdt, buf, n))
			log_sys("write %d to file %s failed", fdt, to);
//		printf("write\n");
	}
	if (errno != 0)
		log_sys("some error occur during reading");
	if (op == FDATASYNC)
		fdatasync(fdt);
	else if (op == FSYNC)
		fsync(fdt);
	printf("success\n");
	return;
}


void writetest(int ac, char *av[]) {
	char *file1, *file2;
	assert(ac >=3 && strlen(av[1]) >= 2);
	switch(av[1][1]) {
		case 'n':  //write to null
			file1 = av[2];
			do_rw_test(file1, NULL, 0, 0);
			break;
		case 'f':  //rw from file normal
			file1 = av[2];
			file2 = av[3];
			do_rw_test(file1, file2, 0, 0);
			break;
		case 's': //from file and open with sync
			file1 = av[2];
			file2 = av[3];
			do_rw_test(file1, file2, O_SYNC, 0);
			break;
		case 'd': //use fdadasync afterwrite
			file1 = av[2];
			file2 = av[3];
			do_rw_test(file1, file2, 0, FDATASYNC);
			break;
		case 'y':
			file1 = av[2];
			file2 = av[3];
			do_rw_test(file1, file2, 0, FSYNC);
			break;
		case 'c':
			file1 = av[2];
			file2 = av[3];
			do_rw_test(file1, file2, O_SYNC, FDATASYNC);
			break;
		default:
			log_quit("not know your option");
			break;
	}
}

/*
 * use close to close and judgement error can only be EBADF
 *		open first, if fail, then no need to close
 * use open creat|excl to open exclusive, so will fail if already open
 *			until return the fd you want, record opened fd during open try
 *	then close already opened not needed fd
 */
void mydup2(int from, int to) {
	int max = sysconf(_SC_OPEN_MAX);
	int *stack, tmpfd, i = 0, j;
	stack = (int*)malloc(sizeof(int)*max);
	if (stack == NULL) log_sys("can not malloc");
	if (from == to) return; //same 
	if (to >= max)  log_quit("overflow fd max than %d", max-1);
	while ((tmpfd = dup(from)) < to)
		stack[i++] = tmpfd;
	close(to);
	if (-1 == dup(from)) log_sys("can not dup fd %d", from);
	getchar();
	for (j = 0; j < i; j++)
		close(stack[j]);
}
	
void testdup(int ac, char *av[]) {
	int fd, fd2;
	assert(ac >=3);

	fd = openfile(av[1], O_CREAT|O_TRUNC|O_RDWR, 0660);
	fd2 = strtol(av[2], NULL, 10);
	mydup2(fd, fd2);
	if (-1 == write(fd, "this is fd1", 11))
		log_quit("error write fd1 %d", fd);
	if (-1 == write(fd2, "this is fd2", 11))
		log_quit("error write fd2 %d", fd2);
	return;
}

void print_time(char *str, struct timespec time) {
	printf(str);
	printf(" sec %ld, nsec %ld\n", time.tv_sec, time.tv_nsec);
}

void parse_mode(int mode) {
	switch(mode & S_IFMT) {
		case S_IFSOCK: printf("socket ");break;
		case S_IFLNK: printf("symbolic link ");break;
		case S_IFREG: printf("regular file ");break;
		case S_IFBLK: printf("blk dev ");break;
		case S_IFDIR: printf("dir ");break;
		case S_IFCHR: printf("char ");break;
		case S_IFIFO: printf("pipe ");break;
		default: printf("unknown-file-type "); break;
	}
	if (mode & S_ISUID) printf("suid ");
	if (mode & S_ISGID) printf("sgid ");
	if (mode & S_ISVTX) printf("svtx ");
	if (mode & S_IRUSR) printf("read_usr ");
	if (mode & S_IWUSR) printf("write_usr ");
	if (mode & S_IXUSR) printf("exec_usr ");
	if (mode & S_IRGRP) printf("read_grp ");
	if (mode & S_IWGRP) printf("write_grp ");
	if (mode & S_IXGRP) printf("exec_grp ");
	if (mode & S_IROTH) printf("read_oth ");
	if (mode & S_IWOTH) printf("write_oth ");
	if (mode & S_IXOTH) printf("exec_oth ");
	putchar('\n');
}


void print_stat(const struct stat *state) {

	parse_mode((int)state->st_mode);
	printf("dev_t %lx   ino_t %lu   mode_t %x\n", (unsigned long)state->st_dev, (unsigned long)state->st_ino, (int)state->st_mode);
	printf("nlink_t %lu   uid_t %lu   gid_t %lu\n", (unsigned long)state->st_nlink, (unsigned long)state->st_uid, (unsigned long)state->st_gid);
	printf("rdev_t %lx   off_t %ld   blksize_t %ld\n", (unsigned long)state->st_rdev, (long)state->st_size, (long)state->st_blksize);
	printf("blkcnt_t %ld\n", (unsigned long)state->st_blocks);
	print_time("atime", state->st_atim);
	print_time("mtime", state->st_mtim);
	print_time("ctime", state->st_ctim);
}

void trystat(int ac, char *av[]) {
	int fd;
	struct stat state;

	assert(ac >= 2);
/*	printf("open with fd\n");
	fd = openfile(av[1], O_CREAT|O_RDWR, 0660);
	if (-1 == fstat(fd, &state))
		log_quit("err when get file state");
*/
/*
	printf("open with filename\n");
	if (-1 == stat(av[1], &state))
		log_quit("error when get file state");
*/
	printf("open with fstatat\n");
	if (-1 == fstatat(AT_FDCWD, av[1], &state, AT_SYMLINK_NOFOLLOW))
		log_quit("error when get file state");

	print_stat(&state);
}

int main(int ac, char *av[])
{
	trystat(ac, av);
	return 0;
}

