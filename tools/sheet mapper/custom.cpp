#include "custom.h"

extern gdl::Font font;

extern int zoomRatio;
extern int activeSheet;

namespace Sheet {

    extern int NumSheets;
    extern int SheetAlloc;
    extern gdl::Image **SheetImage;

    extern gui::ListBox SheetList;

    extern char **SheetFiles;

}



GLuint Checks::textureID = 0;

void Checks::Init() {

    if (textureID != 0)
        return;

    u_int checkPattern[] = {
        RGBA(167, 167, 167, 255),   RGBA(97,  97,  97,  255),
        RGBA(97,  97,  97,  255),   RGBA(167, 167, 167, 255)
    };

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkPattern);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

void Checks::Draw(int x1, int y1, int x2, int y2, u_int col) {

    float texX = (((x2-x1))/32.f);
    float texY = (((y2-y1))/32.f);

    struct vert {

        int x,y;
        float u,v;
        u_int col;

    } verts[] = {

        { x1, y1, 0.f , 0.f  },
        { x2, y1, texX, 0.f  },
        { x2, y2, texX, texY },
        { x1, y2, 0.f , texY }

    };


    if (gdl::pc::glCompatMode == false)
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, textureID);


    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    glColor4f(RED(col)/255.f, GREEN(col)/255.f, BLUE(col)/255.f, ALPHA(col)/255.f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(2, GL_INT, sizeof(vert), &verts[0].x);
    glTexCoordPointer(2, GL_FLOAT, sizeof(vert), &verts[0].u);

    glDrawArrays(GL_QUADS, 0, 4);

}


ScrollImageBox::ScrollImageBox(int x, int y, int w, int h, ScrollImageBoxCallback callback) {

    ScrollImageBox::xPos = x;
    ScrollImageBox::yPos = y;
    ScrollImageBox::width = w;
    ScrollImageBox::height = h;
    ScrollImageBox::callback = callback;

    vScrollBar.SetPos((x+w)+2, y);
    vScrollBar.SetSize(17, h);

}

void ScrollImageBox::SetSize(int w, int h) {

    ScrollImageBox::width = w;
    ScrollImageBox::height = h;

    if (ScrollImageBox::callback != NULL) {
        ScrollImageBox::callback(ScrollImageBox::xPos, ScrollImageBox::yPos, ScrollImageBox::width, ScrollImageBox::height, &hScrollBar, &vScrollBar);
    }

    vScrollBar.SetPos((ScrollImageBox::xPos+w)+2, ScrollImageBox::yPos);
    vScrollBar.SetSize(17, h);

    hScrollBar.SetPos(ScrollImageBox::xPos, (ScrollImageBox::yPos+h)+2);
    hScrollBar.SetSize(w, 17);

}

void ScrollImageBox::DoImageBox() {

    gui::DrawPanel(
        ScrollImageBox::xPos,
        ScrollImageBox::yPos,
        ScrollImageBox::xPos+(ScrollImageBox::width-1),
        ScrollImageBox::yPos+(ScrollImageBox::height-1),
        gdl::Color::White,
        gui::PanelStyle::InFrame
    );


    ScrollImageBox::vScrollBar.DoScrollBar();
    ScrollImageBox::hScrollBar.DoScrollBar();

}
