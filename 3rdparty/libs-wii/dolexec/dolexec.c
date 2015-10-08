#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <gccore.h>
#include <ogc/machine/processor.h>

#include "dolexec.h"


#define EXECUTE_ADDR	((u8 *) 0x92000000)
#define BOOTER_ADDR		((u8 *) 0x93000000)
#define ARGS_ADDR		((u8 *) 0x93200000)

#define MAX_CMDLINE		640
#define MAX_ARGV		512

#define MAX_FILENAME	256


struct __argv	argParam;

char			*a_argv[MAX_ARGV];
char			*meta_buf = NULL;

typedef void	 (*entrypoint) (void);
extern void		 __exception_setreload(int t);
extern void		 __exception_closeall();
extern const u8	 app_booter_bin[];
extern const u32 app_booter_bin_size;


static void _parseArgs(const char *args);
static void _execProgram();
static void _initArg();
static void _addArg(const char *argv);
static char *_getAbsoluteFileName(const char *fileName);


bool loadDolFromFile(const char *fileName, const char *args) {
	
	
	FILE	*dolFile;
	int		dolSize;
	
	
	// Open dol file
	dolFile = fopen(fileName, "rb");
	if (!dolFile)
		return(false);
	
	
	// Get size of dol file
	fseek(dolFile, 0, SEEK_END);
	dolSize = ftell(dolFile);
	rewind(dolFile);
	
	
	// Load dol file into EXECUTE_ADDR (in MEM2) for the bootloader
	if (fread((void*)EXECUTE_ADDR, 1, dolSize, dolFile) != dolSize)
		return(false);
	
	
	// Prepare argument table
	_initArg();
	
	// Set absolute file name with path of loaded dol file
	char *absFileName = _getAbsoluteFileName(fileName);
	_addArg(absFileName);
	free(absFileName);
	
	// Set the specified arguments
	_parseArgs(args);
	
	
	// Execute!
	_execProgram(args);
	
	
	// Just to get rid of compiler warnings
	return(true);
	
	
}


bool loadDolFromMemory(void *dolProgram, int dolSize, const char* args) {
	
	
	// Copy dol code into EXECUTE_ADDR (in MEM2) for the bootloader
	memcpy((void*)EXECUTE_ADDR, dolProgram, dolSize);
	
	
	// Prepare argument table
	_initArg();
	
	// Set the specified arguments
	_parseArgs(args);
	
	
	// Execute!
	_execProgram(args);
	
	
	// Just to get rid of compiler warnings
	return(true);
	
	
}


void _parseArgs(const char *args) {
	
	if (args != NULL) {
		if (strlen(args)) {
			
			bool inText=false;
			bool inBracket=false;
			
			char argBuff[128] = {0};
			
			// Parse the argument string
			short c=0;
			for(c=0; args[c] != 0x00; c++) {
				
				if ((args[c] == '{') || (args[c] == '"')) {
					c++;
					inBracket=true;
					inText=true;
				} else if ((args[c] == '}') || (args[c] == '"')) {
					if (inText) {
						_addArg(argBuff);
						argBuff[0]=0x00;
					}
					inBracket=false;
					inText=false;
					continue;
				}
				
				if (inBracket == false) {
					if (args[c] != ' ') {
						if (inText==false) {
							inText=true;
						}
					} else if (args[c] == ' ') {
						if (inText) {
							_addArg(argBuff);
							argBuff[0]=0x00;
							inText=false;
						}
					}
				}
				
				if (inText)
					strncat(argBuff, &args[c], 1);
				
			}
			
			if (inText)
				_addArg(argBuff);
			
		}
	}
	
}

void _execProgram() {
	
	
	// Copy booter program and argument data into their respective addresses (in MEM2)
	memcpy(BOOTER_ADDR, app_booter_bin, app_booter_bin_size);
	DCFlushRange(BOOTER_ADDR, app_booter_bin_size);
	
	memcpy(ARGS_ADDR, &argParam, sizeof(argParam));
	DCFlushRange(ARGS_ADDR, sizeof(argParam)+argParam.length);
	
	
	// Set screen to black to hide framebuffer garbage during the transition
	VIDEO_SetBlack(TRUE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	
	
	// Set entrypoint for execution
	entrypoint exeEntryPoint;
	exeEntryPoint = (entrypoint)BOOTER_ADDR;
	
	// Launch dol!
	u32 cookie;
	SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	_CPU_ISR_Disable(cookie);
	__exception_closeall();
	exeEntryPoint();
	_CPU_ISR_Restore(cookie);
	
	
}

void _initArg() {
	
	// Prepares argument data for the booter program... Based from FIX94's loadMii forwarder.
	
	
	memset(&argParam, 0x00, sizeof(argParam));
	memset(a_argv, 0x00, sizeof(a_argv));
	
	argParam.argvMagic		= ARGV_MAGIC;
	argParam.length			= 1;
	argParam.argc			= 0;
	
	argParam.commandLine	= (char*)ARGS_ADDR+sizeof(argParam);
	argParam.argv			= a_argv;
	argParam.endARGV		= a_argv;
	
	
}

void _addArg(const char *argv) {
	
	// Adds an argument entry into the argument table... Based from FIX94's loadMii forwarder.
	
	
	if (argParam.argc >= MAX_ARGV) 
		return;
	
	int charCount=strlen(argv);
	
	if ((argParam.length+charCount+1) > MAX_CMDLINE) 
		return;
	
	strcpy(argParam.commandLine+argParam.length-1, argv);
	
	argParam.length += charCount+1;				// 0 term.
	argParam.commandLine[argParam.length-1] = 0;	// double \0\0
	argParam.argc++;
	argParam.endARGV = argParam.argv+argParam.argc;
	
	
}

char *_getAbsoluteFileName(const char *fileName) {
	
	/* A simple function that attempts to get the full file path of a file using
	 * chdir() and getcwd()... It works well for the most part.
	 */
	
	
	// For storing the final result
	char *retFileName = (char*)malloc(MAX_FILENAME);
	memset(retFileName, 0x00, MAX_FILENAME);
	
	// Path name of file
	char filePath[MAX_FILENAME]	= {0};
	
	
	// Get where the end of the path name is located (by slash)
	char *namePos=strrchr(fileName, '\\');
	if (namePos == NULL) namePos=strrchr(fileName, '/');
	
	
	// If there is a path name
	if (namePos != NULL) {
		
		// Copy only the path name of the file
		namePos++;
		strncpy(filePath, fileName, namePos-fileName);
		
		// Convert backslashes into slashes of path name
		short c;
		for(c=0; filePath[c]!=0x00; c++) {
			if (filePath[c] == '\\') filePath[c] = '/';
		}
		
		// Save current directory
		char oldDir[256];
		getcwd(oldDir, 256);
		
		// Attempt to follow the file path and get the current directory from there
		chdir(filePath);
		getcwd(retFileName, 256);
		strcat(retFileName, namePos);
		
		// Convert forward slashes into backslashes
		for(c=0; retFileName[c]!=0x00; c++) {
			if (retFileName[c] == '\\') retFileName[c] = '/';
		}
		
		// Revert back to old directory
		chdir(oldDir);
		
	} else { // If not
		
		// Simply use current directory
		getcwd(retFileName, 256);
		strcat(retFileName, fileName);
		
	}
	
	realloc(retFileName, strlen(retFileName));
	
	return(retFileName);
	
}