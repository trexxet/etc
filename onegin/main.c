#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
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

	if (O -> oneg_errno == EONEG_SET_LOCALE)
		fprintf (stderr, "%s\n", oneg_strerr (EONEG_SET_LOCALE));

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
	#define _ptr1 (*(wchar_t**) p1)
	#define _ptr2 (*(wchar_t**) p2)

	return wcscasecmp(_ptr1, _ptr2);

	#undef _ptr1
	#undef _ptr2
}

#undef ONEG_FAIL

