/*
 * =====================================================================================
 *
 *       Filename:  onegin.c
 *
 *    Description:  Text file sorter source file
 *
 *        Version:  1.0
 *        Created:  08/27/2017 07:53:38 PM
 *       Compiler:  gcc
 *
 *         Author:  trexxet
 *
 * =====================================================================================
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <unistd.h>
#include <assert.h>
#include <locale.h>

#include "onegin.h"

#define return_oneg_errno(_err_) return (state -> oneg_errno = _err_)


/*==================================================================================*/


/*
 * Create onegin state
 * Returns pointer on created onegin state
 */
oneg_State* oneg_init () {

	oneg_State* state = (oneg_State*) calloc (1, sizeof (oneg_State));
	if (!state)
		return NULL;

	state -> buffer = NULL;
	state -> bufferSize = 0;
	state -> lines = NULL;
	state -> numOfLines = 0;
	state -> oneg_errno = 0;
	
	if (!setlocale (LC_CTYPE, ""))
		state -> oneg_errno = EONEG_SET_LOCALE;

	return state;
}


/* 
 * Load text from a source file $filename to $state buffer
 * Returns ONEG_ERRNO of $state
 */
int oneg_loadSource (oneg_State* state, const char* fileName) {

	assert (state);
	assert (fileName);

	// Open file

	if (access (fileName, F_OK | R_OK) != 0)
		return_oneg_errno (EONEG_SOURCE_FILE_ACCESS);

	FILE* source = fopen (fileName, "rb"); 
	if (!source)
		return_oneg_errno (EONEG_SOURCE_FILE_OPEN);

	// Get file size

	fseek (source, 0L, SEEK_END);
	state -> bufferSize = ftell (source);
	rewind (source);

	// Load file to buffer

	// Buffer for data from source file
	char* sbuffer = (char*) calloc (state -> bufferSize + 1, 1);
	if (!sbuffer) {
		fclose (source);
		return_oneg_errno (EONEG_SOURCE_FILE_BUFFER);
	}

	if ((fread (sbuffer, 1, state -> bufferSize, source) < (state -> bufferSize))
	  || ferror (source)) {
		fclose (source);
		return_oneg_errno (EONEG_SOURCE_FILE_READ);
	}

	// Convert source buffer to wchar_t*

	sbuffer[state -> bufferSize] = 0;
	state -> buffer = (wchar_t*) calloc (state -> bufferSize, sizeof(wchar_t));
	if (!state -> buffer) {
		free (sbuffer);
		sbuffer = NULL;
		fclose (source);
		return_oneg_errno (EONEG_SOURCE_FILE_BUFFER);
	}
	mbstowcs (state -> buffer, sbuffer, state -> bufferSize);

	// Finish

	free (sbuffer);
	sbuffer = NULL;
	fclose (source);
	return_oneg_errno (EONEG_NO_ERROR);
}


/* 
 * Split $state buffer into lines
 * Returns ONEG_ERRNO of $state
 */
int oneg_splitSource (oneg_State* state) {

	assert (state);

	// Count lines

	state -> numOfLines = 0;
	for (size_t i = 0; i < state -> bufferSize; i++)
		if (state -> buffer[i] == L'\n')
			state -> numOfLines++;

	// Split source to lines

	state -> lines = (wchar_t**) calloc (state -> numOfLines, sizeof(wchar_t*));
	if (!state -> lines)
		return_oneg_errno (EONEG_SPLITTED_LINES_BUFFER);

	wchar_t* saveptr = NULL;
	wchar_t* line = NULL;
	wchar_t* tokptr = state -> buffer;
	for (size_t j = 0; ; j++, tokptr = NULL)
	{
		line = wcstok (tokptr, L"\n", &saveptr);
		if (line)
			state -> lines[j] = line;
		else
			break;
	}

	return_oneg_errno (EONEG_NO_ERROR);
}


/*
 * Sort $state lines according to a comparsion function pointed by $compar
 * The comparsion function must return an integer less than, equal to or greater than zero
 * if the first argument is considered to be respectively less than, equal to or greater
 * than the second
 */
void oneg_sortLines (oneg_State* state, int (*compar)(const void*, const void*, void*)) {

	assert (state);
	assert (compar);

	void* saveptr = NULL;
	qsort_r (state -> lines, state -> numOfLines, sizeof(wchar_t*), compar, saveptr);
}


/*
 * Write $state buffer according to $state lines pointer to output file $filename
 * Returns ONEG_ERRNO of $state
 */
int oneg_writeSorted (oneg_State* state, const char* fileName) {

	assert (state);
	assert (fileName);
	
	size_t numOfWrittenLines = 0;

	// Open file
	
	FILE* dest = fopen (fileName, "w"); 
	if (!dest)
		return_oneg_errno (EONEG_DEST_FILE_OPEN);

	// Write buffer to file according to sorted lines
	
	for (size_t i = 0; i < state -> numOfLines; i++) {
		if (fwprintf (dest, L"%ls\n", state -> lines[i]) > 0)
			numOfWrittenLines++;
		else
			break;
	}
	fclose (dest);

	if (numOfWrittenLines < state -> numOfLines)
		return_oneg_errno (EONEG_DEST_FILE_WRITE);

	return_oneg_errno (EONEG_NO_ERROR);
}


#undef return_oneg_errno


/* 
 * Free memory used by $state and destroy it
 */
void oneg_free (oneg_State* state) {
	
	assert (state);

	if (state -> buffer) {
		free (state -> buffer);
		state -> buffer = NULL;
	}
	if (state -> lines) {
		free (state -> lines);
		state -> lines = NULL;
	}
	free (state);
	state = NULL;
}

