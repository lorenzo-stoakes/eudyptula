#include <errno.h>
#include <linux/msdos_fs.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

void print_err(int errnum)
{
	fprintf(stderr, "fatnametest: %s\n", strerror(errnum));
}

int label_char_valid(char chr)
{
	return chr == ' ' || ('A' <= chr && chr <= 'Z');
}

int label_valid(char *label)
{
	int i, length = strlen(label);

	if (length > 11) {
		fputs("fatnametest: label too long, max. 11 chars\n", stderr);

		return false;
	}

	for (i = 0; i < length; i++)
		if (!label_char_valid(label[i])) {
			fputs("fatnametest: invalid char in label, expect "
				"whitespace or [A-Z]\n", stderr);

			return false;
		}

	return true;
}

int main(int argc, char **argv)
{
	int fd, preverr;

	char *filename, *label;

	if (argc < 3) {
		fprintf(stderr, "usage: %s [FAT-mounted file] [label]\n", argv[0]);
		return EXIT_FAILURE;
	}

	filename = argv[1];
	label = argv[2];

	if (!label_valid(label))
		return EXIT_FAILURE;

	/*
	 * Need an open fd to use ioctl, however we don't want to cause issues
	 * by having an open file so set O_NONBLOCK flag.
	 *
	 * See http://man7.org/linux/man-pages/man2/ioctl.2.html#NOTES
	 */
	fd = open(filename, O_RDONLY | O_NONBLOCK);
	if (fd == EOF)
		goto error;

	if (ioctl(fd, FAT_IOCTL_SET_VOLUME_LABEL, label) == EOF)
		goto close_error;

	if (close(fd) == EOF)
		goto error;

	return EXIT_SUCCESS;
close_error:
	/* If error when closing, print original error + close error. */
	preverr = errno;
	if (close(fd) == EOF)
		print_err(preverr);
	else
		errno = preverr;
error:
	print_err(errno);
	return EXIT_FAILURE;
}
