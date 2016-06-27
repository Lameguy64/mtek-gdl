#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include <windows.h>

#include <gl/glew.h>
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <glfw/glfw3native.h>

#define GDL_DEBUG
#include <mgdl-pc.h>

#include "font.h"
#include "mgui.h"
#include "custom.h"


#define VERSION "0.10a"

#define FILTERS_IMAGE_FILES     "Image Files (*.png,*.bmp,*.jpg)\0*.png;*.bmp;*.jpg\0All Files (*.*)\0"
#define FILTERS_MAP_FILES       "TR Sprite Map (*.tsm)\0*.tsm\0C Header (*.h)\0*.h\0"

#define MAP_FMT_TSM 0
#define MAP_FMT_H 1


gdl::Font font;

int zoomRatio = 1;
int activeSheet = 0;


int  spriteMapFormat = MAP_FMT_TSM;
char spriteMapFile[MAX_PATH] = { 0 };
//char nameDefsFile[MAX_PATH] = { 0 };
bool changesMade = false;

typedef struct {
    char	id[3];
    char	ver;
    short	numSheets;
    short	numSprites;
} TSM_HeaderStruct;

typedef struct {
    short	sheetNum;
    short	tx1,ty1;
    short	tx2,ty2;
    short	px,py;
} TSM_EntryStruct;


namespace Sheet {

    int NumSheets = 0;
    int SheetAlloc = 0;
    gdl::Image **SheetImage = NULL;

    char **SheetFiles = NULL;

    void Init();
    bool LoadSheet(const char* fileName);
    void DeleteSheet(int id);

    void AddButtonCallback(int id);
    void RemoveButtonCallback(int id);
    void RenameButtonCallback(int id);

    gui::ListBox SheetFilesList(4, 308, 32, 32, NULL, true);

    gui::Button AddButton("Add Sheet...", 10, 10, 104, 24, AddButtonCallback);
    gui::Button RemoveButton("Remove Sheet", 10, 10, 104, 24, RemoveButtonCallback);
    gui::Button RenameButton("Rename Sheet...", 10, 10, 104, 24, RenameButtonCallback);

}


namespace Sprite {

    typedef struct {
        int x,y;
        int w,h;
        int sheet;
        int px,py;
        char name[64];
    } SpriteEntry;

    int NumSprites = 0;
    int SpritesAlloc = 0;
    SpriteEntry *Sprites = NULL;

    int MapperZoom = 1;


    class SpriteList {

        int x,y;
        int width,height;

        gui::VScrollBar ScrollBar;

    public:

        int CurrentSprite;

        SpriteList(int x = 0, int y = 0, int width = 0, int height = 0);

        void SetPos(int x, int y);
        void SetSize(int width, int height);

        void DoSpriteList();
        void ScrollToSelection();

    };


    void Init();
    void AddEntry(int x = 0, int y = 0, int w = 0, int h = 0, int sheet = -1, int px = 0, int py = 0);
    void DeleteEntry(int index);
    void Deinit();

    void DrawSprite(short x, short y, short cx, short cy, float scale, bool filtered, SpriteEntry* sprite);

    void SpriteViewCallback(int x, int y, int width, int height, gui::HScrollBar *hScrollBar, gui::VScrollBar *vScrollBar);
    void AddSpriteButtonCallback(int index);
    void MapSpriteButtonCallback(int index);
    void DeleteSpriteButtonCallback(int index);

    Sprite::SpriteList SpriteList;
    ScrollImageBox SpriteView(4, 48, 320, 256, SpriteViewCallback);

    gui::Button NewSpriteButton("New Sprite", 0, 0, 104, 24, AddSpriteButtonCallback);
    gui::Button MapSpriteButton("Map Sprite...", 0, 0, 104, 24, MapSpriteButtonCallback);
    gui::Button DeleteSpriteButton("Delete Sprite", 0, 0, 104, 24, DeleteSpriteButtonCallback);

    gui::CheckBox CenterPivotCheck(10, 10, "Center by Pivot");
    gui::CheckBox FilteredCheck(10, 10, "Filtered");

    gui::TextBox DefNameText;

}

void NewSet();
bool SaveSet(bool saveAs);
bool CheckSaved();

void NewFileButtonCallback(int id);
void LoadFileButtonCallback(int id);
void SaveFileButtonCallback(int id);
void SaveFileAsButtonCallback(int id);

bool LoadTSMfile(const char* fileName);
bool SaveTSMfile(const char* fileName);
bool SaveHeaderFile(const char* fileName);
bool LoadNamesFile(const char* fileName);
bool SaveNamesFile(const char* fileName);

void StripPathName(char* outName, const char* pathName);

bool OpenFileDialog(char* fileNameBuff, const char* filters, int filterIndex, const char* initialDir = NULL);
bool SaveFileDialog(char* fileNameBuff, const char* filters, int filterIndex, const char* initialDir = NULL);

#endif // _MAIN_H
