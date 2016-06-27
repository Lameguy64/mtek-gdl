namespace mui {

    namespace PanelStyle {
        enum {
            OutButton,
            InButton,
            TextBox,
            InFrame,
        };
    }


    int     *mouseXposPtr=NULL;
    int     *mouseYposPtr=NULL;
    bool    *mouseClickPtr=NULL;
    bool    *mouseOldClickPtr=NULL;

    gdl::Font *currentFont=NULL;

    bool InBox(int x, int y, int x1, int y1, int x2, int y2) {

        if ((x >= x1) && (x <= x2)) {
            if ((y >= y1) && (y <= y2)) {
                return(true);
            }
        }

        return(false);

    }

    void DrawPanel(int x1, int y1, int x2, int y2, int col, short style);

    class Button {

        int x,y;
        int w,h;
        int col;

        const char *text;
        bool clickedOn;

    public:

        bool disabled;
        int status;
        bool clicked;

        Button();
        Button(const char* text, int x, int y, int w, int h, int col);
        virtual ~Button();

        void DoButton();
        void SetButtonPos(int x, int y);
        void SetButtonSize(int w, int h);

    };

    class HScrollBar {
        int x,y;
        int w,h;
        int col;
        int minVal;
        int maxVal;
        int clickPos;
        bool clickedOn;
        bool barClicked;
    public:

        bool disabled;
        int status;
        int value;

        HScrollBar();
        HScrollBar(int minVal, int maxVal, int x, int y, int w, int h, int col);
        virtual ~HScrollBar();

        void DoScrollBar();

        void SetPos(int x, int y) {
            mui::HScrollBar::x = x;
            mui::HScrollBar::y = y;
        }
        void SetSize(int w, int h) {
            mui::HScrollBar::w = w;
            mui::HScrollBar::h = h;
        }

    };

    class VScrollBar {
        int x,y;
        int w,h;
        int col;
        int minVal;
        int maxVal;
        int clickPos;
        bool clickedOn;
        bool barClicked;
    public:

        bool disabled;
        int status;
        int value;

        VScrollBar();
        VScrollBar(int minVal, int maxVal, int x, int y, int w, int h, int col);
        virtual ~VScrollBar();

        void DoScrollBar();

        void SetPos(int x, int y) {
            mui::VScrollBar::x = x;
            mui::VScrollBar::y = y;
        }
        void SetSize(int w, int h) {
            mui::VScrollBar::w = w;
            mui::VScrollBar::h = h;
        }

    };

}

// Button class functions
mui::Button::Button() {

    mui::Button::x=0;
    mui::Button::y=0;
    mui::Button::w=92;
    mui::Button::h=24;
    mui::Button::col = gdl::Color::White;
    mui::Button::text = "Button";
    mui::Button::status = 0;
    mui::Button::clickedOn = false;
    mui::Button::disabled = false;

}
mui::Button::Button(const char* text, int x, int y, int width, int height, int col) {

    mui::Button::x = x;
    mui::Button::y = y;
    mui::Button::w = width;
    mui::Button::h = height;
    mui::Button::col = col;
    mui::Button::status = 0;
    mui::Button::clickedOn = false;
    mui::Button::text = (char*)text;
    mui::Button::disabled = false;

}
mui::Button::~Button() {
}
void mui::Button::DoButton() {

    // Button click handling
    mui::Button::status = 0;
    mui::Button::clicked = false;

    if (mui::Button::disabled == false) {
        if ((mouseClickPtr != NULL) && (mouseXposPtr != NULL) && (mouseYposPtr != NULL)) {

            if (*mouseClickPtr)  {

                if ((*mouseXposPtr >= mui::Button::x) &&
                    (*mouseXposPtr <= mui::Button::x+(mui::Button::w)) &&
                    (*mouseYposPtr >= mui::Button::y) &&
                    (*mouseYposPtr <= mui::Button::y+(mui::Button::h))
                ) {

                    if (*mouseOldClickPtr == false) mui::Button::clickedOn = true;
                    if (mui::Button::clickedOn) mui::Button::status = 1;

                }

            } else {

                if ((clickedOn) && (*mouseOldClickPtr)) {
                    if ((*mouseXposPtr >= mui::Button::x) &&
                        (*mouseXposPtr <= mui::Button::x+(mui::Button::w)) &&
                        (*mouseYposPtr >= mui::Button::y) &&
                        (*mouseYposPtr <= mui::Button::y+(mui::Button::h))
                    ) {
                        mui::Button::status = 2;
                        mui::Button::clicked = true;
                    }
                }

                mui::Button::clickedOn = false;

            }

        }
    } else {

        mui::Button::clickedOn = false;

    }

    // Draw the button itself
    int cx,cy;
    if (mui::Button::disabled == false) {

        if (mui::Button::status == 0)
            DrawPanel(mui::Button::x, mui::Button::y, mui::Button::x+mui::Button::w, mui::Button::y+mui::Button::h, mui::Button::col, mui::PanelStyle::OutButton);
        else
            DrawPanel(mui::Button::x, mui::Button::y, mui::Button::x+mui::Button::w, mui::Button::y+mui::Button::h, mui::Button::col, mui::PanelStyle::InButton);

        if (currentFont != NULL) {

            cx=(mui::Button::x+(mui::Button::w/2))-(currentFont->CalcStrLen(text)/2);
            cy=(mui::Button::y+(mui::Button::h/2))-(currentFont->GetCharHeight()/2);

            if (mui::Button::status) {
                cx++;
                cy++;
            }

            currentFont->DrawText(text, cx, cy, 1, gdl::Color::White);

        }

    } else {

        cx=(mui::Button::x+(mui::Button::w/2))-(currentFont->CalcStrLen(text)/2);
        cy=(mui::Button::y+(mui::Button::h/2))-(currentFont->GetCharHeight()/2);

        DrawPanel(mui::Button::x, mui::Button::y, mui::Button::x+mui::Button::w, mui::Button::y+mui::Button::h, mui::Button::col, mui::PanelStyle::OutButton);
        currentFont->DrawText(text, cx, cy, 1, 0xff848484);

    }

}
void mui::Button::SetButtonPos(int x, int y) {

    mui::Button::x = x;
    mui::Button::y = y;

}
void mui::Button::SetButtonSize(int w, int h) {

    mui::Button::w = w;
    mui::Button::h = h;

}


mui::HScrollBar::HScrollBar() {

    mui::HScrollBar::x = 0;
    mui::HScrollBar::y = 0;
    mui::HScrollBar::w = 120;
    mui::HScrollBar::h = 12;
    mui::HScrollBar::col = gdl::Color::White;
    mui::HScrollBar::clickedOn = false;
    mui::HScrollBar::minVal = 0;
    mui::HScrollBar::maxVal = 100;
    mui::HScrollBar::value = 0;
    mui::HScrollBar::disabled = false;

}
mui::HScrollBar::HScrollBar(int minVal, int maxVal, int x, int y, int w, int h, int col) {

    mui::HScrollBar::x=x;
    mui::HScrollBar::y=y;
    mui::HScrollBar::w=w;
    mui::HScrollBar::h=h;
    mui::HScrollBar::col = col;
    mui::HScrollBar::status = 0;
    mui::HScrollBar::clickedOn = false;
    mui::HScrollBar::minVal = minVal;
    mui::HScrollBar::maxVal = maxVal;
    mui::HScrollBar::value = 0;
    mui::HScrollBar::disabled = false;

}
mui::HScrollBar::~HScrollBar() {
}
void mui::HScrollBar::DoScrollBar() {

    int bx1=mui::HScrollBar::x;
    int by1=mui::HScrollBar::y;
    int bx2=mui::HScrollBar::x+mui::HScrollBar::w;
    int by2=mui::HScrollBar::y+mui::HScrollBar::h;

    int lArrowX = bx1+3;
    int lArrowY = y+((mui::HScrollBar::h)/2);
    int rArrowX = bx2-3;
    int rArrowY = y+((mui::HScrollBar::h)/2);

    status = 0;
    if ((mouseClickPtr != NULL) && (mouseOldClickPtr != NULL) && (mouseXposPtr != NULL) && (mouseYposPtr != NULL)) {

        if (*mouseClickPtr) {
            if (InBox(*mouseXposPtr, *mouseYposPtr, bx1, by1, bx2, by2)) {

                if (*mouseOldClickPtr == false)
                    clickedOn = true;

                if (clickedOn) {
                    if (InBox(*mouseXposPtr, *mouseYposPtr, bx1, by1, bx1+10, by2)) {
                        if (barClicked == false) {
                            if (value > minVal) value -= 1;
                            status = 3;
                        }
                    } else if (InBox(*mouseXposPtr, *mouseYposPtr, bx2-10, by1, bx2, by2)) {
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
            if (*mouseOldClickPtr == true) clickedOn = false;
            barClicked = false;
        }

    } else {
        clickedOn = false;
        barClicked = false;
    }


    // Calculate scrollbar size and position
    int sliderRange = (maxVal-minVal);
    int sliderXsize = ((mui::HScrollBar::w-1)-20)-sliderRange;
    int sliderXpos;


    if (sliderXsize < 16) {
        sliderXsize = 16;
        sliderXpos = (bx1+10)+((sliderRange*(((float)mui::HScrollBar::w-36)/sliderRange))*(((float)value-minVal)/sliderRange));
    } else {
        sliderXpos = (bx1+10)+((mui::HScrollBar::w-20)-sliderXsize)*(((float)value-minVal)/sliderRange);
    }


    if (barClicked) {
        sliderXpos = (bx1+10)+(*mouseXposPtr-clickPos);
        if (sliderXpos < (bx1+10)) sliderXpos = bx1+10;
        if (sliderXpos > ((bx2-10)-sliderXsize)) sliderXpos = (bx2-10)-sliderXsize;
        value = minVal + (sliderRange* ( ((float)sliderXpos-(bx1+10))/ sliderRange ) );
        if (value < minVal) value = minVal;
        if (value > maxVal) value = maxVal;
    }


    if ((status == 1) && (clickedOn == true)) {
        if (InBox(*mouseXposPtr, *mouseYposPtr, sliderXpos, by1, sliderXpos+sliderXsize, by2)) {
            clickPos = (bx1+10)+(*mouseXposPtr-sliderXpos);
            barClicked = true;
        }
    }


    if (status == 3) {
        DrawPanel(bx1, by1, bx1+9, by2, mui::HScrollBar::col, mui::PanelStyle::InButton);
    } else {
        DrawPanel(bx1, by1, bx1+9, by2, mui::HScrollBar::col, mui::PanelStyle::OutButton);
    }

    if (status == 4) {
        DrawPanel(bx2-9, by1, bx2, by2, mui::HScrollBar::col, mui::PanelStyle::InButton);
    } else {
        DrawPanel(bx2-9, by1, bx2, by2, mui::HScrollBar::col, mui::PanelStyle::OutButton);
    }


    struct vert {
        int x,y;
        u_int col;
    } verts[8];

    verts[0] = (vert){ lArrowX+2    , lArrowY-2 , 0xffffffff };
    verts[1] = (vert){ lArrowX      , lArrowY   , 0xffffffff };
    verts[2] = (vert){ lArrowX      , lArrowY   , 0xffffffff };
    verts[3] = (vert){ lArrowX+3    , lArrowY+3 , 0xffffffff };

    verts[4] = (vert){ rArrowX-2    , rArrowY-2 , 0xffffffff };
    verts[5] = (vert){ rArrowX      , rArrowY   , 0xffffffff };
    verts[6] = (vert){ rArrowX      , rArrowY   , 0xffffffff };
    verts[7] = (vert){ rArrowX-3    , rArrowY+3 , 0xffffffff };

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

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(2, GL_INT, sizeof(vert), &verts[0].x);
    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(vert), &verts[0].col);

    glDrawArrays(GL_LINES, 0, 8);


    DrawPanel(bx1+10, by1, bx2-10, by2, mui::HScrollBar::col, mui::PanelStyle::TextBox);

    if (barClicked) {
        DrawPanel(sliderXpos, by1, sliderXpos+sliderXsize, by2, mui::HScrollBar::col, mui::PanelStyle::InButton);
    } else {
        DrawPanel(sliderXpos, by1, sliderXpos+sliderXsize, by2, mui::HScrollBar::col, mui::PanelStyle::OutButton);
    }

}


mui::VScrollBar::VScrollBar() {

    mui::VScrollBar::x = 0;
    mui::VScrollBar::y = 0;
    mui::VScrollBar::w = 120;
    mui::VScrollBar::h = 12;
    mui::VScrollBar::col = gdl::Color::White;
    mui::VScrollBar::clickedOn = false;
    mui::VScrollBar::minVal = 0;
    mui::VScrollBar::maxVal = 100;
    mui::VScrollBar::value = 0;
    mui::VScrollBar::disabled = false;

}
mui::VScrollBar::VScrollBar(int minVal, int maxVal, int x, int y, int w, int h, int col) {

    mui::VScrollBar::x=x;
    mui::VScrollBar::y=y;
    mui::VScrollBar::w=w;
    mui::VScrollBar::h=h;
    mui::VScrollBar::col = col;
    mui::VScrollBar::status = 0;
    mui::VScrollBar::clickedOn = false;
    mui::VScrollBar::minVal = minVal;
    mui::VScrollBar::maxVal = maxVal;
    mui::VScrollBar::value = 0;
    mui::VScrollBar::disabled = false;

}
mui::VScrollBar::~VScrollBar() {
}
void mui::VScrollBar::DoScrollBar() {

    int bx1=mui::VScrollBar::x;
    int by1=mui::VScrollBar::y;
    int bx2=mui::VScrollBar::x+mui::VScrollBar::w;
    int by2=mui::VScrollBar::y+mui::VScrollBar::h;

    int lArrowX = x+((mui::VScrollBar::w)/2);
    int lArrowY = by1+3;
    int rArrowX = x+((mui::VScrollBar::w)/2);
    int rArrowY = by2-3;

    status = 0;
    if ((mouseClickPtr != NULL) && (mouseOldClickPtr != NULL) && (mouseXposPtr != NULL) && (mouseYposPtr != NULL)) {

        if (*mouseClickPtr) {
            if (InBox(*mouseXposPtr, *mouseYposPtr, bx1, by1, bx2, by2)) {

                if (*mouseOldClickPtr == false)
                    clickedOn = true;

                if (clickedOn) {
                    if (InBox(*mouseXposPtr, *mouseYposPtr, bx1, by1, bx2, by1+10)) {
                        if (barClicked == false) {
                            if (value > minVal) value -= 1;
                            status = 3;
                        }
                    } else if (InBox(*mouseXposPtr, *mouseYposPtr, bx1, by2-10, bx2, by2)) {
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
            if (*mouseOldClickPtr == true) clickedOn = false;
            barClicked = false;
        }

    } else {

        clickedOn = false;
        barClicked = false;

    }


    // Calculate scrollbar size and position
    int sliderRange = (maxVal-minVal);
    int sliderYsize = ((mui::VScrollBar::h-1)-20)-sliderRange;
    int sliderYpos;


    if (sliderYsize < 16) {
        sliderYsize = 16;
        sliderYpos = (by1+10)+((sliderRange*(((float)mui::VScrollBar::h-36)/sliderRange))*(((float)value-minVal)/sliderRange));
    } else {
        sliderYpos = (by1+10)+((mui::VScrollBar::h-20)-sliderYsize)*(((float)value-minVal)/sliderRange);
    }


    if (barClicked) {
        sliderYpos = (by1+10)+(*mouseYposPtr-clickPos);
        if (sliderYpos < (by1+10)) sliderYpos = by1+10;
        if (sliderYpos > ((by2-10)-sliderYsize)) sliderYpos = (by2-10)-sliderYsize;
        value = minVal + (sliderRange* ( ((float)sliderYpos-(by1+10))/ sliderRange ) );
        if (value < minVal) value = minVal;
        if (value > maxVal) value = maxVal;
    }


    if ((status == 1) && (clickedOn == true)) {
        if (InBox(*mouseXposPtr, *mouseYposPtr, bx1, sliderYpos, bx2, sliderYpos+sliderYsize)) {
            clickPos = (by1+10)+(*mouseYposPtr-sliderYpos);
            barClicked = true;
        }
    }


    if (status == 3) {
        DrawPanel(bx1, by1, bx2, by1+9, mui::VScrollBar::col, mui::PanelStyle::InButton);
    } else {
        DrawPanel(bx1, by1, bx2, by1+9, mui::VScrollBar::col, mui::PanelStyle::OutButton);
    }

    if (status == 4) {
        DrawPanel(bx1, by2-9, bx2, by2, mui::VScrollBar::col, mui::PanelStyle::InButton);
    } else {
        DrawPanel(bx1, by2-9, bx2, by2, mui::VScrollBar::col, mui::PanelStyle::OutButton);
    }


    struct vert {
        int x,y;
        u_int col;
    } verts[8];

    verts[0] = (vert){ lArrowX-2    , lArrowY+2 , 0xffffffff };
    verts[1] = (vert){ lArrowX      , lArrowY   , 0xffffffff };
    verts[2] = (vert){ lArrowX      , lArrowY   , 0xffffffff };
    verts[3] = (vert){ lArrowX+3    , lArrowY+3 , 0xffffffff };

    verts[4] = (vert){ rArrowX-2    , rArrowY-2 , 0xffffffff };
    verts[5] = (vert){ rArrowX      , rArrowY   , 0xffffffff };
    verts[6] = (vert){ rArrowX      , rArrowY   , 0xffffffff };
    verts[7] = (vert){ rArrowX+3    , rArrowY-2 , 0xffffffff };

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

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(2, GL_INT, sizeof(vert), &verts[0].x);
    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(vert), &verts[0].col);

    glDrawArrays(GL_LINES, 0, 8);


    DrawPanel(bx1, by1+10, bx2, by2-10, mui::VScrollBar::col, mui::PanelStyle::TextBox);


    if (barClicked) {
        DrawPanel(bx1, sliderYpos, bx2, sliderYpos+sliderYsize, mui::VScrollBar::col, mui::PanelStyle::InButton);
    } else {
        DrawPanel(bx1, sliderYpos, bx2, sliderYpos+sliderYsize, mui::VScrollBar::col, mui::PanelStyle::OutButton);
    }

}


void mui::DrawPanel(int x1, int y1, int x2, int y2, int col, short style) {

    float rv = (float)RED(col)/255;
    float gv = (float)GREEN(col)/255;
    float bv = (float)BLUE(col)/255;

    u_int pal[] = {
        RGBA(204*rv, 204*gv, 204*bv, 255),   // Top-left shine
        RGBA(135*rv, 135*gv, 135*bv, 255),   // Bottom-right shine
        RGBA(168*rv, 168*gv, 168*bv, 255),   // Fill
        RGBA(231*rv, 231*gv, 231*bv, 255),   // Top-left corner
        RGBA(168*rv, 168*gv, 168*bv, 255),   // Bottom-left, top-right corners
        RGBA(102*rv, 102*gv, 102*bv, 255),   // Bottom-right corner
        RGBA(181*rv, 181*gv, 181*bv, 255),
    };

    struct vert {
        int x,y;
        u_int col;
    } verts[8];


    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    if (gdl::pc::glCompatMode == false) glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);


    glVertexPointer(2, GL_INT, sizeof(vert), &verts[0].x);
    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(vert), &verts[0].col);


    switch(style) {
    case mui::PanelStyle::OutButton: // Outward button

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

    case mui::PanelStyle::InButton:  // Inward button

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

    case mui::PanelStyle::TextBox:

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

    case mui::PanelStyle::InFrame:

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

        break;

    }

}
