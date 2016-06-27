#include "mgui.h"


namespace gui {

    KeyboardInputStruct KeyboardInput = {0};
    MouseInputStruct MouseInput = {0};

    gdl::Font* GuiCurrentFont = NULL;
    bool TextBoxActive = false;

}


void gui::SetFont(gdl::Font& font) {

    GuiCurrentFont = &font;

}


gui::Button::Button() {

    gui::Button::x=0;
    gui::Button::y=0;
    gui::Button::w=92;
    gui::Button::h=24;
    gui::Button::text = "Button";
    gui::Button::status = 0;
    gui::Button::clickedOn = false;
    gui::Button::disabled = false;
    gui::Button::callback = NULL;

}

gui::Button::Button(const char* text, int x, int y, int width, int height, ButtonCallback callback) {

    gui::Button::x = x;
    gui::Button::y = y;
    gui::Button::w = width;
    gui::Button::h = height;
    gui::Button::status = 0;
    gui::Button::clickedOn = false;
    gui::Button::text = (char*)text;
    gui::Button::disabled = false;
    gui::Button::callback = callback;

}

void gui::Button::DoButton(int id) {

    // Button click handling
    gui::Button::clicked = false;
    gui::Button::status = 0;

    if (gui::Button::disabled == false) {

        if (gui::MouseInput.Left)  {

            if ((gui::MouseInput.x >= gui::Button::x) &&
                (gui::MouseInput.x <= gui::Button::x+(gui::Button::w)) &&
                (gui::MouseInput.y >= gui::Button::y) &&
                (gui::MouseInput.y <= gui::Button::y+(gui::Button::h))
            ) {

                if (gui::MouseInput.ClickLeft)
                    gui::Button::clickedOn = true;

                if (gui::Button::clickedOn)
                    gui::Button::status = 1;

            }

        } else {

            if ((clickedOn) && (!gui::MouseInput.Left)) {

                if ((gui::MouseInput.x >= gui::Button::x) &&
                    (gui::MouseInput.x <= gui::Button::x+(gui::Button::w)) &&
                    (gui::MouseInput.y >= gui::Button::y) &&
                    (gui::MouseInput.y <= gui::Button::y+(gui::Button::h))
                ) {

                    gui::Button::clicked = true;

                    if (gui::Button::callback != NULL)
                        gui::Button::callback(id);

                    gui::Button::clickedOn = false;
                    gui::Button::status = 2;

                } else {

                    gui::Button::clickedOn = false;

                }

            }

        }

    } else {

        gui::Button::clickedOn = false;

    }

    // Draw the button itself
    int cx,cy;
    if (gui::Button::disabled == false) {

        if ((gui::Button::status == 0) || (gui::Button::status == 2))
            DrawPanel(
                gui::Button::x, gui::Button::y,
                gui::Button::x+(gui::Button::w-1), gui::Button::y+(gui::Button::h-1),
                gdl::Color::White,
                gui::PanelStyle::OutPanel
            );
        else
            DrawPanel(
                gui::Button::x, gui::Button::y,
                gui::Button::x+(gui::Button::w-1), gui::Button::y+(gui::Button::h-1),
                gdl::Color::White,
                gui::PanelStyle::InPanel
            );

        if (GuiCurrentFont != NULL) {

            cx=(gui::Button::x+(gui::Button::w/2))-(gui::GuiCurrentFont->CalcStrLen(text)/2);
            cy=(gui::Button::y+(gui::Button::h/2))-(gui::GuiCurrentFont->GetCharHeight()/2);

            if (gui::Button::status == 1) {
                cx++;
                cy++;
            }

            gui::GuiCurrentFont->DrawText(text, cx, cy, 1, gdl::Color::Black);

        }


    } else {

        DrawPanel(
            gui::Button::x, gui::Button::y,
            gui::Button::x+(gui::Button::w-1), gui::Button::y+(gui::Button::h-1),
            gdl::Color::White,
            gui::PanelStyle::OutPanel
        );

        cx=(gui::Button::x+(gui::Button::w/2))-(gui::GuiCurrentFont->CalcStrLen(text)/2);
        cy=(gui::Button::y+(gui::Button::h/2))-(gui::GuiCurrentFont->GetCharHeight()/2);

        gui::GuiCurrentFont->DrawText(text, cx, cy, 1, 0xff848484);

    }

}

void gui::Button::SetPos(int x, int y) {

    gui::Button::x = x;
    gui::Button::y = y;
}

void gui::Button::SetSize(int w, int h) {

    gui::Button::w = w;
    gui::Button::h = h;

}


gui::VScrollBar::VScrollBar() {

    gui::VScrollBar::x = 0;
    gui::VScrollBar::y = 0;
    gui::VScrollBar::w = 120;
    gui::VScrollBar::h = 12;
    gui::VScrollBar::clickedOn = false;
    gui::VScrollBar::minVal = 0;
    gui::VScrollBar::maxVal = 100;
    gui::VScrollBar::stepVal = 1;
    gui::VScrollBar::value = 0;
    gui::VScrollBar::disabled = false;

}

gui::VScrollBar::VScrollBar(int minVal, int maxVal, int x, int y, int w, int h, int step) {

    gui::VScrollBar::x=x;
    gui::VScrollBar::y=y;
    gui::VScrollBar::w=w;
    gui::VScrollBar::h=h;
    gui::VScrollBar::status = 0;
    gui::VScrollBar::clickedOn = false;
    gui::VScrollBar::minVal = minVal;
    gui::VScrollBar::maxVal = maxVal;
    gui::VScrollBar::stepVal = step;
    gui::VScrollBar::value = 0;
    gui::VScrollBar::disabled = false;

}

void gui::VScrollBar::DoScrollBar() {

    int bx1=gui::VScrollBar::x;
    int by1=gui::VScrollBar::y;
    int bx2=gui::VScrollBar::x+(gui::VScrollBar::w-1);
    int by2=gui::VScrollBar::y+(gui::VScrollBar::h-1);

    int lArrowX = x+((gui::VScrollBar::w)/2);
    int lArrowY = by1+3;
    int rArrowX = x+((gui::VScrollBar::w)/2);
    int rArrowY = by2-3;

    status = 0;

    if ((gui::MouseInput.Left) && (gui::VScrollBar::disabled == false)) {

        if (InBox(gui::MouseInput.x, gui::MouseInput.y, bx1, by1, bx2, by2)) {

            if (gui::MouseInput.ClickLeft)
                clickedOn = true;

            if (clickedOn) {

                if (InBox(gui::MouseInput.x, gui::MouseInput.y, bx1, by1, bx2, by1+10)) {

                    if (barClicked == false) {
                        if (value > minVal) value -= stepVal;
                        if (value < minVal) value = minVal;
                        status = 3;
                    }

                } else if (InBox(gui::MouseInput.x, gui::MouseInput.y, bx1, by2-10, bx2, by2)) {

                    if (barClicked == false) {
                        if (value < maxVal) value += stepVal;
                        if (value > maxVal) value = maxVal;
                        status = 4;
                    }

                } else {

                    status = 1;

                }

            }

        }

    } else {

        if (gui::MouseInput.ClickLeft == false) {

            clickedOn = false;

        }

        barClicked = false;

    }


    // Calculate scrollbar size and position
    int sliderRange = maxVal-minVal;
    int sliderYsize = ((gui::VScrollBar::h-1)-20)-sliderRange;
    int sliderYpos;

    if (sliderYsize < 16) {

        sliderYsize = 16;
        sliderYpos = (by1+10)+((sliderRange*(((float)gui::VScrollBar::h-37)/sliderRange))*(((float)value-minVal)/sliderRange));

    } else {

        sliderYpos = (by1+10)+((gui::VScrollBar::h-21)-sliderYsize)*(((float)value-minVal)/sliderRange);

    }

    if (gui::VScrollBar::disabled == false) {

        if (barClicked) {

            sliderYpos = (by1+10)+(gdl::Mouse::MouseInfo.y-clickPos);

            if (sliderYpos < (by1+10)) sliderYpos = by1+10;
            if (sliderYpos > ((by2-10)-sliderYsize)) sliderYpos = (by2-10)-sliderYsize;

            value = minVal + (sliderRange*(((float)sliderYpos-(by1+10))/((gui::VScrollBar::h-22)-sliderYsize)));

            if (value < minVal) value = minVal;
            if (value > maxVal) value = maxVal;

        }


        if ((status == 1) && (clickedOn == true)) {

            if (InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y, bx1, sliderYpos, bx2, sliderYpos+sliderYsize)) {
                clickPos = (by1+10)+(gdl::Mouse::MouseInfo.y-sliderYpos);
                barClicked = true;
            }

        }

    } else {

        status = 0;
        barClicked = false;

    }


    if (gui::VScrollBar::disabled == false) {

        if (status == 3) {
            DrawPanel(bx1, by1, bx2, by1+9, gdl::Color::White, gui::PanelStyle::InPanel);
        } else {
            DrawPanel(bx1, by1, bx2, by1+9, gdl::Color::White, gui::PanelStyle::OutPanel);
        }

        if (status == 4) {
            DrawPanel(bx1, by2-9, bx2, by2, gdl::Color::White, gui::PanelStyle::InPanel);
        } else {
            DrawPanel(bx1, by2-9, bx2, by2, gdl::Color::White, gui::PanelStyle::OutPanel);
        }

    } else {

        gdl::DrawBox(bx1, by1, bx2, by1+9, 0xff8f8f8f);
        gdl::DrawBox(bx1, by2-9, bx2, by2, 0xff8f8f8f);

    }


    struct vert {
        int x,y;
        u_int col;
    } verts[8];


    u_int col;
    if (gui::VScrollBar::disabled)
        col = 0xff8f8f8f;
    else
        col = gdl::Color::Black;

    verts[0] = (vert){ lArrowX-2    , lArrowY+2 , col };
    verts[1] = (vert){ lArrowX      , lArrowY   , col };
    verts[2] = (vert){ lArrowX      , lArrowY   , col };
    verts[3] = (vert){ lArrowX+3    , lArrowY+3 , col };

    verts[4] = (vert){ rArrowX-2    , rArrowY-2 , col };
    verts[5] = (vert){ rArrowX      , rArrowY   , col };
    verts[6] = (vert){ rArrowX      , rArrowY   , col };
    verts[7] = (vert){ rArrowX+3    , rArrowY-2 , col };

    if (status == 3) {
        verts[0].x++; verts[0].y++;
        verts[1].x++; verts[1].y++;
        verts[2].x++; verts[2].y++;
        verts[3].x++; verts[3].y++;
    } else if (status == 4) {
        verts[4].x++; verts[4].y++;
        verts[5].x++; verts[5].y++;
        verts[6].x++; verts[6].y++;
        verts[7].x++; verts[7].y++;
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    if (gdl::pc::glCompatMode == false)
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(2, GL_INT, sizeof(vert), &verts[0].x);
    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(vert), &verts[0].col);

    glDrawArrays(GL_LINES, 0, 8);


    if (gui::VScrollBar::disabled == false) {

        gui::DrawPanel(bx1, by1+10, bx2, by2-10, gdl::Color::White, gui::PanelStyle::Slider);

        if (sliderRange != 0) {
            if (barClicked) {
                DrawPanel(bx1, sliderYpos, bx2, sliderYpos+sliderYsize, gdl::Color::White, gui::PanelStyle::InPanel);
            } else {
                DrawPanel(bx1, sliderYpos, bx2, sliderYpos+sliderYsize, gdl::Color::White, gui::PanelStyle::OutPanel);
            }
        }

    } else {

        gdl::DrawBox(bx1, by1+10, bx2, by2-10, 0xff8f8f8f);
        //gdl::DrawBox(bx1, sliderYpos, bx2, sliderYpos+sliderYsize, 0xff8f8f8f);

    }

}

void gui::VScrollBar::SetMinMaxVal(int minVal, int maxVal) {

    gui::VScrollBar::minVal = minVal;
    gui::VScrollBar::maxVal = maxVal;

}

void gui::VScrollBar::SetPos(int x, int y) {

    gui::VScrollBar::x = x;
    gui::VScrollBar::y = y;

}

void gui::VScrollBar::SetSize(int w, int h) {

    gui::VScrollBar::w = w;
    gui::VScrollBar::h = h;

}


gui::HScrollBar::HScrollBar() {

    gui::HScrollBar::x = 0;
    gui::HScrollBar::y = 0;
    gui::HScrollBar::w = 120;
    gui::HScrollBar::h = 12;
    gui::HScrollBar::clickedOn = false;
    gui::HScrollBar::minVal = 0;
    gui::HScrollBar::maxVal = 100;
    gui::HScrollBar::value = 0;
    gui::HScrollBar::disabled = false;

}

gui::HScrollBar::HScrollBar(int minVal, int maxVal, int x, int y, int w, int h, int step) {

    gui::HScrollBar::x=x;
    gui::HScrollBar::y=y;
    gui::HScrollBar::w=w;
    gui::HScrollBar::h=h;
    gui::HScrollBar::status = 0;
    gui::HScrollBar::clickedOn = false;
    gui::HScrollBar::minVal = minVal;
    gui::HScrollBar::maxVal = maxVal;
    gui::HScrollBar::value = 0;
    gui::HScrollBar::disabled = false;

}

void gui::HScrollBar::DoScrollBar() {

    int bx1=gui::HScrollBar::x;
    int by1=gui::HScrollBar::y;
    int bx2=gui::HScrollBar::x+(gui::HScrollBar::w-1);
    int by2=gui::HScrollBar::y+(gui::HScrollBar::h-1);

    int lArrowX = bx1+3;
    int lArrowY = y+((gui::HScrollBar::h)/2);
    int rArrowX = bx2-3;
    int rArrowY = y+((gui::HScrollBar::h)/2);

    status = 0;


    if ((gui::MouseInput.Left) && (gui::HScrollBar::disabled == false)) {

        if (InBox(gui::MouseInput.x, gui::MouseInput.y, bx1, by1, bx2, by2)) {

            if (gui::MouseInput.ClickLeft)
                clickedOn = true;

            if (clickedOn) {

                if (InBox(gui::MouseInput.x, gui::MouseInput.y, bx1, by1, bx1+10, by2)) {

                    if (barClicked == false) {
                        if (value > minVal) value -= 1;
                        status = 3;
                    }

                } else if (InBox(gui::MouseInput.x, gui::MouseInput.y, bx2-10, by1, bx2, by2)) {

                    if (barClicked == false) {
                        if (value < maxVal) value += 1;
                        status = 4;
                    }

                } else {

                    status = 1;

                }
            }

        }

    } else {

        if (!gui::MouseInput.ClickLeft)
            clickedOn = false;

        barClicked = false;

    }


    // Calculate scrollbar size and position
    int sliderRange = (maxVal-minVal);
    int sliderXsize = ((gui::HScrollBar::w-1)-20)-sliderRange;
    int sliderXpos;


    if (sliderXsize < 16) {

        sliderXsize = 16;
        sliderXpos = (bx1+10)+((sliderRange * (((float)gui::HScrollBar::w-37)/sliderRange))*(((float)value-minVal)/sliderRange));

    } else {

        sliderXpos = (bx1+10)+((gui::HScrollBar::w-21)-sliderXsize)*(((float)value-minVal)/sliderRange);

    }


    if (barClicked) {

        sliderXpos = (bx1+10)+(gui::MouseInput.x-clickPos);
        if (sliderXpos < (bx1+10)) sliderXpos = bx1+10;
        if (sliderXpos > ((bx2-10)-sliderXsize)) sliderXpos = (bx2-10)-sliderXsize;

        value = minVal + (sliderRange*(((float)sliderXpos-(bx1+10))/((gui::HScrollBar::w-23)-sliderXsize)));

        if (value < minVal) value = minVal;
        if (value > maxVal) value = maxVal;

    }


    if ((status == 1) && (clickedOn == true)) {

        if (InBox(gui::MouseInput.x, gui::MouseInput.y, sliderXpos, by1, sliderXpos+sliderXsize, by2)) {

            clickPos = (bx1+10)+(gui::MouseInput.x-sliderXpos);
            barClicked = true;

        }

    }


    if (gui::HScrollBar::disabled == false) {

        if (status == 3) {
            DrawPanel(bx1, by1, bx1+9, by2, gdl::Color::White, gui::PanelStyle::InPanel);
        } else {
            DrawPanel(bx1, by1, bx1+9, by2, gdl::Color::White, gui::PanelStyle::OutPanel);
        }

        if (status == 4) {
            DrawPanel(bx2-9, by1, bx2, by2, gdl::Color::White, gui::PanelStyle::InPanel);
        } else {
            DrawPanel(bx2-9, by1, bx2, by2, gdl::Color::White, gui::PanelStyle::OutPanel);
        }

    } else {

        gdl::DrawBox(bx1, by1, bx1+9, by2, 0xff8f8f8f);
        gdl::DrawBox(bx2-9, by1, bx2, by2, 0xff8f8f8f);

    }


    struct vert {
        int x,y;
        u_int col;
    } verts[8];

    u_int col;
    if (gui::HScrollBar::disabled == false)
        col = gdl::Color::Black;
    else
        col = 0xff8f8f8f;

    verts[0] = (vert){ lArrowX+2    , lArrowY-2 , col };
    verts[1] = (vert){ lArrowX      , lArrowY   , col };
    verts[2] = (vert){ lArrowX      , lArrowY   , col };
    verts[3] = (vert){ lArrowX+3    , lArrowY+3 , col };

    verts[4] = (vert){ rArrowX-2    , rArrowY-2 , col };
    verts[5] = (vert){ rArrowX      , rArrowY   , col };
    verts[6] = (vert){ rArrowX      , rArrowY   , col };
    verts[7] = (vert){ rArrowX-3    , rArrowY+3 , col };

    if (status == 3) {
        verts[0].x++; verts[0].y++;
        verts[1].x++; verts[1].y++;
        verts[2].x++; verts[2].y++;
        verts[3].x++; verts[3].y++;
    } else if (status == 4) {
        verts[4].x++; verts[4].y++;
        verts[5].x++; verts[5].y++;
        verts[6].x++; verts[6].y++;
        verts[7].x++; verts[7].y++;
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    if (gdl::pc::glCompatMode == false)
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(2, GL_INT, sizeof(vert), &verts[0].x);
    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(vert), &verts[0].col);

    glDrawArrays(GL_LINES, 0, 8);



    if (gui::HScrollBar::disabled == false) {

        gui::DrawPanel(bx1+10, by1, bx2-10, by2, gdl::Color::White, gui::PanelStyle::Slider);

        if (barClicked) {
            gui::DrawPanel(sliderXpos, by1, sliderXpos+sliderXsize, by2, gdl::Color::White, gui::PanelStyle::InPanel);
        } else {
            gui::DrawPanel(sliderXpos, by1, sliderXpos+sliderXsize, by2, gdl::Color::White, gui::PanelStyle::OutPanel);
        }

    } else {

        gdl::DrawBox(bx1+10, by1, bx2-10, by2, 0xff8f8f8f);
        //gdl::DrawBox(sliderXpos, by1, sliderXpos+sliderXsize, by2, 0xff8f8f8f);

    }

}

void gui::HScrollBar::SetMinMaxVal(int minVal, int maxVal) {

    gui::HScrollBar::minVal = minVal;
    gui::HScrollBar::maxVal = maxVal;

}

void gui::HScrollBar::SetPos(int x, int y) {

    gui::HScrollBar::x = x;
    gui::HScrollBar::y = y;

}

void gui::HScrollBar::SetSize(int w, int h) {

    gui::HScrollBar::w = w;
    gui::HScrollBar::h = h;

}


gui::ListBox::ListBox(int x, int y, int w, int h, gui::ListBoxCallback callback, bool keepSelect) {

    gui::ListBox::x = x;
    gui::ListBox::y = y;
    gui::ListBox::w = w;
    gui::ListBox::h = h;
    gui::ListBox::numItems = 0;
    gui::ListBox::allocItems = 1;
    gui::ListBox::item = (char**)malloc(sizeof(char*));
    gui::ListBox::item[0] = NULL;
    gui::ListBox::clickedOn = false;
    gui::ListBox::status = 0;
    gui::ListBox::keepSelect = keepSelect;

    gui::ListBox::selItem = -1;
    gui::ListBox::scrollPos = 0;

    gui::ListBox::callback = callback;

    gui::ListBox::vScrollBar.SetPos((x+(gui::ListBox::w-1)-16), y);
    gui::ListBox::vScrollBar.SetSize(17, h);
    gui::ListBox::vScrollBar.SetMinMaxVal(0, 0);


}

void gui::ListBox::AddItem(const char* text, int index) {

    if (gui::ListBox::numItems >= gui::ListBox::allocItems) {
        gui::ListBox::item = (char**)realloc(gui::ListBox::item, sizeof(char*)*(gui::ListBox::numItems+1));
        gui::ListBox::allocItems++;
    }

    gui::ListBox::item[gui::ListBox::numItems] = strdup(text);

    gui::ListBox::numItems += 1;

}

void gui::ListBox::DeleteItem(int index) {

    free(gui::ListBox::item[index]);

    for(int i=index; i<ListBox::numItems-1; i++) {
        gui::ListBox::item[index] = gui::ListBox::item[index+1];
    }

    ListBox::numItems--;

}

void gui::ListBox::DoListBox() {


    bool scrollable = true;

    if (((gui::GuiCurrentFont->GetCharHeight()+3)*gui::ListBox::numItems) < (gui::ListBox::h-3))
        scrollable = false;


    gui::DrawPanel(
        gui::ListBox::x, gui::ListBox::y,
        gui::ListBox::x+((gui::ListBox::w-1)-19), gui::ListBox::y+(gui::ListBox::h-1),
        gdl::Color::White, gui::PanelStyle::TextBox
    );


    gui::ListBox::vScrollBar.DoScrollBar();
    gui::ListBox::scrollPos = gui::ListBox::vScrollBar.value;


    if ((gui::MouseInput.ClickLeft) && (gui::ListBox::numItems)) {

        if (gui::InBox(gui::MouseInput.x, gui::MouseInput.y, gui::ListBox::x+1, gui::ListBox::y+1, gui::ListBox::x+((gui::ListBox::w-1)-20), gui::ListBox::y+(gui::ListBox::h-2))) {

            gui::ListBox::clickedOn = true;
            gui::ListBox::status = 1;

        } else {

            if (gui::ListBox::vScrollBar.status == 0)
                gui::ListBox::clickedOn = false;

            gui::ListBox::status = 0;

            if (!gui::ListBox::keepSelect)
                gui::ListBox::selItem = -1;

        }

    }

    if ((gui::ListBox::clickedOn) && (scrollable)) {

        gui::ListBox::scrollPos -= ((gui::GuiCurrentFont->GetCharHeight()+3)/2)*gui::MouseInput.Vscroll;

        if (gui::ListBox::scrollPos < 0)
            gui::ListBox::scrollPos = 0;
        if (gui::ListBox::scrollPos > (((gui::GuiCurrentFont->GetCharHeight()+3)*gui::ListBox::numItems)-(gui::ListBox::h-1)))
            gui::ListBox::scrollPos = ((gui::GuiCurrentFont->GetCharHeight()+3)*gui::ListBox::numItems)-(gui::ListBox::h-1);

        gui::ListBox::vScrollBar.value = gui::ListBox::scrollPos;

    }


    if ((gui::ListBox::clickedOn) && (gui::MouseInput.Left)) {

        if (gui::ListBox::status == 1) {

            int sel = ((gui::MouseInput.y-(gui::ListBox::y+1))+gui::ListBox::scrollPos)/(gui::GuiCurrentFont->GetCharHeight()+3);

            if (sel < gui::ListBox::numItems) {

                gui::ListBox::selItem = sel;

                if (gui::ListBox::selItem < 0)
                    gui::ListBox::selItem = 0;
                if (gui::ListBox::selItem > (gui::ListBox::numItems-1))
                    gui::ListBox::selItem = gui::ListBox::numItems-1;

            }

            if (gui::ListBox::callback != NULL)
                gui::ListBox::callback(gui::ListBox::selItem);

        }

    }


    if (gui::ListBox::numItems == 0) {
        gui::ListBox::selItem = -1;
    }


    if (scrollable == false) {

        gui::ListBox::scrollPos = 0;
        gui::ListBox::vScrollBar.disabled = true;
        gui::ListBox::vScrollBar.SetMinMaxVal(0, 0);

    } else {

        gui::ListBox::vScrollBar.disabled = false;
        gui::ListBox::vScrollBar.SetMinMaxVal(0, ((gui::GuiCurrentFont->GetCharHeight()+3)*gui::ListBox::numItems)-(gui::ListBox::h-1));

    }


    gdl::SetClipBox(gui::ListBox::x+1, gui::ListBox::y+1, gui::ListBox::x+((gui::ListBox::w-1)-20), gui::ListBox::y+(gui::ListBox::h-2));

    if (gui::GuiCurrentFont != NULL) {

        int ty = (gui::ListBox::y+1)-gui::ListBox::scrollPos;

        for(int i=0; i<gui::ListBox::numItems; i++) {

            if ((gui::ListBox::clickedOn) && (i == gui::ListBox::selItem)) {

                gdl::DrawBoxF(
                    gui::ListBox::x+1, ty,
                    gui::ListBox::x+(gui::ListBox::w-21), ty+(gui::GuiCurrentFont->GetCharHeight()+1),
                    gdl::Color::Blue
                );

                gui::GuiCurrentFont->Printf(gui::ListBox::x+4, ty+2, 1.f, gdl::Color::White, gui::ListBox::item[i]);

            } else {

                if ((gui::ListBox::keepSelect) && (i == gui::ListBox::selItem)) {

                    gdl::DrawBoxF(
                        gui::ListBox::x+1, ty,
                        gui::ListBox::x+(gui::ListBox::w-21), ty+(gui::GuiCurrentFont->GetCharHeight()+1),
                        0xff7f7f7f
                    );

                }

                gui::GuiCurrentFont->Printf(gui::ListBox::x+4, ty+2, 1.f, gdl::Color::Black, gui::ListBox::item[i]);

            }

            ty += (gui::GuiCurrentFont->GetCharHeight()+3);

        }

    }

    gdl::ResetClipBox();

}

void gui::ListBox::SetPos(int x, int y) {

    gui::ListBox::x = x;
    gui::ListBox::y = y;

    gui::ListBox::vScrollBar.SetPos((x+(gui::ListBox::w-1)-16), y);

}

void gui::ListBox::SetSize(int w, int h) {

    gui::ListBox::w = w;
    gui::ListBox::h = h;

    gui::ListBox::vScrollBar.SetPos((x+(gui::ListBox::w-1)-16), y);
    gui::ListBox::vScrollBar.SetSize(17, h);

}

const char* gui::ListBox::GetItem(int index) {

    if (index < 0)
        return(NULL);
    if (index > numItems)
        return(NULL);

    return(item[index]);

}

void gui::ListBox::SetItem(const char* text, int index) {

    if (index < 0)
        return;
    if (index > numItems)
        return;

    item[index] = (char*)realloc(item[index], strlen(text)+1);
    strcpy(item[index], text);

}


gui::CheckBox::CheckBox(int x, int y, const char* caption, bool ticked, bool disabled) {

    CheckBox::x = x;
    CheckBox::y = y;
    CheckBox::ticked = ticked;
    CheckBox::disabled = disabled;

    if (caption != NULL) {
        CheckBox::caption = strdup(caption);
    } else {
        CheckBox::caption = NULL;
    }

}

void gui::CheckBox::SetPos(int x, int y) {

    CheckBox::x = x;
    CheckBox::y = y;

}

void gui::CheckBox::DoCheckBox() {

    if (CheckBox::disabled == false) {

        gui::DrawPanel(CheckBox::x, CheckBox::y, CheckBox::x+10, CheckBox::y+10, gdl::Color::White, gui::PanelStyle::TextBox);

        int checkBoxWidth = 9;
        if (CheckBox::caption != NULL) {
            gui::GuiCurrentFont->DrawText(CheckBox::caption, CheckBox::x+13,CheckBox::y+2, 1.f, 0xff000000);
            checkBoxWidth += 3+gui::GuiCurrentFont->CalcStrLen(CheckBox::caption);
        }


        if (gui::MouseInput.ClickLeft) {

            if (gui::InBox(gui::MouseInput.x, gui::MouseInput.y, CheckBox::x+1, CheckBox::y+1, CheckBox::x+checkBoxWidth, CheckBox::y+9)) {

                CheckBox::status = 1;
                CheckBox::clickedOn = true;

            }

        } else {

            if ((gui::MouseInput.Left) && (CheckBox::clickedOn)) {

                if (gui::InBox(gui::MouseInput.x, gui::MouseInput.y, CheckBox::x+1, CheckBox::y+1, CheckBox::x+checkBoxWidth, CheckBox::y+9)) {

                    CheckBox::status = 1;

                } else {

                    CheckBox::status = 0;

                }

            } else {

                if ((CheckBox::status) && (CheckBox::clickedOn)) {
                    CheckBox::ticked ^= 1;
                }

                CheckBox::clickedOn = false;
                CheckBox::status = 0;

            }

        }

    } else {

        gdl::DrawBox(CheckBox::x, CheckBox::y, CheckBox::x+10, CheckBox::y+10, 0xff8f8f8f);
        if (CheckBox::caption != NULL) gui::GuiCurrentFont->DrawText(CheckBox::caption, CheckBox::x+13,CheckBox::y+2, 1.f, 0xff8f8f8f);
        CheckBox::status = 0;
        CheckBox::clickedOn = false;

    }


    // Draw check mark
    if (CheckBox::ticked) {

        struct vert {
            int x,y;
            u_int col;
        } verts[8];

        u_int col;
        if (CheckBox::disabled == false)
            col = gdl::Color::Black;
        else
            col = 0xff8f8f8f;

        verts[0] = (vert){ CheckBox::x+2, CheckBox::y+5, col };
        verts[1] = (vert){ CheckBox::x+4, CheckBox::y+7, col };
        verts[2] = (vert){ CheckBox::x+4, CheckBox::y+7, col };
        verts[3] = (vert){ CheckBox::x+9, CheckBox::y+2, col };

        verts[4] = (vert){ CheckBox::x+2, CheckBox::y+6, col };
        verts[5] = (vert){ CheckBox::x+4, CheckBox::y+8, col };
        verts[6] = (vert){ CheckBox::x+4, CheckBox::y+8, col };
        verts[7] = (vert){ CheckBox::x+9, CheckBox::y+3, col };

        if (CheckBox::status) {
            verts[0].x++; verts[0].y++;
            verts[1].x++; verts[1].y++;
            verts[2].x++; verts[2].y++;
            verts[3].x++; verts[3].y++;
            verts[4].x++; verts[4].y++;
            verts[5].x++; verts[5].y++;
            verts[6].x++; verts[6].y++;
            verts[7].x++; verts[7].y++;
        }

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);

        if (gdl::pc::glCompatMode == false)
            glBindBuffer(GL_ARRAY_BUFFER, 0);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);

        glVertexPointer(2, GL_INT, sizeof(vert), &verts[0].x);
        glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(vert), &verts[0].col);

        glDrawArrays(GL_LINES, 0, 8);

    }

}


gui::TextBox::TextBox(int x, int y, int w, int h, char* textBuff, int textBuffSize, bool disabled) {

    gui::TextBox::x = x;
    gui::TextBox::y = y;
    gui::TextBox::w = w;
    gui::TextBox::h = h;

    gui::TextBox::status = 0;
    gui::TextBox::disabled = disabled;
    gui::TextBox::clickedOn = false;

    gui::TextBox::textBuff = textBuff;
    gui::TextBox::textBuffSize = textBuffSize;

}

void gui::TextBox::SetPos(int x, int y) {

    gui::TextBox::x = x;
    gui::TextBox::y = y;

}

void gui::TextBox::SetSize(int w, int h) {

    gui::TextBox::w = w;
    gui::TextBox::h = h;

}

void gui::TextBox::SetTextBuff(char *textBuff, int textBuffSize) {

    gui::TextBox::textBuff = textBuff;
    gui::TextBox::textBuffSize = textBuffSize;

}

void gui::TextBox::DoTextBox() {

    int x2=x+(w-1);
    int y2=y+(h-1);

    if (gui::TextBox::disabled == false) {

        if (gui::MouseInput.ClickLeft) {

            if (gui::InBox(gui::MouseInput.x, gui::MouseInput.y, x+1, y+1, x2-1, y2-1)) {

                gui::TextBox::status = 1;
                gui::TextBox::clickedOn = true;

            } else {

                gui::TextBox::status = 0;
                gui::TextBox::clickedOn = false;

            }

        }

        if ((gui::TextBox::textBuff != NULL) && (gui::TextBox::clickedOn)) {

            if (gui::KeyboardInput.keyHit) {

                if (gui::KeyboardInput.keyCode == gdl::Keyboard::KEY_BACKSPACE) {

                    if (strlen(gui::TextBox::textBuff) > 0)
                        gui::TextBox::textBuff[strlen(gui::TextBox::textBuff)-1] = 0x00;

                } else if (gui::KeyboardInput.keyCode == gdl::Keyboard::KEY_ENTER) {

                    clickedOn = false;

                } else {

                    if (strlen(gui::TextBox::textBuff) < (size_t)(gui::TextBox::textBuffSize-1)) {
                        char c = gdl::Keyboard::Key.Char;
                        strncat(gui::TextBox::textBuff, &c, 1);
                    }

                }

            }

        }

        gui::DrawPanel(x, y, x2, y2, gdl::Color::White, gui::PanelStyle::TextBox);

        gdl::SetClipBox(x+1, y+1, x2-1, y2-1);
        if (gui::TextBox::textBuff != NULL)
            gui::GuiCurrentFont->DrawText(gui::TextBox::textBuff, x+2, y+3, 1.f, gdl::Color::Black);

        if (gui::TextBox::clickedOn) {
            int c = (x+3)+gui::GuiCurrentFont->CalcStrLen(gui::TextBox::textBuff);
            gdl::DrawLine(c, y+2, c, y2-2, gdl::Color::Black);
            gui::TextBoxActive = true;
        }

        gdl::ResetClipBox();

    } else {

        gdl::DrawBox(x, y, x2, y2, 0xff8f8f8f);
        gdl::SetClipBox(x+1, y+1, x2-1, y2-1);

        if (gui::TextBox::textBuff != NULL)
            gui::GuiCurrentFont->DrawText(gui::TextBox::textBuff, x+2, y+3, 1.f, 0xff7f7f7f);

        gdl::ResetClipBox();

    }

}


void gui::DrawPanel(int x1, int y1, int x2, int y2, int col, short style) {

    float rv = (float)RED(col)/255;
    float gv = (float)GREEN(col)/255;
    float bv = (float)BLUE(col)/255;

    u_int pal[] = {
        RGBA(234*rv, 234*gv, 230*bv, 255),   // Top-left shine
        RGBA(155*rv, 155*gv, 151*bv, 255),   // Bottom-right shine
        RGBA(198*rv, 198*gv, 194*bv, 255),   // Fill
        RGBA(254*rv, 254*gv, 250*bv, 255),   // Top-left corner
        RGBA(168*rv, 168*gv, 168*bv, 255),   // Bottom-left, top-right corners
        RGBA(122*rv, 122*gv, 118*bv, 255),   // Bottom-right corner
        RGBA(255*rv, 255*gv, 255*bv, 255),   // Text box fill
        RGBA(208*rv, 208*gv, 204*bv, 255),   // Fill
    };

    /*
    u_int pal[] = {
        RGBA(204*rv, 204*gv, 204*bv, 255),   // Top-left shine
        RGBA(135*rv, 135*gv, 135*bv, 255),   // Bottom-right shine
        RGBA(168*rv, 168*gv, 168*bv, 255),   // Fill
        RGBA(231*rv, 231*gv, 231*bv, 255),   // Top-left corner
        RGBA(168*rv, 168*gv, 168*bv, 255),   // Bottom-left, top-right corners
        RGBA(102*rv, 102*gv, 102*bv, 255),   // Bottom-right corner
        RGBA(181*rv, 181*gv, 181*bv, 255),
    };
    */

    struct vert {
        int x,y;
        u_int col;
    } verts[8];


    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    if (gdl::pc::glCompatMode == false)
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);


    glVertexPointer(2, GL_INT, sizeof(vert), &verts[0].x);
    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(vert), &verts[0].col);


    switch(style) {
    case gui::PanelStyle::OutPanel: // Outward button

        // Outer line of panel
        verts[0] = (vert){ x1, y2, pal[0] };
        verts[1] = (vert){ x1, y1, pal[0] };
        verts[2] = (vert){ x1, y1, pal[0] };
        verts[3] = (vert){ x2, y1, pal[0] };
        verts[4] = (vert){ x2, y1, pal[1] };
        verts[5] = (vert){ x2, y2, pal[1] };
        verts[6] = (vert){ x2, y2, pal[1] };
        verts[7] = (vert){ x1, y2, pal[1] };
        glDrawArrays(GL_LINES, 0, 8);

        // Panel fill
        verts[0] = (vert){ x1+1 , y1+1  , pal[2] };
        verts[1] = (vert){ x2   , y1+1  , pal[2] };
        verts[2] = (vert){ x2   , y2    , pal[2] };
        verts[3] = (vert){ x1+1 , y2    , pal[2] };
        glDrawArrays(GL_QUADS, 0, 4);

        // Corners
        verts[0] = (vert){ x1   , y1    , pal[3] };
        verts[1] = (vert){ x2   , y1    , pal[4] };
        verts[2] = (vert){ x1   , y2    , pal[4] };
        verts[3] = (vert){ x2   , y2    , pal[5] };
        glDrawArrays(GL_POINTS, 0, 4);

        break;

    case gui::PanelStyle::InPanel:  // Inward button

        // Outer line of panel
        verts[0] = (vert){ x1, y2, pal[1] };
        verts[1] = (vert){ x1, y1, pal[1] };
        verts[2] = (vert){ x1, y1, pal[1] };
        verts[3] = (vert){ x2, y1, pal[1] };
        verts[4] = (vert){ x2, y1, pal[0] };
        verts[5] = (vert){ x2, y2, pal[0] };
        verts[6] = (vert){ x2, y2, pal[0] };
        verts[7] = (vert){ x1, y2, pal[0] };
        glDrawArrays(GL_LINES, 0, 8);

        // Panel fill
        verts[0] = (vert){ x1+1 , y1+1  , pal[2] };
        verts[1] = (vert){ x2   , y1+1  , pal[2] };
        verts[2] = (vert){ x2   , y2    , pal[2] };
        verts[3] = (vert){ x1+1 , y2    , pal[2] };
        glDrawArrays(GL_QUADS, 0, 4);

        // Corners
        verts[0] = (vert){ x1   , y1    , pal[5] };
        verts[1] = (vert){ x2   , y1    , pal[4] };
        verts[2] = (vert){ x1   , y2    , pal[4] };
        verts[3] = (vert){ x2   , y2    , pal[3] };
        glDrawArrays(GL_POINTS, 0, 4);

        break;

    case gui::PanelStyle::TextBox:

        // Outer line of panel
        verts[0] = (vert){ x1, y2, pal[1] };
        verts[1] = (vert){ x1, y1, pal[1] };
        verts[2] = (vert){ x1, y1, pal[1] };
        verts[3] = (vert){ x2, y1, pal[1] };
        verts[4] = (vert){ x2, y1, pal[0] };
        verts[5] = (vert){ x2, y2, pal[0] };
        verts[6] = (vert){ x2, y2, pal[0] };
        verts[7] = (vert){ x1, y2, pal[0] };
        glDrawArrays(GL_LINES, 0, 8);

        // Panel fill
        verts[0] = (vert){ x1+1 , y1+1  , pal[6] };
        verts[1] = (vert){ x2   , y1+1  , pal[6] };
        verts[2] = (vert){ x2   , y2    , pal[6] };
        verts[3] = (vert){ x1+1 , y2    , pal[6] };
        glDrawArrays(GL_QUADS, 0, 4);

        // Corners
        verts[0] = (vert){ x1   , y1    , pal[5] };
        verts[1] = (vert){ x2   , y1    , pal[4] };
        verts[2] = (vert){ x1   , y2    , pal[4] };
        verts[3] = (vert){ x2   , y2    , pal[3] };
        glDrawArrays(GL_POINTS, 0, 4);

        break;

    case gui::PanelStyle::InFrame:

        // Outer line of panel
        verts[0] = (vert){ x1, y2, pal[1] };
        verts[1] = (vert){ x1, y1, pal[1] };
        verts[2] = (vert){ x1, y1, pal[1] };
        verts[3] = (vert){ x2, y1, pal[1] };
        verts[4] = (vert){ x2, y1, pal[0] };
        verts[5] = (vert){ x2, y2, pal[0] };
        verts[6] = (vert){ x2, y2, pal[0] };
        verts[7] = (vert){ x1, y2, pal[0] };
        glDrawArrays(GL_LINES, 0, 8);

        // Corners
        verts[0] = (vert){ x1   , y1    , pal[5] };
        verts[1] = (vert){ x2   , y1    , pal[4] };
        verts[2] = (vert){ x1   , y2    , pal[4] };
        verts[3] = (vert){ x2   , y2    , pal[3] };
        glDrawArrays(GL_POINTS, 0, 4);

        break;

    case gui::PanelStyle::Slider:

        // Outer line of panel
        verts[0] = (vert){ x1, y2, pal[1] };
        verts[1] = (vert){ x1, y1, pal[1] };
        verts[2] = (vert){ x1, y1, pal[1] };
        verts[3] = (vert){ x2, y1, pal[1] };
        verts[4] = (vert){ x2, y1, pal[0] };
        verts[5] = (vert){ x2, y2, pal[0] };
        verts[6] = (vert){ x2, y2, pal[0] };
        verts[7] = (vert){ x1, y2, pal[0] };
        glDrawArrays(GL_LINES, 0, 8);

        // Panel fill
        verts[0] = (vert){ x1+1 , y1+1  , pal[7] };
        verts[1] = (vert){ x2   , y1+1  , pal[7] };
        verts[2] = (vert){ x2   , y2    , pal[7] };
        verts[3] = (vert){ x1+1 , y2    , pal[7] };
        glDrawArrays(GL_QUADS, 0, 4);

        // Corners
        verts[0] = (vert){ x1   , y1    , pal[5] };
        verts[1] = (vert){ x2   , y1    , pal[4] };
        verts[2] = (vert){ x1   , y2    , pal[4] };
        verts[3] = (vert){ x2   , y2    , pal[3] };
        glDrawArrays(GL_POINTS, 0, 4);

    }

}

bool gui::InBox(int x, int y, int x1, int y1, int x2, int y2) {

    if ((x >= x1) && (x <= x2)) {
        if ((y >= y1) && (y <= y2)) {
            return(true);
        }
    }

    return(false);

}
