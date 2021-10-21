/** @file
  String type library interface
 
  @author Dawid Mędrek
  @date 2021
*/

#ifndef __NEWSTRING__
#define __NEWSTRING__

#include <stdbool.h>
#include <stddef.h>

/**
 * Struct representing a string.
 */
typedef struct {
  char *chars; ///< characters the string consists of
  size_t length; ///< number of characters of the string (length)
  size_t size; ///< size of the array of chracters
} string_t;

/** Type representing what character a line ended with */
typedef enum {
  EndOfFile, ///< line ended with an EOF char
  EndOfLine ///< line ended with a new line character
} LineType;

/**
 * Creates a new, empty string and returns it.
 * @return new, empty string
 */
string_t CreateString();

/**
 * Returns the number of characters a string consists of (its length).
 * @param[in] string : pointer to a string
 * @return length of the string
 */
size_t StringLength(string_t *string);

/**
 * Gets input from the standard stream until coming across an @p EOF
 * or a new line character and adds them to a string (however, the ending
 * character is not added). If the line ended with an @p EOF, returns
 * @p EndOfFile. Otherwise, returns @p EndOfLine.
 * @param[in] string : pointer to a string
 * @return @p EndOfFile if @p EOF was the last character in the line;
 * @p EndOfLine otherwise
 */
LineType ScanString(string_t *string);

/**
 * Returns the character at a given index in a string.
 * @param[in] string : pointer to a string
 * @param[in] index : index
 * @return character at the index @p index in @p string
 */
char CharAt(string_t *string, const size_t index);

/**
 * Compares a string with an sequence of characters.
 * @param[in] string : pointer to a string
 * @param[in] text : a @p char sequence
 * @return @p true if @p string and @p text represent the same
 * sequence of characters (when they are isomorphic); @p false otherwise
 */
bool CompareTo(string_t *string, const char *text);

/**
 * Returns a pointer to the array of characters a string consists of,
 * starting at index @p index. Actions performed at the returned array
 * affect the original string.
 * @param[in] string : pointer to a string
 * @param[in] index : index
 * @return pointer to the fragment of the array the string consists of
 * starting at index @p index
 */
char *GetCharArrayAt(string_t *string, const size_t index);

/**
 * Trims the string to an empty one. The allocated memory for it
 * is not freed.
 * @param[in] string : pointer to a string
 */
void ResetString(string_t *string);

/**
 * Frees the memory for a string and sets its atributes so that
 * it represents an empty one again (its length and size are equal to 0).
 * @param[in] string : pointer to a string
 */
void DestroyString(string_t *string);

#endif

