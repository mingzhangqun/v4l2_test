#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>		/* getopt_long() */
#include <fcntl.h>		/* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>


#define DEFAULT_SUFFIX		"raw"

int main(int argc, char *argv[])
{
	int ret = 0;
	int wt = 3864, ht = 2192;
	int bpl = 4832; // bytes per line
	int cnt = 3864*10/8/5;

	unsigned char *buf1, *p1;//[4832];
	unsigned short *buf2, *p2;//[3864];

	int i = 0, j = 0;
	char *ptr = NULL;
	char str[128];

	if (argc <= 1) {
		printf("argc=%d\n", argc);
		return -1;
	}

	strcpy(str, argv[1]);

	int fdr = open(str, O_RDONLY, 00700);
	if (-1 == fdr) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n",
			str, errno, strerror(errno));
		goto err;
	}

	ptr = strrchr(str, '.');
	if (ptr && (ptr != str)) {
		*ptr = '\0';
	}
	sprintf(str, "%s.%s", str, DEFAULT_SUFFIX);
	int fdw = open(str, O_WRONLY|O_CREAT, 00700);
	if (-1 == fdr) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n",
			str, errno, strerror(errno));
		goto err;
	}

	buf1 = (unsigned char*)malloc(bpl);
	if (!buf1) {
		printf("malloc buf1 failed!\n");
		ret = -1;
		goto err;
	}
	buf2 = (unsigned short *)malloc(wt*sizeof(short));
	if (!buf2) {
		printf("malloc buf2 failed!\n");
		ret = -1;
		goto err;
	}

	p1 = buf1;
	p2 = buf2;

	for (j=0; j<ht; j++) {
		read(fdr, buf1, bpl);
		p1 = buf1;
		p2 = buf2;
		for (i=0; i<cnt; i++) {
			p2[0] = p1[0];
			p2[0] <<= 2;
			p2[0] += (p1[1]>>6)&0x03;

			p2[1] = p1[1]&0x3F;
			p2[1] <<= 4;
			p2[1] += (p1[2]>>4)&0x0F;

			p2[2] = p1[2]&0x0F;
			p2[2] <<= 6;
			p2[2] += (p1[3]>>2)&0x3F;

			p2[3] = p1[3]&0x03;
			p2[3] <<= 8;
			p2[3] += p1[4];

			write(fdw, p2, 8);
			p1 +=5;
			p2 += 4;
		}
	}

err:
	if (buf1)
		free(buf1);
	if (buf2)
		free(buf2);

	if (-1 != fdr){
		close(fdr);
	}
	if (-1 != fdw){
		close(fdw);
	}

	return ret;
}