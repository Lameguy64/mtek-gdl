/*! \file mgdl-config.h
 *	\brief Library configuration header
 *
 *	\details This header file defines the general internal configuration of this library. If you change any of the settings
 *	in this file, a recompile is necessary.
 */

#ifndef _GDL_WII_CONFIG_H
#define _GDL_WII_CONFIG_H


/*! \addtogroup libConfigDefs Library Settings
 *	\brief Defines and macros of this library's internal configuration.
 *	\details All these settings can be found within the mgdl-config.h header file.
 *	@{
 */


//! Library Version
/*!
 *	If you wish to release a modified version of this library, you may want to change this to 0.90-unofficial or something
 *	along those lines to minimize confusion.
 */
#define GDL_VERSION "0.75"


//! Disable sound module
/*!
 *	This defines whether or not the sound module of this library will be disabled completely. If set to TRUE, the ASND
 *	library will not be initialized and sound related functions are disabled.
 */
#define GDL_NO_SOUND FALSE


//! GPU FIFO Buffer Size
/*!
 *	This defines the size of the GPU FIFO buffer in bytes.
 *
 *	Increasing the FIFO buffer size is not really necessary if you're using display lists for processing large amounts of GX
 *	commands since display lists are generally stored in its own memory buffer and the main FIFO buffer would simply read off
 *	of that when the list gets called. Change this value only if you know exactly what you're doing.
 */
#define GDL_GP_FIFO_SIZE (1024*256)


/*! \addtogroup mem2macros MEM2 Memory Management Macros
 *	\brief Macros to seconday memory management functions for MEM2 memory management.
 *
 *	\details For convenience, MEM2 memory management routines are defined by these macros to make it convenient to replace
 *	memory managers by simply modifying the appropriate macros.
 *
 * @{
 */

#include <valloc.h>

#define malloc2(s)	(valloc(s))	/*!< MEM2 version of malloc(). Its actually an alias for valloc() by default. */
#define free2(s)	(vfree(s))	/*!< MEM2 version of free(). Its actually an alias for vfree() by default. */


/*! @} */

/*! @}  */


#endif // _GDL_WII_CONFIG_H
