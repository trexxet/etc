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
	EONEG_NO_ERROR = 0,
	EONEG_SET_LOCALE,
	EONEG_SOURCE_FILE_ACCESS,
	EONEG_SOURCE_FILE_OPEN,
	EONEG_SOURCE_FILE_BUFFER,
	EONEG_SOURCE_FILE_READ,
	EONEG_SPLITTED_LINES_BUFFER,
	EONEG_DEST_FILE_OPEN,
	EONEG_DEST_FILE_WRITE
};

static const char* const oneg_strerrlist[] = {
	"No error",
	"Warning: can't set locale, output may be corrupted",
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
	wchar_t* wbuffer;
	size_t bufferSize;

	wchar_t** lines;
	size_t numOfLines;

	int oneg_errno;
} oneg_State;


oneg_State* oneg_init ();
int oneg_loadSource (oneg_State* state, const char* fileName);
int oneg_splitSource (oneg_State* state);
void oneg_sortLines (oneg_State *state, int (*compar) (const void*, const void*, void*));
int oneg_writeSorted (oneg_State* state, const char* fileName);
void oneg_free (oneg_State* state);

