#ifndef _MGUI_H
#define _MGUI_H

#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <mgdl-pc.h>


namespace gui {


namespace PanelStyle {
    enum {
        OutPanel = 0,
        InPanel  = 1,
        TextBox  = 2,
        Slider   = 3,
        InFrame  = 4,
    };
}

typedef struct {
    short keyChar;
    int keyCode;
    bool keyHit;
} KeyboardInputStruct;

typedef struct {
    int x;
    int y;
    float Hscroll;
    float Vscroll;
    bool Left;
    bool Right;
    bool Middle;
    bool ClickLeft;
    bool ClickRight;
    bool ClickMiddle;
} MouseInputStruct;


typedef void (*ButtonCallback)(int id);
typedef void (*ListBoxCallback)(int listEntry);

extern KeyboardInputStruct KeyboardInput;
extern MouseInputStruct MouseInput;

extern bool TextBoxActive;


void SetFont(gdl::Font& font);

void DrawPanel(int x1, int y1, int x2, int y2, int col, short style);

bool InBox(int x, int y, int x1, int y1, int x2, int y2);


class Button {

    int x,y;
    int w,h;

    const char* text;
    bool clickedOn;

    ButtonCallback callback;

public:

    int status;
    bool clicked;
    bool disabled;

    Button();
    Button(const char* text, int x, int y, int w, int h, ButtonCallback callback = NULL);

    void SetPos(int x, int y);
    void SetSize(int w, int h);
    void DoButton(int id = 0);

};


class HScrollBar {

    int x,y;
    int w,h;
    int clickPos;
    bool clickedOn;
    bool barClicked;

public:

    bool disabled;
    int status;
    int minVal;
    int maxVal;
    int value;

    HScrollBar();
    HScrollBar(int minVal, int maxVal, int x, int y, int w, int h, int step = 1);

    void DoScrollBar();

    void SetMinMaxVal(int minVal, int maxVal);
    void SetPos(int x, int y);
    void SetSize(int w, int h);

};


class VScrollBar {

    int x,y;
    int w,h;
    int stepVal;
    int clickPos;
    bool clickedOn;
    bool barClicked;

public:

    bool disabled;
    int status;
    int minVal;
    int maxVal;
    int value;

    VScrollBar();
    VScrollBar(int minVal, int maxVal, int x, int y, int w, int h, int step = 1);

    void DoScrollBar();

    void SetMinMaxVal(int minVal, int maxVal);
    void SetPos(int x, int y);
    void SetSize(int w, int h);

};


class ListBox {

    int x,y;
    int w,h;

    gui::VScrollBar vScrollBar;

    int numItems;
    int allocItems;
    char** item;

    int scrollPos;

    bool clickedOn;

    bool keepSelect;

    gui::ListBoxCallback callback;

public:

    int status;
    int selItem;

    ListBox(int x = 0, int y = 0, int w = 0, int h = 0, gui::ListBoxCallback callback = NULL, bool keepSelect = false);

    void AddItem(const char* text, int index);
    void DeleteItem(int index);

    void DoListBox();

    void SetPos(int x, int y);
    void SetSize(int w, int h);

    const char* GetItem(int index);
    void SetItem(const char* text, int index);

};


class CheckBox {

    int x,y;
    int status;
    bool clickedOn;

    const char *caption;

public:

    bool disabled;
    bool ticked;

    CheckBox(int x = 0, int y = 0, const char* caption = NULL, bool ticked = false, bool disabled = false);

    void SetPos(int x, int y);
    void DoCheckBox();

};


class TextBox {

    int x,y;
    int w,h;

    int status;
    bool clickedOn;

    char *textBuff;
    int textBuffSize;


public:

    bool disabled;

    TextBox(int x = 0, int y = 0, int w = 100, int h = 13, char* textBuff = NULL, int textBuffSize = 0, bool disabled = false);

    void SetPos(int x, int y);
    void SetSize(int w, int h);
    void SetTextBuff(char *textBuff = NULL, int textBuffSize = 0);

    void DoTextBox();

};

}

#endif // _MGUI_H
