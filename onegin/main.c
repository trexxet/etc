#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>
#include <errno.h>

#include "onegin.h"

#define ONEG_FAIL() {                           \
	perror (oneg_strerr (O -> oneg_errno)); \
	oneg_free (O);                          \
	return errno;                           \
}


static int cmpAlphabet (const void* p1, const void* p2, void* saveptr);

int main (int argc, char** argv) {

	if (argc != 3) {
		fprintf (stderr, "Usage: onegin <source file> <destination file>\n");
		return EXIT_FAILURE;
	}
	
	// Create state
	
	oneg_State* O = oneg_init();
	if (!O) {
		perror ("Onegin initialization");
		return errno;
	}

	// Load source file
	
	if (oneg_loadSource (O, argv[1]) != 0)
		ONEG_FAIL ();
	
	// Split file to lines
	
	if (oneg_splitSource(O) != 0)
		ONEG_FAIL ();

	// Sort and write to file
	
	oneg_sortLines (O, cmpAlphabet);
	if (oneg_writeSorted (O, argv[2]) != 0) 
		ONEG_FAIL ();

	// Finish
	oneg_free (O);
	return EXIT_SUCCESS;
}


static int cmpAlphabet (const void* p1, const void* p2, void* saveptr) {
	assert (p1);
	assert (p2);

	return strcasecmp(*(char**) p1, *(char**) p2);
}

#undef ONEG_FAIL

