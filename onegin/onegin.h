/*
 * =====================================================================================
 *
 *       Filename:  onegin.h
 *
 *    Description:  Text lines sorter header file
 *
 *        Version:  1.0
 *        Created:  08/27/2017 07:54:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  trexxet
 *
 * =====================================================================================
 */

#pragma once


enum {
	NO_ERROR = 0,
	SOURCE_FILE_ACCESS,
	SOURCE_FILE_OPEN,
	SOURCE_FILE_BUFFER,
	SOURCE_FILE_READ,
	SPLITTED_LINES_BUFFER,
	DEST_FILE_OPEN,
	DEST_FILE_WRITE
};

static const char * const oneg_strerrlist[] = { 
	"No error",
        "Can't access source file",
        "Can't open source file",
        "Can't create buffer for source file",
        "Error while reading source file",
        "Can't create buffer for splitted lines",
	"Can't create or open destination file",
	"Error while writing to destination file" };

#define oneg_strerr(_err_) oneg_strerrlist[_err_]


typedef struct {
	char* buffer;
	size_t bufferSize;
	char** lines;
	size_t numOfLines;
	int oneg_errno;
} oneg_State;


oneg_State* oneg_init ();
int oneg_loadSource (oneg_State* state, const char* fileName);
int oneg_splitSource (oneg_State* state);
void oneg_sortLines (oneg_State *state, int (*compar) (const void*, const void*, void*));
int oneg_writeSorted (oneg_State* state, const char* fileName);
void oneg_free (oneg_State* state);

