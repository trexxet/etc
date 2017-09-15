/*
 * =====================================================================================
 *
 *       Filename:  onegin.c
 *
 *    Description:  Text lines sorter source file
 *
 *        Version:  1.0
 *        Created:  08/27/2017 07:53:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  trexxet
 *
 * =====================================================================================
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "onegin.h"

#define return_oneg_errno(_err_) return (state -> oneg_errno = _err_)


oneg_State* oneg_init () {

	oneg_State* state = (oneg_State*) calloc (1, sizeof (oneg_State));
	if (!state)
		return NULL;

	state -> buffer = NULL;
	state -> bufferSize = 0;
	state -> lines = NULL;
	state -> numOfLines = 0;
	state -> oneg_errno = 0;
	
	return state;
}


int oneg_loadSource (oneg_State* state, const char* fileName) {

	assert (state);
	assert (fileName);
	
	// Open file
	
	if (access (fileName, F_OK | R_OK) != 0)
		return_oneg_errno (SOURCE_FILE_ACCESS);

	FILE* source = fopen (fileName, "rb"); 
	if (!source)
		return_oneg_errno (SOURCE_FILE_OPEN);
	
	// Get file size
	
	fseek (source, 0L, SEEK_END);
	state -> bufferSize = ftell (source);
	rewind (source);
	
	// Load file to buffer
	
	state -> buffer = (char*) calloc (state -> bufferSize + 3, 1); 
	// not sizeof(char) because bufferSize is in bytes
	if (!state -> buffer) {
		fclose (source);
		return_oneg_errno (SOURCE_FILE_BUFFER);
	}

	if ((fread (state -> buffer, 1, state -> bufferSize, source) < state -> bufferSize)
	  || ferror (source)) {
		fclose (source);
		return_oneg_errno (SOURCE_FILE_READ);
	}

	// Finish

	fclose (source);
	return_oneg_errno (NO_ERROR);
}


int oneg_splitSource (oneg_State* state) {

	assert (state);

	// Count lines

	state -> numOfLines = 0;
	for (size_t i = 0; i < state -> bufferSize; i++)
		if (state -> buffer[i] == '\n') 
			state -> numOfLines++;

	// Split source to lines

	state -> lines = (char**) calloc (state -> numOfLines, sizeof(char*));
	if (!state -> lines)
		return_oneg_errno (SPLITTED_LINES_BUFFER);

	char* saveptr = state -> buffer;
	size_t j = 0; // counter for splitted lines
	for (size_t i = 0; i < state -> bufferSize; i++)
		if (state -> buffer[i] == '\n') {
			state -> buffer[i] = 0;
			state -> lines[j++] = saveptr;
			saveptr = &(state -> buffer[i]) + 1;
		}

	return_oneg_errno (NO_ERROR);
}


void oneg_sortLines (oneg_State* state, int (*compar)(const void*, const void*, void*)) {

	assert (state);
	assert (compar);

	void* saveptr = NULL;
	qsort_r (state -> lines, state -> numOfLines, sizeof(char*), compar, saveptr);
}


int oneg_writeSorted (oneg_State* state, const char* fileName) {

	assert (state);
	assert (fileName);
	
	size_t numOfWrittenLines = 0;

	// Open file
	
	FILE* dest = fopen (fileName, "w"); 
	if (!dest)
		return_oneg_errno (DEST_FILE_OPEN);

	// Write buffer to file according to sorted lines
	
	for (size_t i = 0; i < state -> numOfLines; i++) {
		if (fprintf (dest, "%s\n", state -> lines[i]) > 0)
			numOfWrittenLines++;
		else
			break;
	}
	fclose (dest);

	if (numOfWrittenLines < state -> numOfLines)
		return_oneg_errno (DEST_FILE_WRITE);

	return_oneg_errno (NO_ERROR);
}


void oneg_free (oneg_State* state) {
	
	assert (state);

	if (state -> buffer)
		free (state -> buffer);
	if (state -> lines)
		free (state -> lines);
	free (state);
}

#undef return_oneg_errno

