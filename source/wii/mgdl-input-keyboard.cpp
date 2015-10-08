#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <gccore.h>
#include <sys/iosupport.h>
#include <wiikeyboard/usbkeyboard.h>

#include "mgdl-wii/mgdl-types.h"
#include "mgdl-wii/mgdl-globals.h"
#include "mgdl-wii/mgdl-globals-internal.h"
#include "mgdl-wii/mgdl-main.h"
#include "mgdl-wii/mgdl-input.h"


#define STDIN_MAX_CHARS      16
#define KEY_QUERY_MAX		 16

#define KEYBOARD_NUM_LOCK    0x1
#define KEYBOARD_CAPS_LOCK   0x2
#define KEYBOARD_SCROLL_LOCK 0x4


// Internal functions and variables
namespace gdl {

bool UsbInitialized = false;

namespace Keyboard {


    // Default keymap table for standard US English QWERTY keyboards (233 entries)
    static u_char DefaultKeyMap[] = {
        // STD, SHFT, KC
        0   , 0,    0,
        0   , 0,    0,
        0   , 0,    0,
        0   , 0,    0,    // 3
        'a' , 'A',  gdl::Keyboard::KEY_A,
        'b' , 'B',  gdl::Keyboard::KEY_B,
        'c' , 'C',  gdl::Keyboard::KEY_C,
        'd' , 'D',  gdl::Keyboard::KEY_D,
        'e' , 'E',  gdl::Keyboard::KEY_E,
        'f' , 'F',  gdl::Keyboard::KEY_F,
        'g' , 'G',  gdl::Keyboard::KEY_G,
        'h' , 'H',  gdl::Keyboard::KEY_H,
        'i' , 'I',  gdl::Keyboard::KEY_I,
        'j' , 'J',  gdl::Keyboard::KEY_J,
        'k' , 'K',  gdl::Keyboard::KEY_K,
        'l' , 'L',  gdl::Keyboard::KEY_L,
        'm' , 'M',  gdl::Keyboard::KEY_M,
        'n' , 'N',  gdl::Keyboard::KEY_N,
        'o' , 'O',  gdl::Keyboard::KEY_O,
        'p' , 'P',  gdl::Keyboard::KEY_P,
        'q' , 'Q',  gdl::Keyboard::KEY_Q,
        'r' , 'R',  gdl::Keyboard::KEY_R,
        's' , 'S',  gdl::Keyboard::KEY_S,
        't' , 'T',  gdl::Keyboard::KEY_T,
        'u' , 'U',  gdl::Keyboard::KEY_U,
        'v' , 'V',  gdl::Keyboard::KEY_V,
        'w' , 'W',  gdl::Keyboard::KEY_W,
        'x' , 'X',  gdl::Keyboard::KEY_X,
        'y' , 'Y',  gdl::Keyboard::KEY_Y,
        'z' , 'Z',  gdl::Keyboard::KEY_Z,
        '1' , '!',  gdl::Keyboard::KEY_1,
        '2' , '@',  gdl::Keyboard::KEY_2,
        '3' , '#',  gdl::Keyboard::KEY_3,
        '4' , '$',  gdl::Keyboard::KEY_4,
        '5' , '%',  gdl::Keyboard::KEY_5,
        '6' , '^',  gdl::Keyboard::KEY_6,
        '7' , '&',  gdl::Keyboard::KEY_7,
        '8' , '*',  gdl::Keyboard::KEY_8,
        '9' , '(',  gdl::Keyboard::KEY_9,
        '0' , ')',  gdl::Keyboard::KEY_0,
        0   , 0  ,  gdl::Keyboard::KEY_ENTER,  // 40 - ENTER
        0   , 0  ,  gdl::Keyboard::KEY_ESCAPE,  // 41 - ESCAPE
        0   , 0  ,  gdl::Keyboard::KEY_BACKSPACE,  // 42 - BACKSPACE
        0   , 0  ,  gdl::Keyboard::KEY_TAB,  // 43 - TAB
        ' ' , ' ',  gdl::Keyboard::KEY_SPACE,
        '-' , '_',  gdl::Keyboard::KEY_DASH,
        '=' , '+',  gdl::Keyboard::KEY_EQUAL,
        '[' , '{',  gdl::Keyboard::KEY_LEFT_BRACKET,
        ']' , '}',  gdl::Keyboard::KEY_RIGHT_BRACKET,
        '\\', '|',  gdl::Keyboard::KEY_BACKSLASH,
        0   , 0  ,  0,
        ';' , ':',  gdl::Keyboard::KEY_SEMICOLON,
        '\'', '"',  gdl::Keyboard::KEY_APOSTROPHE,
        '`' , '~',  gdl::Keyboard::KEY_GRAVE_ACCENT,
        ',' , '<',  gdl::Keyboard::KEY_COMMA,
        '.' , '>',  gdl::Keyboard::KEY_PERIOD,
        '/' , '?',  gdl::Keyboard::KEY_SLASH,
        0   , 0  ,  gdl::Keyboard::KEY_CAPS_LOCK,  // 57 - Caps Lock
        0   , 0  ,  gdl::Keyboard::KEY_F1,  // F1
        0   , 0  ,  gdl::Keyboard::KEY_F2,  // F2
        0   , 0  ,  gdl::Keyboard::KEY_F3,  // F3
        0   , 0  ,  gdl::Keyboard::KEY_F4,  // F4
        0   , 0  ,  gdl::Keyboard::KEY_F5,  // F5
        0   , 0  ,  gdl::Keyboard::KEY_F6,  // F6
        0   , 0  ,  gdl::Keyboard::KEY_F7,  // F7
        0   , 0  ,  gdl::Keyboard::KEY_F8,  // F8
        0   , 0  ,  gdl::Keyboard::KEY_F9,  // F9
        0   , 0  ,  gdl::Keyboard::KEY_F10,  // F10
        0   , 0  ,  gdl::Keyboard::KEY_F11,  // F11
        0   , 0  ,  gdl::Keyboard::KEY_F12,  // F12
        0   , 0  ,  gdl::Keyboard::KEY_PRINT_SCREEN,  // 70 - Print Screen
        0   , 0  ,  gdl::Keyboard::KEY_SCROLL_LOCK,  // 71 - Scroll lock
        0   , 0  ,  gdl::Keyboard::KEY_PAUSE,  // 72 - Pause / Break
        0   , 0  ,  gdl::Keyboard::KEY_INSERT,  // 73 - Insert
        0   , 0  ,  gdl::Keyboard::KEY_HOME,  // 74 - Home
        0   , 0  ,  gdl::Keyboard::KEY_PAGE_UP,  // 75 - PgUp
        0   , 0  ,  gdl::Keyboard::KEY_DELETE,  // 76 - Delete
        0   , 0  ,  gdl::Keyboard::KEY_END,  // 77 - End
        0   , 0  ,  gdl::Keyboard::KEY_PAGE_DOWN,  // 78 - PgDown
        0   , 0  ,  gdl::Keyboard::KEY_RIGHT,  // 79 - Right
        0   , 0  ,  gdl::Keyboard::KEY_LEFT,  // 80 - Left
        0   , 0  ,  gdl::Keyboard::KEY_DOWN,  // 81 - Down
        0   , 0  ,  gdl::Keyboard::KEY_UP,  // 82 - Up
        0   , 0  ,  gdl::Keyboard::KEY_NUM_LOCK,  // 83 - Num Lock
        '/' , '/',  gdl::Keyboard::KEY_KP_DIVIDE,  // 84 - Numpad Divide
        '*' , '*',  gdl::Keyboard::KEY_KP_MULTIPLY,  // 85 - Numpad Multiply
        '-' , '-',  gdl::Keyboard::KEY_KP_MINUS,  // 86 - Numpad Minus
        '+' , '+',  gdl::Keyboard::KEY_KP_PLUS,  // 87 - Numpad Plus
        0   , 0  ,  gdl::Keyboard::KEY_KP_ENTER,  // 88 - Numpad Enter
        '1' , 0  ,  gdl::Keyboard::KEY_KP_1,  // 89 - Numpad 1
        '2' , 0  ,  gdl::Keyboard::KEY_KP_2,  // 90 - Numpad 2
        '3' , 0  ,  gdl::Keyboard::KEY_KP_3,  // 91 - Numpad 3
        '4' , 0  ,  gdl::Keyboard::KEY_KP_4,  // 92 - Numpad 4
        '5' , 0  ,  gdl::Keyboard::KEY_KP_5,  // 93 - Numpad 5
        '6' , 0  ,  gdl::Keyboard::KEY_KP_6,  // 94 - Numpad 6
        '7' , 0  ,  gdl::Keyboard::KEY_KP_7,  // 95 - Numpad 7
        '8' , 0  ,  gdl::Keyboard::KEY_KP_8,  // 96 - Numpad 8
        '9' , 0  ,  gdl::Keyboard::KEY_KP_9,  // 97 - Numpad 9
        '0' , 0  ,  gdl::Keyboard::KEY_KP_0,  // 98 - Numpad 0
        '.' , 0  ,  gdl::Keyboard::KEY_KP_DECIMAL,  // 99 - Numpad Decimal
        0   , 0  ,  0,  // 100
        0   , 0  ,  gdl::Keyboard::KEY_MENU,  // 101 - Menu Button
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  0,
        0   , 0  ,  gdl::Keyboard::KEY_LEFT_CTRL,       // 224 - Left Ctrl
        0   , 0  ,  gdl::Keyboard::KEY_LEFT_SHIFT,      // 225 - Left Shift
        0   , 0  ,  gdl::Keyboard::KEY_LEFT_ALT,        // 226 - Left Alt
        0   , 0  ,  gdl::Keyboard::KEY_LEFT_WINKEY,     // 227 - Left WinKey
        0   , 0  ,  gdl::Keyboard::KEY_RIGHT_CTRL,      // 228 - Right Ctrl
        0   , 0  ,  gdl::Keyboard::KEY_RIGHT_SHIFT,     // 229 - Right Shift
        0   , 0  ,  gdl::Keyboard::KEY_RIGHT_ALT,       // 230 - Right Alt
        0   , 0  ,  gdl::Keyboard::KEY_RIGHT_WINKEY,    // 231 - Right WinKey
        0   , 0  ,  0,
    };


    // Keymap and key state buffer
    static char		KeyMap[233*3];
	static bool		keyDown[gdl::Keyboard::KEY_LAST] = { 0 };

    // Thread handles
    static lwp_t	kbThread = 0;        // Main keyboard handler thread
    static lwp_t	tmThread = 0;		// Typematic emulator thread

    // Keyboard status
    static bool		initialized = false;
    static bool		connected = false;               // Keyboard connected flag
    static short	lockStates = KEYBOARD_NUM_LOCK; // Keyboard lock states
    static short	modifierPressed = 0;
    static char		lastCharPressed = 0;

    // Callback function pointers
    static gdl::ConnectCallbackFunc*		connectCallbackPtr = NULL;
    static gdl::Keyboard::KeyCallbackFunc*	keyCallbackPtr = NULL;


	// For the key press query system (used for callback)
	typedef struct {
		short chr;
		short key;
		short scancode;
		short action;
        short mods;
	} keyQueryEntry;

	typedef struct {
        short pressed;
        short tail;
        short head;
		keyQueryEntry buff[KEY_QUERY_MAX];
	} keyQueryStruct;

	static keyQueryStruct keyQuery;

	static void QueryKeyPress(short chr, short key, short scancode, short action, short mods) {

		if (keyQuery.pressed < KEY_QUERY_MAX)
			keyQuery.pressed++;
		else
			keyQuery.head = (keyQuery.head+1) % KEY_QUERY_MAX;

		keyQuery.buff[keyQuery.tail].chr		= chr;
		keyQuery.buff[keyQuery.tail].key		= key;
		keyQuery.buff[keyQuery.tail].scancode	= scancode;
		keyQuery.buff[keyQuery.tail].action		= action;
		keyQuery.buff[keyQuery.tail].mods		= mods;
		keyQuery.tail = (keyQuery.tail+1) % KEY_QUERY_MAX;

	}

	static void PollKeyboard() {

		lastCharPressed = 0;

        while(keyQuery.pressed) {

			if (keyCallbackPtr != NULL) {

				keyCallbackPtr(
					keyQuery.buff[keyQuery.head].key,
					keyQuery.buff[keyQuery.head].scancode,
					keyQuery.buff[keyQuery.head].action,
					keyQuery.buff[keyQuery.head].mods
				);

			}

			lastCharPressed = keyQuery.buff[keyQuery.head].chr;

			keyQuery.head = (keyQuery.head+1) % KEY_QUERY_MAX;
			keyQuery.pressed--;


        }

	}


    // For stdin support
    namespace StdIn {

        // Read function callback for stdin
        static ssize_t stdinRead(struct _reent *r, int unused, char *ptr, size_t len) {

            // Parse keyboard presses and output them to stdin
            int charsRead = 0;

			// Loop until read request is full
            while((size_t)charsRead < len) {

				// If keyboard was disconnected, exit parse loop
				if (!gdl::Keyboard::connected) {

					if (charsRead)
						return(charsRead);
					else
						return(-1);

				}

				// Parse key presses in query
				if (gdl::Keyboard::keyQuery.pressed > 0) {

					if ((gdl::Keyboard::keyQuery.buff[gdl::Keyboard::keyQuery.head].action != gdl::Keyboard::KeyReleased) &&
					(gdl::Keyboard::keyQuery.buff[gdl::Keyboard::keyQuery.head].chr)) {

						ptr[charsRead] = gdl::Keyboard::keyQuery.buff[gdl::Keyboard::keyQuery.head].chr;
						charsRead++;

					}

					gdl::Keyboard::keyQuery.head = (gdl::Keyboard::keyQuery.head+1)%KEY_QUERY_MAX;
					gdl::Keyboard::keyQuery.pressed--;

				}

				usleep(20*1000);

            }

            return(charsRead);

        }

        // Device table (for mounting to stdin)
        static const devoptab_t devTable = {
            "stdin",
            0,
            NULL,
            NULL,
            NULL,
            stdinRead,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        };

        // Init function, sets stdin callbacks for the keyboard driver to input characters into it
        void Init() {

            devoptab_list[STD_IN] = &devTable;
            setvbuf(stdin, NULL, _IONBF, 0);

        }

    }


    // Typematic emulation system
    namespace Typematic {

        static bool KeyStillDown = false;
        static char KeyDown = 0;
        static int ScancodeDown = 0;
        static int LastScancodeDown = 0;

        void *TmThreadFunc(void *arg) {

            int typeCount = 0;

            while(1) {

                if (KeyStillDown) {

                    if (ScancodeDown != LastScancodeDown) {

                        LastScancodeDown = ScancodeDown;
                        typeCount = -1;

                    }

                    typeCount += 1;
                    if (typeCount >= 40) {

                        //gdl::Keyboard::StdIn::SendChar(KeyDown);
                        gdl::Keyboard::QueryKeyPress(KeyDown, gdl::Keyboard::KeyMap[(3*ScancodeDown)+2], ScancodeDown, gdl::Keyboard::KeyRepeat, gdl::Keyboard::modifierPressed);
                        typeCount = 35;

                    }

                } else {

                    typeCount = 0;

                }

                usleep(10*1000);

            }

            return(NULL);

        }


    }


    // Keyboard callback function
	static void _input_KbCallback(USBKeyboard_event event) {

		if (event.keyCode > 233)
			return;

		short keyCode = 3*event.keyCode;
		short keyAction = 0;
		short keyChar = 0;

		// Parse modifier key presses
		if (event.type == 0) {

			gdl::Keyboard::keyDown[(int)gdl::Keyboard::KeyMap[keyCode+2]] = true;
			keyAction = gdl::Keyboard::KeyPressed;

			if ((gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_LEFT_SHIFT) ||
			(gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_RIGHT_SHIFT)) {

				gdl::Keyboard::modifierPressed |= gdl::Keyboard::KeyShift;

			} else if ((gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_LEFT_CTRL) ||
			(gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_RIGHT_CTRL)) {

				gdl::Keyboard::modifierPressed |= gdl::Keyboard::KeyCtrl;

			} else if ((gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_LEFT_ALT) ||
			(gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_RIGHT_ALT)) {

				gdl::Keyboard::modifierPressed |= gdl::Keyboard::KeyAlt;

			}

		} else if (event.type == 1) {

			gdl::Keyboard::keyDown[(int)gdl::Keyboard::KeyMap[keyCode+2]] = false;
			keyAction = gdl::Keyboard::KeyReleased;

			if ((gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_LEFT_SHIFT) ||
			(gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_RIGHT_SHIFT)) {

				if (gdl::Keyboard::modifierPressed & gdl::Keyboard::KeyShift)
					gdl::Keyboard::modifierPressed ^= gdl::Keyboard::KeyShift;

			} else if ((gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_LEFT_CTRL) ||
			(gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_RIGHT_CTRL)) {

				if (gdl::Keyboard::modifierPressed & gdl::Keyboard::KeyCtrl)
					gdl::Keyboard::modifierPressed ^= gdl::Keyboard::KeyCtrl;

			} else if ((gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_LEFT_ALT) ||
			(gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_RIGHT_ALT)) {

				if (gdl::Keyboard::modifierPressed & gdl::Keyboard::KeyAlt)
					gdl::Keyboard::modifierPressed ^= gdl::Keyboard::KeyAlt;

			}

		}


		// If event type is 2, the keyboard was disconnected
		if (event.type == 2) {
			gdl::Keyboard::connected = false;
			return;
		}


		// Parse character key presses
		if (gdl::Keyboard::KeyMap[keyCode] > 0) {

			if ((!(gdl::Keyboard::lockStates & KEYBOARD_NUM_LOCK)) &&
			(gdl::Keyboard::KeyMap[keyCode+2] >= gdl::Keyboard::KEY_KP_1) &&
			(gdl::Keyboard::KeyMap[keyCode+2] <= gdl::Keyboard::KEY_KP_DECIMAL)) {

				// No logic for keypad arrows yet

			} else {

				// If shift pressed, use the alternate character code for when the shift key is pressed
				if ((gdl::Keyboard::modifierPressed & gdl::Keyboard::KeyShift) && (!(gdl::Keyboard::modifierPressed & gdl::Keyboard::KeyCtrl)))
					keyChar = gdl::Keyboard::KeyMap[keyCode+1];
				else
					keyChar = gdl::Keyboard::KeyMap[keyCode];


				if (!(gdl::Keyboard::modifierPressed & gdl::Keyboard::KeyCtrl)) {

					// If caps lock is on, make characters upper-case
					if (gdl::Keyboard::lockStates & KEYBOARD_CAPS_LOCK)
						keyChar = toupper(keyChar);

				} else {

					// If Ctrl is pressed, step down character code by 64 if its an alphabetic character
					keyChar = toupper(keyChar);
					if ((keyChar >= 65) && (keyChar <= 90))
						keyChar -= 64;
					else
						keyChar = 0;

				}

			}

		} else if (gdl::Keyboard::KeyMap[keyCode+2]) {

			// Escape
			if (gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_ESCAPE) {

				keyChar = '\e';

			// Backspace
			} else if (gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_BACKSPACE) {

				keyChar = '\b';

			// Tab
			} else if (gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_TAB) {

				keyChar = '\t';

			// Enter
			} else if ((gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_ENTER) || (gdl::Keyboard::KeyMap[keyCode+2] == gdl::Keyboard::KEY_KP_ENTER)) {

				keyChar = '\n';

			}

		}


		// Start Typematic handling (for keys to repeat when pressed down)
		if (event.type == 0) {

			gdl::Keyboard::Typematic::KeyDown = keyChar;
			gdl::Keyboard::Typematic::ScancodeDown = event.keyCode;
			gdl::Keyboard::Typematic::KeyStillDown = true;

		} else {

			if (gdl::Keyboard::Typematic::ScancodeDown == event.keyCode)
				gdl::Keyboard::Typematic::KeyStillDown = false;

		}


		// Insert key to query
		QueryKeyPress(keyChar, gdl::Keyboard::KeyMap[keyCode+2], event.keyCode, keyAction, gdl::Keyboard::modifierPressed);


		// Handle num/caps/scroll lock status lights
		if (event.type == 0) {

			if (event.keyCode == 83) {

				gdl::Keyboard::lockStates ^= KEYBOARD_NUM_LOCK;
				USBKeyboard_SetLed(USBKEYBOARD_LEDNUM, gdl::Keyboard::lockStates&KEYBOARD_NUM_LOCK);

			} else if (event.keyCode == 57) {

				gdl::Keyboard::lockStates ^= KEYBOARD_CAPS_LOCK;
				USBKeyboard_SetLed(USBKEYBOARD_LEDCAPS, gdl::Keyboard::lockStates&KEYBOARD_CAPS_LOCK);

			} else if (event.keyCode == 71) {

				gdl::Keyboard::lockStates ^= KEYBOARD_SCROLL_LOCK;
				USBKeyboard_SetLed(USBKEYBOARD_LEDSCROLL, gdl::Keyboard::lockStates&KEYBOARD_SCROLL_LOCK);

			}

		}


	}


	// Keyboard handler thread
	static void* KbThreadFunc(void *args) {

		// Detect if keyboard is connected
		KeyboardReconnect:

		{

			// Keep trying until it had been opened successfully
			while(1) {

				// Once detected, leave detection loop
				if (USBKeyboard_Open(_input_KbCallback) >= 0)
					break;

				usleep(200*1000);

			}

		}


		// Call connect callback if set telling that the keyboard was connected
		if (gdl::Keyboard::connectCallbackPtr != NULL)
			gdl::Keyboard::connectCallbackPtr(true);


		// Flash all keyboard lights to tell the user that keyboard had been detected (like on a PC)
		USBKeyboard_SetLed(USBKEYBOARD_LEDNUM, true);
		USBKeyboard_SetLed(USBKEYBOARD_LEDCAPS, true);
		USBKeyboard_SetLed(USBKEYBOARD_LEDSCROLL, true);

		usleep(200*1000);

		// Set initial lock states
		USBKeyboard_SetLed(USBKEYBOARD_LEDNUM, gdl::Keyboard::lockStates&KEYBOARD_NUM_LOCK);
		USBKeyboard_SetLed(USBKEYBOARD_LEDCAPS, gdl::Keyboard::lockStates&KEYBOARD_CAPS_LOCK);
		USBKeyboard_SetLed(USBKEYBOARD_LEDSCROLL, gdl::Keyboard::lockStates&KEYBOARD_SCROLL_LOCK);

		gdl::Keyboard::connected = true;


		// Parse keyboard presses
		while(gdl::Keyboard::connected) {

			if (USBKeyboard_Scan() < 0)
				break;

		}


		// Close keyboard once it had been disconnected
		USBKeyboard_Close();
		gdl::Keyboard::connected = false;


		// Call connect callback if set telling that the keyboard was disconnected
		if (gdl::Keyboard::connectCallbackPtr != NULL)
			gdl::Keyboard::connectCallbackPtr(false);


		// Retry detection
		goto KeyboardReconnect;


		return(NULL);

	}


}

}


gdl::ConnectCallbackFunc* gdl::Keyboard::SetConnectCallback(gdl::ConnectCallbackFunc* callback) {

	if (!gdl::Keyboard::initialized)
		return(NULL);

	ConnectCallbackFunc* oldCallback = gdl::Keyboard::connectCallbackPtr;
    gdl::Keyboard::connectCallbackPtr = callback;

    return(oldCallback);

}

gdl::Keyboard::KeyCallbackFunc* gdl::Keyboard::SetKeyCallback(KeyCallbackFunc* callback) {

	if (!gdl::Keyboard::initialized)
		return(NULL);

	KeyCallbackFunc* oldCallback = gdl::Keyboard::keyCallbackPtr;
    gdl::Keyboard::keyCallbackPtr = callback;

    return(oldCallback);

}

bool gdl::Keyboard::Init(ConnectCallbackFunc* callback, const char* keyMap) {

	if (!gdl::Keyboard::initialized) {

		if (gdl::ConsoleActive)
			printf("gdl: USB keyboard subsystem already initialized...");

		return(false);

	}

	if (gdl::ConsoleActive)
		printf("gdl: Initializing USB keyboard subsystem... ");

    // Initialize USB interface
	if (gdl::UsbInitialized == false) {

		if (USB_Initialize() != IPC_OK) {
			printf("Could not initialize USB.");
			return(false);
		}

		gdl::UsbInitialized = true;

	}

    // Initialize Keyboard
	if (USBKeyboard_Initialize() != IPC_OK) {
		gdl::CallErrorCallback("Cannot initialize USB keyboard driver.");
        return(false);
	}

    gdl::Keyboard::connected = false;
    gdl::Keyboard::lockStates = KEYBOARD_NUM_LOCK;
    gdl::Keyboard::connectCallbackPtr = callback;
    gdl::Keyboard::keyCallbackPtr = NULL;

	// Copy keymap table from either the default mapping or a user defined map
	if (keyMap == NULL)
		memcpy(gdl::Keyboard::KeyMap, gdl::Keyboard::DefaultKeyMap, 233*3);
	else
		memcpy(gdl::Keyboard::KeyMap, keyMap, 233*3);


    // Mount stdin function to devoptab
    gdl::Keyboard::StdIn::Init();


    // Start keyboard driver thread
    if (LWP_CreateThread(&gdl::Keyboard::kbThread, gdl::Keyboard::KbThreadFunc, NULL, NULL, 1024*8, 15) < 0) {
		gdl::CallErrorCallback("Cannot create keyboard handler thread.");
        return(false);
    }


    // Start Typematic driver thread
    if (LWP_CreateThread(&gdl::Keyboard::tmThread, gdl::Keyboard::Typematic::TmThreadFunc, NULL, NULL, 1024*4, 15) < 0) {
		gdl::CallErrorCallback("Cannot create keyboard typematic thread.");
        return(false);
    }


	if (gdl::ConsoleActive)
		printf("Ok.\n");


	gdl::Keyboard::initialized = true;
	gdl::wii::PollKeyboardCallback = gdl::Keyboard::PollKeyboard;

    return(true);

}

bool gdl::Keyboard::KeyDown(gdl::Keyboard::KeyCode keyCode) {

	if (!gdl::Keyboard::initialized)
		return(false);

    return(gdl::Keyboard::keyDown[keyCode]);

}

char gdl::Keyboard::GetChar() {

	if (!gdl::Keyboard::initialized)
		return(false);

	return(lastCharPressed);

}
