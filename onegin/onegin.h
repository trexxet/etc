/*
 * =====================================================================================
 *
 *       Filename:  onegin.h
 *
 *    Description:  Text file sorter header file
 *
 *        Version:  1.0
 *        Created:  08/27/2017 07:54:03 PM
 *       Compiler:  gcc
 *
 *         Author:  trexxet
 *
 * =====================================================================================
 */

#pragma once


/* 
 * Error codes
 */
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
} ONEG_ERRNO;


/*
 * Explanation of ONEG_ERRNO codes
 */
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


/*
 * Match ONEG_ERRNO code to it's explanation
 */
#define oneg_strerr(_err_) oneg_strerrlist[_err_]


/*==================================================================================*/


/*
 * Onegin state structure
 */
typedef struct {
	char* buffer;        // Buffer for data from source file
	wchar_t* wbuffer;    // Main data buffer
	size_t bufferSize;   // Size of buffers

	wchar_t** lines;     // Pointers on lines in wbuffer
	size_t numOfLines;   // Number of lines

	int oneg_errno;      // ONEG_ERRNO of state
} oneg_State;



/*==================================================================================*/


/*
 * Create onegin state
 * Returns pointer on created onegin state
 */
oneg_State* oneg_init ();
/* 
 * Load text from a source file $filename to $state buffer and convert it into wbuffer
 * Returns ONEG_ERRNO of $state
 */
int oneg_loadSource (oneg_State* state, const char* fileName);
/* 
 * Split $state wbuffer into lines
 * Returns ONEG_ERRNO of $state
 */
int oneg_splitSource (oneg_State* state);
/*
 * Sort $state lines according to a comparsion function pointed by $compar
 * The comparsion function must return an integer less than, equal to or greater than zero
 * if the first argument is considered to be respectively less than, equal to or greater
 * than the second
 */
void oneg_sortLines (oneg_State *state, int (*compar) (const void*, const void*, void*));
/*
 * Write $state wbuffer according to $state lines pointer to output file $filename
 * Returns ONEG_ERRNO of $state
 */
int oneg_writeSorted (oneg_State* state, const char* fileName);
/* 
 * Free memory used by $state and destroy it
 */
void oneg_free (oneg_State* state);

