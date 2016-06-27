#ifndef _CUSTOM_H
#define _CUSTOM_H

#include <windows.h>

#include <gl/glew.h>
#include <glfw/glfw3.h>

#include "mgui.h"


namespace Checks {

    extern GLuint textureID;

    void Init();
    void Draw(int x1, int y1, int x2, int y2, u_int col);

}


typedef void (*ScrollImageBoxCallback)(int x, int y, int width, int height, gui::HScrollBar *hScrollBar, gui::VScrollBar *vScrollBar);

class ScrollImageBox {

    int xPos,yPos;
    int width,height;

    gui::VScrollBar vScrollBar;
    gui::HScrollBar hScrollBar;

    ScrollImageBoxCallback callback;

public:

    ScrollImageBox(int x = 0, int y = 0, int w = 32, int h = 32, ScrollImageBoxCallback callback = NULL);

    void SetSize(int w, int h);
    void DoImageBox();

};


#endif // _CUSTOM_H
