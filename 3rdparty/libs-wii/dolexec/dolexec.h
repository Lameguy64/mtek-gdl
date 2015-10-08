/****************************************************************************
 * dolexec - A Wii dol forwarder library by Lameguy64/TheCodingBrony
 * Copyleft 2014 Meido-Tek Productions
 *
 * Based from FIX94's loadMii forwarder.
 *
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 ***************************************************************************/
 
#ifndef _DOLEXEC_H_
#define _DOLEXEC_H_

#ifdef __cplusplus
extern "C" {
#endif


/**	Loads a dol file and then executes it.
 *
 *	This function will attempt to load and execute a dol file. It'll also
 *	pass the absolute file name with path of the dol file being executed
 *	as the first argument for programs that access files relative to where
 *	the dol file is located.
 *
 *	If you want to specify an argument that contains spaces (such as a file
 *	or directory path), you can encapsulate such arguments with brackets or
 *	quotation marks.
 *
 *	Notes: 
 *	- The absolute path of the dol file is acquired through getcwd()
 *	  and chdir() functions.
 *	- File name and path must be no longer than 256 characters.
 *
 *	param[in]	fileName	Name of dol file to load (may contain a path).
 *	param[in]	args		Arguments.
 *
 *	returns: Zero if it failed to load the dol file.
 *
 */
bool loadDolFromFile(const char *fileName, const char* args);


/**	Loads dol data from a memory buffer and then executes it.
 *
 *	Do not use this function when the dol data to be executed is stored
 *	in MEM2 otherwise, the forwarder will most likely fail.
 *
 *	Because there is no file name specified, the first argument would still be
 *	present but the string will be blank. However, you can specify a file path
 *	as the first argument.
 *
 *	param[in]	dolProgram	Pointer to dol data.
 *	param[in]	dolSize		Size of dol data in bytes.
 *	param[in]	args		Program arguments.
 *
 */
bool loadDolFromMemory(void *dolProgram, int dolSize, const char* args);


#ifdef __cplusplus
}
#endif

#endif