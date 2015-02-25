#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#define EUDYPTULA_HIGH 0xf8cd
#define EUDYPTULA_LOW 0xc7cef553

void
check_eudyptula(int high_id, int low_id, int valid)
{
	int ret, expected_ret;

	/*
	 * We use system call number __NR_eudyptula == 323 as it's the next
	 * available system call number on the x86_64 architecture which isn't
	 * an x32 specific call (those start at 512.)
	 */

	/* Valid result returns 0, otherwise -1, normalise to false, true. */
	ret = !!syscall(__NR_eudyptula, high_id, low_id);
	expected_ret = !valid;

	if (ret != expected_ret) {
		fprintf(stderr,
			"Unexpected result '%s' for high_id, low_id = %x, %x\n",
			strerror(errno), high_id, low_id);

		exit(EXIT_FAILURE);
	}
}

int
main(void)
{
	/* Expectation here is no output at all. */

	check_eudyptula(1, 2, false);
	check_eudyptula(EUDYPTULA_LOW, EUDYPTULA_HIGH, false);
	check_eudyptula(-EUDYPTULA_HIGH, -EUDYPTULA_LOW, false);
	check_eudyptula(-EUDYPTULA_HIGH, EUDYPTULA_LOW, false);
	check_eudyptula(EUDYPTULA_HIGH, -EUDYPTULA_LOW, false);

	check_eudyptula(EUDYPTULA_HIGH, EUDYPTULA_LOW, true);

	return EXIT_SUCCESS;
}
