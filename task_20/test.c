#include <fcntl.h>
#include <linux/msdos_fs.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int fd;
	int ret;

	if (argc != 3) {
		printf("Usage: %s FILENAME LABEL\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	fd = open(argv[1], 0);
	if (fd == -1) {
		perror("open\n");
		exit(EXIT_FAILURE);
	}
	ret = ioctl(fd, FAT_IOCTL_SET_VOLUME_LABEL, argv[2]);
	if (ret == -1) {
		perror("ioctl\n");
		exit(EXIT_FAILURE);
	}
	printf("Volume label changed\n");
	close(fd);
	exit(EXIT_SUCCESS);
}
