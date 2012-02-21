#ifndef _UTILS_H
#define _UTILS_H

/**
 * @file
 */

#include <string>

/**
 * Get number of elements in an array.
 * @param	x	Array to examine
 * @returns	Number of elements in the array 'x'.
 */
#define NELEMS(x) (sizeof(x)/sizeof(x[0]))

/**
 * Read Only Variable Accessor generator.
 * Generates a generic accessor for a read-only variable.
 * @param	type	Variable type
 * @param	name	Variable name
 */
#define ROVAR(type, name) \
	const type name() const { return _##name; }

/**
 * Read-Write Variable Accessor/Mutator generator.
 * Generates a generic accessor and mutator for a read-write variable.
 * @param	type	Variable type
 * @param	name	Variable name
 */
#define RWVAR(type, name) \
	const type name() const { return _##name; }			\
	void name(type val) { _##name = val; }

/**
 * Trim leading and trailing whitespace from a std::string.
 * @param	str		String from which to trim the leading and trailing whitespace.
 * @note	<b>str</b> is edited in-place and passed by reference.
 */
// trim spaces from a string
void TrimSpaces(std::string &str);

#endif // _UTILS_H
