#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <assert.h>
#include <errno.h>

#include "onegin.h"


/*==================================================================================*/
static int cmpAlphabetic (const void* p1, const void* p2, void* saveptr);
/*==================================================================================*/


#define ONEG_FAIL() {                           \
	perror (oneg_strerr (O -> oneg_errno)); \
	oneg_free (O);                          \
	return errno;                           \
}

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
	
	oneg_sortLines (O, cmpAlphabetic);
	if (oneg_writeSorted (O, argv[2]) != 0) 
		ONEG_FAIL ();

	// Finish

	oneg_free (O);
	return EXIT_SUCCESS;
}

#undef ONEG_FAIL


/*
 * Comparator for sorting lines in alphabetic order exluding non-alpha characters
 */
static int cmpAlphabetic (const void* p1, const void* p2, void* saveptr) {
	wchar_t* ptr1 = *(wchar_t**) p1;
	wchar_t* ptr2 = *(wchar_t**) p2;
	wint_t i1 = 0, i2 = 0;

	while (!iswalpha(ptr1[i1]) && (ptr1[i1] != L'\0'))
		i1++;
	while (!iswalpha(ptr2[i2]) && (ptr2[i2] != L'\0'))
		i2++;

	return wcscasecmp(&ptr1[i1], &ptr2[i2]);
}

