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
static int cmpReverseAlphabetic (const void* p1, const void* p2, void* saveptr);
/*==================================================================================*/


#define ONEG_FAIL() {                           \
	perror (oneg_strerr (O -> oneg_errno)); \
	oneg_free (O);                          \
	return errno;                           \
}

int main (int argc, char** argv) {

	if (argc != 4 || (argv[1][0] != 's' && argv[1][0] != 'r')) {
		fprintf (stderr, "Usage: onegin <s/r> <source file> <destination file>\n");
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
	
	if (oneg_loadSource (O, argv[2]) != 0)
		ONEG_FAIL ();

	// Split file to lines

	if (oneg_splitSource(O) != 0)
		ONEG_FAIL ();

	// Sort and write to file

	oneg_sortLines (O, argv[1][0] == 's' ? cmpAlphabetic : cmpReverseAlphabetic);
	if (oneg_writeSorted (O, argv[3]) != 0) 
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
	wchar_t* str1 = ((oneg_String*) p1) -> string;
	wchar_t* str2 = ((oneg_String*) p2) -> string;
	wint_t i1 = 0, i2 = 0;

	while (!iswalpha(str1[i1]) && (str1[i1] != L'\0'))
		i1++;
	while (!iswalpha(str2[i2]) && (str2[i2] != L'\0'))
		i2++;

	return wcscasecmp(&str1[i1], &str2[i2]);
}


/*
 * Comparator for sorting reversed lines in alphabetic order excluding non-alpha characters
 */
static int cmpReverseAlphabetic (const void* p1, const void* p2, void* saveptr) {
	oneg_String* oStr1 = (oneg_String*) p1;
	oneg_String* oStr2 = (oneg_String*) p2;
	wint_t i1 = oStr1 -> length - 1, i2 = oStr2 -> length - 1;
	wchar_t* str1 = oStr1 -> string;
	wchar_t* str2 = oStr2 -> string;

	while (!iswalpha(str1[i1]) && (i1 > 0))
		i1--;
	while (!iswalpha(str2[i2]) && (i2 > 0))
		i2--;
	while ((i1 > 0) && (i2 > 0) && (str1[i1] == str2[i2])) {
		i1--;
		i2--;
	}

	return str1[i1] - str2[i2];
}

