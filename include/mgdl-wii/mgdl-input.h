/*!	\file mgdl-input.h
 *	\brief Keyboard and mouse input module header
 */

#ifndef _GDL_INPUT_H
#define _GDL_INPUT_H

#include <ogc/usbmouse.h>
#include <wiikeyboard/usbkeyboard.h>


namespace gdl {

/*! \addtogroup inputModule Input Module
 *	\brief Functions for handling USB keyboard and mouse input devices.
 *
 *	Using a USB keyboard and mouse for input is most useful for prototyping/development purposes as it
 *	initializes faster than a Wiimote and is very beneficial for testing purposes. It can also assist
 *	in porting PC projects that use MTek-GDL to the Wii as you don't have to rewrite its input code
 *	so you can focus on porting over platform-specific code. However it is not advisable to make Wii
 *	homebrew that only supports USB keyboard/mouse input as not everyone likes to use those on a home
 *	console.
 *
 *	The keyboard and mouse subsystems support hot-plugging which means you can detach and reconnect either
 *	devices at any time. However hot-plugging is not 100% perfect such as unplugging the keyboard connected
 *  to the first USB port while a mouse is plugged to the second USB port, the keyboard may sometimes not
 *	initialize (when all status lights blink for a split second) when reconnected until the mouse has been
 *	disconnected. Both USB devices will still be recognized when connected through a USB hub in case you
 *	have a USB storage device or a Broadband Adapter connected to one of the console's USB ports.
 *
 *	This input module actually uses wiiKeyboard for USB keyboard handling and libogc (which actually has
 *	built-in USB mouse support in the library) for USB mouse handling.
 *
 *	\note This is a fairly new implementation of this library which means you might run into some problems
 *	when using this module. Hopefully, its very unlikely for such issues to happen for the most part.
 *
 *	@{
 */


//! Keyboard and mouse connect/disconnect callback
typedef void (ConnectCallbackFunc(bool));


//! Keyboard namespace
/*!
 *	\details Functions, namespaces and typedefs for initializing and reading USB keyboard inputs. The keyboard
 *	subsystem features typematic emulation (for key repeating), event callbacks and can pipe key presses to
 *	stdin once the subsystem is initialized.
 *
 *  \note The keyboard subsystem of this library does not support any other keyboard layout other than US English
 *	keyboards with a QWERTY layout by default. However you can however make your own keymap for different keyboard
 *	layouts not supported by this library but keep in mind that this library does not support unicode or non-latin
 *	character input.
 *
 */
namespace Keyboard {


	//! Keyboard action states
    enum KeyAction {
        KeyPressed = 0,	//!< Key has been pressed.
        KeyReleased,	//!< Key has been released.
        KeyRepeat,		//!< Key is repeated (when key is held down long enough).
    };

	//! Keyboard key codes
    enum KeyCode {
        KEY_UNKNOWN = 0,
        KEY_A, KEY_B, KEY_C, KEY_D,
        KEY_E, KEY_F, KEY_G, KEY_H,
        KEY_I, KEY_J, KEY_K, KEY_L,
        KEY_M, KEY_N, KEY_O, KEY_P,
        KEY_Q, KEY_R, KEY_S, KEY_T,
        KEY_U, KEY_V, KEY_W, KEY_X,
        KEY_Y, KEY_Z, KEY_1, KEY_2,
        KEY_3, KEY_4, KEY_5, KEY_6,
        KEY_7, KEY_8, KEY_9, KEY_0,
        KEY_ENTER,
        KEY_ESCAPE,
        KEY_BACKSPACE,
        KEY_TAB,
        KEY_SPACE,
        KEY_DASH,
        KEY_EQUAL,
        KEY_LEFT_BRACKET,
        KEY_RIGHT_BRACKET,
        KEY_BACKSLASH,
        KEY_SEMICOLON,
        KEY_APOSTROPHE,
        KEY_GRAVE_ACCENT,
        KEY_COMMA,
        KEY_PERIOD,
        KEY_SLASH,
        KEY_CAPS_LOCK,
        KEY_F1, KEY_F2 , KEY_F3 , KEY_F4,
        KEY_F5, KEY_F6 , KEY_F7 , KEY_F8,
        KEY_F9, KEY_F10, KEY_F11, KEY_F12,
        KEY_PRINT_SCREEN,
        KEY_SCROLL_LOCK,
        KEY_PAUSE,
        KEY_INSERT,
        KEY_HOME,
        KEY_PAGE_UP,
        KEY_DELETE,
        KEY_END,
        KEY_PAGE_DOWN,
        KEY_RIGHT,
        KEY_LEFT,
        KEY_DOWN,
        KEY_UP,
        KEY_NUM_LOCK,
        KEY_KP_DIVIDE,
        KEY_KP_MULTIPLY,
        KEY_KP_MINUS,
        KEY_KP_PLUS,
        KEY_KP_ENTER,
        KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_4,
        KEY_KP_5, KEY_KP_6, KEY_KP_7, KEY_KP_8,
        KEY_KP_9, KEY_KP_0, KEY_KP_DECIMAL,
        KEY_MENU,
        KEY_LEFT_CTRL,
        KEY_LEFT_SHIFT,
        KEY_LEFT_ALT,
        KEY_LEFT_WINKEY,
        KEY_RIGHT_CTRL,
        KEY_RIGHT_SHIFT,
        KEY_RIGHT_ALT,
        KEY_RIGHT_WINKEY,
        KEY_LAST,
    };

	//! Keyboard modifier bit masks
    enum KeyMods {
        KeyShift	= 0x01,
        KeyCtrl		= 0x02,
        KeyAlt		= 0x04,
    };

	//! Keyboard lock state bit masks
    enum KeyLocks {
    	KeyNum		= 0x01,
    	KeyCaps		= 0x02,
    	KeyScroll	= 0x04,
    };


    //! Keyboard event callback
    typedef void (KeyCallbackFunc(int, int, int, int));

    //! Keyboard character press callback
	typedef void (KeyCharCallbackFunc(int));


	//! Initializes the USB keyboard module
	/*!
	 *	\details This function initializes the USB keyboard subsystem and must be called first before using any of keyboard
	 *	input functions. Hot-plugging of USB keyboards are supported and a connect/disconnect callback can be set with
	 *	gdl::Keyboard::SetConnectCallback(). The keyboard will still be detected when plugged through a USB hub but only
	 *	one keyboard is supported.
	 *
	 *	\note When initializing with the mouse subsystem, it is recommended to initialize the mouse first before initializing
	 *		the keyboard to avoid hot-plugging problems. It is also recommended to initialize both subsystems after initializing
	 *		libfat and the Wiimotes if you plan on using them.
	 *	\note Once the keyboard is initialized, gdl::PrepDisplay() will now poll keyboard inputs when called similar to the PC
	 *		version of this library, callback events and gdl::Keyboard::GetChar() only work correctly when this function is called.
	 *		Functions that read input through stdin such as getchar() will also work for traditional command-line style input.
	 *	\note If you want to ensure that no key press is missed when polling it, use a key event callback with gdl::SetKeyCallback().
	 *	\note Key presses are always buffered similar to that of a PC until it has been polled with gdl::PrepDisplay() or
	 *		functions that read input through stdin.
	 *	\note When a keyboard is not connected, stdin functions will either return EOF (or -1) or the remaining characters
	 *		pressed before the keyboard was disconnected.
	 *
	 *	\param[in]	callback	Pointer to keyboard connect/disconnect callback (see gdl::Keyboard::SetConnectCallback()).
	 *	\param[in]	keyMap      Pointer to an array of a custom keyboard map.
	 *
	 *	\returns Non-zero if the keyboard subsystem was successfully initialized, otherwise an error occurred.
	 */
    bool Init(ConnectCallbackFunc* callback = NULL, const char* keyMap = NULL);


	//! Sets a keyboard connect callback
    /*!
     *	This function sets a keyboard connect callback which is called whenever a keyboard is connected or
     *	disconnected from one of the console's USB ports.
     *
     *	Define your callback function as follows:
     *	\code{.cpp}
     *	void myCallback(bool connect) { }
     *	\endcode
     *	connect will be true when a keyboard was connected, otherwise it was disconnected.
     *
     *	\note Do not set a callback to a function containing a program loop as the callback is called within a thread
     *		separate from your main program and doing so may result to unpredictable stability problems. Instead, use
     *		it to set state values which should then be parsed by a separate routine within your main program loop.
	 *
     *	\param[in]	*callback	The new keyboard connect callback or NULL to remove the callback.
     *
     *	\returns Pointer to previous callback function or NULL if no callback routine was previously set.
     */
    gdl::ConnectCallbackFunc* SetConnectCallback(gdl::ConnectCallbackFunc* callback);


	//! Sets a keyboard event callback
    /*!
     *  This function sets a callback for key press events which is called whenever a key is pressed,
     *	repeated or released.
     *
     *	Define your callback function as follows:
     *	\code{.cpp}
     *	void myFunc(int keyCode, int scanCode, int action, int mods) { }
     *	\endcode
     *  keyCode is one of gdl::Keyboard::KeyCode of the key that was pressed, repeated or released and
     *	is relative to the currently set keymap, scanCode is the raw keyboard code and is not dependent
     *	to the loaded keymap (see <a href="http://www.usb.org/developers/hidpage/Hut1_12v2.pdf"><b>here</b></a>
	 *	at page 53 for reference), action is one of gdl::Keyboard::KeyAction states, mods are the modifier
     *	keys that were held down when a key is pressed which can be masked with one of gdl::Keyboard::KeyMods;
     *
     *	\note The callback is only executed when keyboard input is polled by gdl::PrepDisplay(). This way,
     *		it can be used to run a function with its own program loop.
     *
     *	\param[in]	*callback	Pointer to keyboard event callback function.
     *
     *	\returns Pointer to previous callback function or NULL if no callback routine was set previously.
     */
    gdl::Keyboard::KeyCallbackFunc* SetKeyCallback(KeyCallbackFunc* callback);


	//! Returns the last reported state of a specified key
	/*!
	 *	This function returns the last state of the specified key whether or not it was pressed down or not.
	 *
	 *	\param[in]	keyCode		The desired gdl::Keyboard::KeyCode to get the last state of (gdl::Keyboard::KEY_UNKNOWN is used for unsupported keys).
	 *
	 *	\returns Non-zero if the specified key was pressed, otherwise it is not.
	 */
	bool KeyDown(gdl::Keyboard::KeyCode keyCode);


	//! Returns the last reported character pressed polled by gdl::PrepDisplay()
	char GetChar();


	//! Returns the status bits of the keyboard's lock states
	/*!
	 *  This function returns the status bits of the 3 lock states on the keyboard which can be tested individually
	 *	by AND'ing against one of gdl::Keyboard::KeyLocks.
	 *
	 *	\note This function is only available in the Wii version of this library.
	 */
	short GetLockStates();

}

/*! @} */

namespace Mouse {

	typedef struct {
		short x;
		short y;
		float xScroll;
		float yScroll;
		bool left;
		bool right;
		bool middle;
		bool leftClick;
		bool rightClick;
		bool middleClick;
		bool inWindow;
	} MouseStateStruct;

	bool Init(ConnectCallbackFunc* callback = NULL);

	MouseStateStruct GetStats();

}


}

#endif // _GDL_INPUT_H
