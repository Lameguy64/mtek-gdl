#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <gccore.h>
#include <sys/iosupport.h>
#include <ogc/usbmouse.h>

#include "mgdl-wii/mgdl-types.h"
#include "mgdl-wii/mgdl-globals.h"
#include "mgdl-wii/mgdl-globals-internal.h"
#include "mgdl-wii/mgdl-main.h"
#include "mgdl-wii/mgdl-input.h"


#define MOUSE_BUTTON_1	0x1
#define MOUSE_BUTTON_2 	0x2
#define MOUSE_BUTTON_3	0x4


namespace gdl {

extern bool UsbInitialized;

namespace Mouse {

	static lwp_t MsThread = 0;
	static MouseStateStruct mouseStats = { 0 };

	static bool initialized = false;

	static void* MsThreadFunc(void *args) {

		MouseReconnect:

		gdl::Mouse::mouseStats.x = gdl::ScreenCenterX;
		gdl::Mouse::mouseStats.y = gdl::ScreenCenterY;

		while(MOUSE_IsConnected()) {

			gdl::Mouse::mouseStats.inWindow = true;

			mouse_event event;

			while(MOUSE_GetEvent(&event)) {

				gdl::Mouse::mouseStats.x += event.rx;
				gdl::Mouse::mouseStats.y += event.ry;

				if (gdl::Mouse::mouseStats.x < 0)
					gdl::Mouse::mouseStats.x = 0;
				if (gdl::Mouse::mouseStats.x > gdl::ScreenXres-1)
					gdl::Mouse::mouseStats.x = gdl::ScreenXres-1;
				if (gdl::Mouse::mouseStats.y < 0)
					gdl::Mouse::mouseStats.y = 0;
				if (gdl::Mouse::mouseStats.y > gdl::ScreenYres-1)
					gdl::Mouse::mouseStats.y = gdl::ScreenYres-1;

				if (event.button & MOUSE_BUTTON_1)
					gdl::Mouse::mouseStats.left		= true;
				else
					gdl::Mouse::mouseStats.left		= false;

				if (event.button & MOUSE_BUTTON_2)
					gdl::Mouse::mouseStats.right	= true;
				else
					gdl::Mouse::mouseStats.right	= false;

				if (event.button & MOUSE_BUTTON_3)
					gdl::Mouse::mouseStats.middle	= true;
				else
					gdl::Mouse::mouseStats.middle	= false;

				gdl::Mouse::mouseStats.xScroll	= 0;
				gdl::Mouse::mouseStats.yScroll	= event.rz;

			}

		}

		gdl::Mouse::mouseStats.inWindow = false;

		usleep(1000*200);

		goto MouseReconnect;

		return(NULL);

	}

}

}


bool gdl::Mouse::Init(ConnectCallbackFunc* callback) {

	if (gdl::ConsoleActive)
		printf("gdl: Initializing mouse subsystem...");


	// Initialize mouse
	if (MOUSE_Init() < 0) {
        gdl::CallErrorCallback("Cannot initialize USB Mouse.");
        return(false);
    }

    gdl::UsbInitialized = true;

	if (LWP_CreateThread(&gdl::Mouse::MsThread, gdl::Mouse::MsThreadFunc, NULL, NULL, 1024*8, 15) < 0) {
		gdl::CallErrorCallback("Cannot create mouse handler thread.");
        return(false);
    }

    gdl::Mouse::initialized = true;

    return(true);

}

gdl::Mouse::MouseStateStruct gdl::Mouse::GetStats() {

    return(gdl::Mouse::mouseStats);

}
