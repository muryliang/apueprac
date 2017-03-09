#include <apue.h>
#include <mydef.h>

int log_to_stderr = 1;
static int regular, chr, blk, fifo, lnk, sock, dir, deep, count;
void print_res(void);
void count_file_type(char *start);

int main(int ac, char *av[]) {
	assert(ac >= 2);

	log_ret("start");
	count_file_type(av[1]);
	count = regular + chr + blk + fifo + lnk + sock + dir;
	print_res();
	return 0;
}

void print_res(void) {
	printf("count %d, deep %d\n", count, deep);
	printf("%20s%20s%20s\n", "File Type", "Count", "Percentage");
	printf("%20s%20d%20.2f%%\n", "regular", regular, regular*1.0/count*100);
	printf("%20s%20d%20.2f%%\n", "dir", dir, dir*1.0/count*100);
	printf("%20s%20d%20.2f%%\n", "symbolic", lnk, lnk*1.0/count*100);
	printf("%20s%20d%20.2f%%\n", "chr", chr, chr*1.0/count*100);
	printf("%20s%20d%20.2f%%\n", "blk", blk, blk*1.0/count*100);
	printf("%20s%20d%20.2f%%\n", "sock", sock, sock*1.0/count*100);
	printf("%20s%20d%20.2f%%\n", "fifo", fifo, fifo*1.0/count*100);
}

void count_file_type(char *name) {
	struct stat st;
	char filebuf[BUFSIZ];
	DIR *dirp;
	struct dirent *direntp;

	printf("now deep %d object %s\n", deep, name);
	if (lstat(name, &st) == -1)
		log_ret("can not get stat from %s", name);
	if (S_ISREG(st.st_mode)) regular++;
	else if(S_ISCHR(st.st_mode)) chr++;
	else if(S_ISBLK(st.st_mode)) blk++;
	else if(S_ISFIFO(st.st_mode)) fifo++;
	else if(S_ISLNK(st.st_mode)) lnk++;
	else if(S_ISSOCK(st.st_mode)) sock++;
	else if(S_ISDIR(st.st_mode))  {
		dir++;
//		if (NULL == (dirp = opendir(name)))
//			log_ret("can not opendir %s", name);
		if (NULL == (dirp = opendir(name))) {
			log_ret("can not opendir %s", name);
			return;
		}
		deep++;
		while ((direntp = readdir(dirp)) != NULL) {
			if (!strcmp("..", direntp->d_name) || !strcmp(".", direntp->d_name))
				continue;
			snprintf(filebuf, BUFSIZ, "%s/%s", name, direntp->d_name);
			count_file_type(filebuf);
		}
		closedir(dirp);
		deep--;
	}
	else 
		printf("do not known this mode %x of file %s\n", st.st_mode, name);

}
