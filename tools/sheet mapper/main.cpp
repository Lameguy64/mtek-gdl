#include "main.h"


void KeyPressCallback(int key, int scancode, int action, int mods) {

    if ((action == gdl::Keyboard::KEY_PRESSED) || (action == gdl::Keyboard::KEY_REPEAT)) {

        if (mods & gdl::Keyboard::KEY_MOD_CTRL) {

            switch(key) {
            case gdl::Keyboard::KEY_N:
                NewFileButtonCallback(0);
                break;
            case gdl::Keyboard::KEY_O:
                LoadFileButtonCallback(0);
                break;
            case gdl::Keyboard::KEY_S:
                SaveFileButtonCallback(0);
                break;
            }

        }

        if (!gui::TextBoxActive) {

            if (mods == gdl::Keyboard::KEY_MOD_ALT) {

                if (key == gdl::Keyboard::KEY_PAGE_UP) {
                    if (Sprite::SpriteList.CurrentSprite > 0) {

                        Sprite::SpriteEntry tempEntry = Sprite::Sprites[Sprite::SpriteList.CurrentSprite-1];
                        Sprite::Sprites[Sprite::SpriteList.CurrentSprite-1] = Sprite::Sprites[Sprite::SpriteList.CurrentSprite];
                        Sprite::Sprites[Sprite::SpriteList.CurrentSprite] = tempEntry;

                        Sprite::SpriteList.CurrentSprite -= 1;
                        Sprite::SpriteList.ScrollToSelection();
                        changesMade = true;

                    }
                } else if (key == gdl::Keyboard::KEY_PAGE_DOWN) {
                    if (Sprite::SpriteList.CurrentSprite < Sprite::NumSprites-1) {

                        Sprite::SpriteEntry tempEntry = Sprite::Sprites[Sprite::SpriteList.CurrentSprite+1];
                        Sprite::Sprites[Sprite::SpriteList.CurrentSprite+1] = Sprite::Sprites[Sprite::SpriteList.CurrentSprite];
                        Sprite::Sprites[Sprite::SpriteList.CurrentSprite] = tempEntry;

                        Sprite::SpriteList.CurrentSprite += 1;
                        Sprite::SpriteList.ScrollToSelection();
                        changesMade = true;

                    }
                }

            } else {

                if (key == gdl::Keyboard::KEY_PAGE_UP) {

                    if (Sprite::SpriteList.CurrentSprite > 0) {
                        Sprite::SpriteList.CurrentSprite -= 1;
                        Sprite::SpriteList.ScrollToSelection();
                    }

                } else if (key == gdl::Keyboard::KEY_PAGE_DOWN) {

                    if (Sprite::SpriteList.CurrentSprite < (Sprite::NumSprites-1)) {
                        Sprite::SpriteList.CurrentSprite += 1;
                        Sprite::SpriteList.ScrollToSelection();
                    }

                } else if (key == gdl::Keyboard::KEY_HOME) {

                    if (Sprite::NumSprites) {
                        Sprite::SpriteList.CurrentSprite = 0;
                        Sprite::SpriteList.ScrollToSelection();
                    }

                } else if (key == gdl::Keyboard::KEY_END) {

                    if (Sprite::NumSprites) {
                        Sprite::SpriteList.CurrentSprite = Sprite::NumSprites-1;
                        Sprite::SpriteList.ScrollToSelection();
                    }

                } else if (key == gdl::Keyboard::KEY_DELETE) {

                    if (Sprite::NumSprites) {

                        Sprite::DeleteEntry(Sprite::SpriteList.CurrentSprite);

                        if (Sprite::SpriteList.CurrentSprite > Sprite::NumSprites-1)
                            Sprite::SpriteList.CurrentSprite = Sprite::NumSprites-1;

                        changesMade = true;

                    }

                }

            }

        }

    }

}


int main(int argc, const char* argv[]) {


    // Init MTek-GDL library
    if (gdl::Init("Sheet Mapper Tool v" VERSION, 1024, 768, 24, 0, gdl::NoVSync|gdl::AllowResize)) {
        printf("Could not initialize MTek-GDL.\n");
        exit(EXIT_FAILURE);
    }


    // Set GLFW specific callbacks
    gdl::Keyboard::SetKeyPressCallback(KeyPressCallback);


    // Load font data
    font.LoadFontFromMem(fnt_NormFont);
    gui::SetFont(font);


    // Init sprite and sheet handlers
    Checks::Init();
    Sheet::Init();
    Sprite::Init();



    // Create button objects
    gui::Button file_NewButton("New (Ctrl+N)", 4, 4, 104, 24, NewFileButtonCallback);
    gui::Button file_OpenButton("Open... (Ctrl+O)", 112, 4, 104, 24, LoadFileButtonCallback);
    gui::Button file_SaveButton("Save... (Ctrl+S)", 220, 4, 104, 24, SaveFileButtonCallback);
    gui::Button file_SaveAsButton("Save As...", 324, 4, 104, 24, SaveFileAsButtonCallback);


    // Main program loop
    while(1) {

        if (gdl::pc::WindowShouldClose()) {

            if (CheckSaved()) {
                break;
            } else {
                glfwSetWindowShouldClose(gdl::pc::WindowContext, FALSE);
            }

        }

        //KeyPress.hit = false;


        /*
        gdl::Keyboard::KeyPressed = false;
        gdl::Keyboard::KeyChar = 0;
        gdl::Keyboard::KeyCode = 0;
        */

        //glfwWaitEvents();


        gui::KeyboardInput.keyHit   = gdl::Keyboard::Key.Pressed;
        gui::KeyboardInput.keyChar  = gdl::Keyboard::Key.Char;
        gui::KeyboardInput.keyCode  = gdl::Keyboard::Key.Code;

        gui::MouseInput.x           = gdl::Mouse::MouseInfo.x;
        gui::MouseInput.y           = gdl::Mouse::MouseInfo.y;
        gui::MouseInput.Left        = gdl::Mouse::MouseInfo.left;
        gui::MouseInput.Right       = gdl::Mouse::MouseInfo.right;
        gui::MouseInput.Middle      = gdl::Mouse::MouseInfo.middle;
        gui::MouseInput.ClickLeft   = gdl::Mouse::MouseInfo.leftClick;
        gui::MouseInput.ClickRight  = gdl::Mouse::MouseInfo.rightClick;
        gui::MouseInput.ClickMiddle = gdl::Mouse::MouseInfo.middleClick;
        gui::MouseInput.Hscroll     = gdl::Mouse::MouseInfo.xScroll;
        gui::MouseInput.Vscroll     = gdl::Mouse::MouseInfo.yScroll;

        // Parse keyboard inputs (but only if no text box is active)
        if (gdl::Keyboard::Key.Pressed) {

            if (!gui::TextBoxActive) {

                switch(tolower(gdl::Keyboard::Key.Char)) {
                case 'a':   // Zoom in
                    if (zoomRatio < 8) zoomRatio += 1;
                    break;
                case 'z':   // Zoom out
                    if (zoomRatio > 1) zoomRatio -= 1;
                    break;
                case 'n':
                    Sprite::AddEntry();
                    Sprite::SpriteList.ScrollToSelection();
                    Sprite::SpriteList.CurrentSprite = Sprite::NumSprites-1;
                    changesMade = true;
                    break;
                case 'm':
                    gdl::Keyboard::SetKeyPressCallback(NULL);
                    Sprite::MapSpriteButtonCallback(0);
                    gdl::Keyboard::SetKeyPressCallback(KeyPressCallback);
                    break;
                }

                if (Sprite::CenterPivotCheck.ticked) {

                    if ((Sprite::SpriteList.CurrentSprite >= 0) && (Sprite::SpriteList.CurrentSprite < Sprite::NumSprites)) {
                        if (Sprite::Sprites[Sprite::SpriteList.CurrentSprite].sheet >= 0) {

                            switch(tolower(gdl::Keyboard::Key.Char)) {
                            case '4':
                                Sprite::Sprites[Sprite::SpriteList.CurrentSprite].px -= 1;
                                changesMade = true;
                                break;
                            case '6':
                                Sprite::Sprites[Sprite::SpriteList.CurrentSprite].px += 1;
                                changesMade = true;
                                break;
                            case '8':
                                Sprite::Sprites[Sprite::SpriteList.CurrentSprite].py -= 1;
                                changesMade = true;
                                break;
                            case '2':
                                Sprite::Sprites[Sprite::SpriteList.CurrentSprite].py += 1;
                                changesMade = true;
                                break;
                            case '\\':
                                Sprite::Sprites[Sprite::SpriteList.CurrentSprite].px = Sprite::Sprites[Sprite::SpriteList.CurrentSprite].w/2;
                                Sprite::Sprites[Sprite::SpriteList.CurrentSprite].py = Sprite::Sprites[Sprite::SpriteList.CurrentSprite].h/2;
                                changesMade = true;
                                break;
                            }

                        }
                    }

                }

            }

        }


        gui::TextBoxActive = false;
        gdl::PrepDisplay();

        gdl::DrawBoxF(0, 0, gdl::ScreenXres-1, gdl::ScreenYres-1, RGBA(198, 198, 194, 255));


        file_NewButton.DoButton();
        file_OpenButton.DoButton();
        file_SaveButton.DoButton();
        file_SaveAsButton.DoButton();


        Sprite::NewSpriteButton.SetPos(gdl::ScreenXres-323, 47);
        Sprite::NewSpriteButton.DoButton();
        Sprite::MapSpriteButton.SetPos(gdl::ScreenXres-323, 75);
        Sprite::MapSpriteButton.DoButton();
        Sprite::DeleteSpriteButton.SetPos(gdl::ScreenXres-323, 103);
        Sprite::DeleteSpriteButton.DoButton();


        Sprite::CenterPivotCheck.SetPos(gdl::ScreenXres-323, 131);
        Sprite::CenterPivotCheck.DoCheckBox();
        Sprite::FilteredCheck.SetPos(gdl::ScreenXres-323, 146);
        Sprite::FilteredCheck.DoCheckBox();


        font.Printf(gdl::ScreenXres-213, 37, 1.f, gdl::Color::Black, "Sprites: %d", Sprite::NumSprites);
        Sprite::SpriteList.SetPos(gdl::ScreenXres-215, 47);
        Sprite::SpriteList.SetSize(212, gdl::ScreenYres-51);
        Sprite::SpriteList.DoSpriteList();

        font.Printf(6, 37, 1.f, gdl::Color::Black, "Sprite Preview:");
        Sprite::SpriteView.SetSize(gdl::ScreenXres-350, gdl::ScreenYres-317);
        Sprite::SpriteView.DoImageBox();


        if (Sprite::SpriteList.CurrentSprite >= 0) {

            Sprite::DefNameText.SetTextBuff(Sprite::Sprites[Sprite::SpriteList.CurrentSprite].name, 64);
            Sprite::DefNameText.disabled = false;

        } else {

            Sprite::DefNameText.SetTextBuff(NULL, 0);
            Sprite::DefNameText.disabled = true;

        }

        font.DrawText("Name", 4, gdl::ScreenYres-243, 1.f, gdl::Color::Black);
        Sprite::DefNameText.SetPos(30, gdl::ScreenYres-246);
        Sprite::DefNameText.SetSize(gdl::ScreenXres-357, 13);
        Sprite::DefNameText.DoTextBox();


        font.Printf(6, gdl::ScreenYres-223, 1.f, gdl::Color::Black, "Sheet Files:");
        Sheet::SheetFilesList.SetPos(4, gdl::ScreenYres-212);
        //Sheet::SheetFilesList.SetSize(gdl::ScreenXres-439, 208);
        Sheet::SheetFilesList.SetSize(gdl::ScreenXres-652, 208);
        Sheet::SheetFilesList.DoListBox();

        // Draw sheet preview
        {

            int px = (gdl::ScreenXres-644);
            int py = (gdl::ScreenYres-212);

            font.DrawText("Sheet Preview:", px+1, py-11, 1.f, gdl::Color::Black);

            gui::DrawPanel(px, py, px+208, py+208, gdl::Color::White, gui::PanelStyle::InPanel);
            Checks::Draw(px+1, py+1, px+207, py+207, gdl::Color::White);

            if (Sheet::SheetFilesList.selItem >= 0) {

                float scale = 1.f;

                if (Sheet::SheetImage[Sheet::SheetFilesList.selItem]->Xsize() > Sheet::SheetImage[Sheet::SheetFilesList.selItem]->Ysize()) {
                    if (Sheet::SheetImage[Sheet::SheetFilesList.selItem]->Xsize() > 207) {
                        scale -= (float)(Sheet::SheetImage[Sheet::SheetFilesList.selItem]->Xsize()-207)/Sheet::SheetImage[Sheet::SheetFilesList.selItem]->Xsize();
                    }
                } else {
                    if (Sheet::SheetImage[Sheet::SheetFilesList.selItem]->Ysize() > 207) {
                        scale -= (float)(Sheet::SheetImage[Sheet::SheetFilesList.selItem]->Ysize()-207)/Sheet::SheetImage[Sheet::SheetFilesList.selItem]->Ysize();
                    }
                }

                Sheet::SheetImage[Sheet::SheetFilesList.selItem]->Put(px+1, py+1, gdl::Color::White, 0, 0, scale);

            }

        }


        Sheet::AddButton.SetPos(gdl::ScreenXres-431, gdl::ScreenYres-212);
        Sheet::AddButton.DoButton();
        Sheet::RemoveButton.SetPos(gdl::ScreenXres-431, gdl::ScreenYres-184);
        Sheet::RemoveButton.DoButton();
        Sheet::RenameButton.SetPos(gdl::ScreenXres-431, gdl::ScreenYres-156);
        Sheet::RenameButton.DoButton();


        gdl::Display();


    }


    Sprite::Deinit();
    gdl::Close();

}


void NewFileButtonCallback(int id) {

    if (!CheckSaved())
        return;

    NewSet();

}

void NewSet() {

    while(Sprite::NumSprites > 0)
        Sprite::DeleteEntry(0);

    Sprite::SpriteList.CurrentSprite = 0;

    while(Sheet::NumSheets > 0)
        Sheet::DeleteSheet(0);

    spriteMapFormat = MAP_FMT_TSM;
    spriteMapFile[0] = 0x00;
    //nameDefsFile[0] = 0x00;

    changesMade = false;

}

bool CheckSaved() {

    if (changesMade) {

        int result = MessageBox(
            glfwGetWin32Window(gdl::pc::WindowContext),
            "Sprite map has not yet been saved, save now?",
            "Save file",
            MB_YESNOCANCEL | MB_ICONEXCLAMATION
        );

        if (result == IDCANCEL) {

            return(false);

        } else if (result == IDYES) {

            SaveFileButtonCallback(0);
            if (changesMade == false)
                return(true);

            return(false);

        } else if (result == IDNO) {

            return(true);

        }

    }

    return(true);

}

bool SaveSet(bool saveAs) {

    if ((spriteMapFile[0] == 0x00) || (saveAs)) {

        char fileName[MAX_PATH] = {0};

        OPENFILENAME dialog;
        memset(&dialog, 0x00, sizeof(OPENFILENAME));
        dialog.lStructSize     = sizeof(OPENFILENAME); // SEE NOTE BELOW
        dialog.hwndOwner       = glfwGetWin32Window(gdl::pc::WindowContext);
        dialog.lpstrFilter     = FILTERS_MAP_FILES;
        dialog.nFilterIndex    = 0;
        dialog.lpstrFile       = fileName;
        dialog.nMaxFile        = MAX_PATH;
        dialog.Flags           = OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

        if (!GetSaveFileName(&dialog))
            return(false);

        strcpy(spriteMapFile, fileName);

        {

            char *baseFileName = strrchr(spriteMapFile, '\\');

            if (baseFileName == NULL)
                baseFileName = spriteMapFile;
            else
                baseFileName++;

            printf("%s\n", baseFileName);

            char *extPos = strrchr(baseFileName, '.');
            if (extPos == NULL) {
                switch(dialog.nFilterIndex) {
                case 1:
                    strcat(spriteMapFile, ".tsm");
                    break;
                case 2:
                    strcat(spriteMapFile, ".h");
                    break;
                }
            }

        }

    }


    printf("%s\n", spriteMapFile);
    if (strrchr(spriteMapFile, '.') != NULL) {

        char *extName = strdup(strrchr(spriteMapFile, '.')+1);

        for(int i=0; extName[i]!=0; i++) {
            extName[i] = tolower(extName[i]);
        }

        printf("%s\n", extName);

        if (strcmp("tsm", extName) == 0) {

            SaveTSMfile(spriteMapFile);
            printf("Saved in TSM format.\n");

        } else if (strcmp("h", extName) == 0) {

            SaveHeaderFile(spriteMapFile);
            printf("Saved in H header format.\n");

        } else {

            free(extName);
            MessageBox(
                glfwGetWin32Window(gdl::pc::WindowContext),
                "Unknown file format specified.",
                "Invalid file extension", MB_OK|MB_ICONEXCLAMATION
            );
            return(false);

        }

        free(extName);

    }

    changesMade = false;
    return(true);

}

void LoadFileButtonCallback(int id) {

    char fileName[MAX_PATH] = {0};

    if (!OpenFileDialog(fileName, "TR Sprite Map (*.tsm)\0*.tsm\0All Files\0*.*\0", 0))
        return;

    if (!LoadTSMfile(fileName))
        return;

}

void SaveFileButtonCallback(int id) {

    SaveSet(false);

}

void SaveFileAsButtonCallback(int id) {

    SaveSet(true);

}

bool SaveTSMfile(const char* fileName) {

    FILE *fp = fopen(fileName, "wb");
    if (!fp) {
        MessageBox(glfwGetWin32Window(gdl::pc::WindowContext), "File could not be written/created for some reason.", "File Save Error", MB_ICONERROR|MB_OK);
        return(false);
    }


    TSM_HeaderStruct TSM_Header;

    TSM_Header.id[0] = 'T';
    TSM_Header.id[1] = 'S';
    TSM_Header.id[2] = 'M';
    TSM_Header.ver = 0;
    TSM_Header.numSheets = Sheet::NumSheets;
    TSM_Header.numSprites = Sprite::NumSprites;

    fwrite(&TSM_Header, sizeof(TSM_HeaderStruct), 1, fp);


    for(int i=0; i<Sheet::NumSheets; i++) {

        int nameLen = strlen(Sheet::SheetFilesList.GetItem(i));

        fwrite(&nameLen, 1, 1, fp);
        fwrite(Sheet::SheetFilesList.GetItem(i), strlen(Sheet::SheetFilesList.GetItem(i)), 1, fp);

    }


    for(int i=0; i<Sprite::NumSprites; i++) {

        TSM_EntryStruct TSM_Entry;

        TSM_Entry.sheetNum = Sprite::Sprites[i].sheet+1;
        TSM_Entry.tx1 = Sprite::Sprites[i].x;
        TSM_Entry.ty1 = Sprite::Sprites[i].y;
        TSM_Entry.tx2 = Sprite::Sprites[i].x+(Sprite::Sprites[i].w-1);
        TSM_Entry.ty2 = Sprite::Sprites[i].y+(Sprite::Sprites[i].h-1);
        TSM_Entry.px = Sprite::Sprites[i].px;
        TSM_Entry.py = Sprite::Sprites[i].py;

        fwrite(&TSM_Entry, sizeof(TSM_EntryStruct), 1, fp);

    }

    fclose(fp);

    {

        bool namesPresent = false;

        for(int i=0; i<Sprite::NumSprites; i++) {

            if (strlen(Sprite::Sprites[i].name) > 0) {
                namesPresent = true;
                break;
            }

        }

        if (namesPresent) {

            char namesFile[MAX_PATH];
            strcpy(namesFile, fileName);

            if (strrchr(namesFile, '.') != NULL)
                *(strrchr(namesFile, '.')) = 0x00;

            strcat(namesFile, "_names.h");

            SaveNamesFile(namesFile);

        }

    }

    return(true);

}

bool LoadTSMfile(const char* fileName) {

    FILE *fp = fopen(fileName, "rb");
    if (!fp) {
        MessageBox(glfwGetWin32Window(gdl::pc::WindowContext), "File could not be opened for some reason.", "File Open Error", MB_ICONERROR|MB_OK);
        return(false);
    }


    // Get header and make sure that the ID and version number is correct
    TSM_HeaderStruct TSM_Header;
    fread(&TSM_Header, 1, sizeof(TSM_HeaderStruct), fp);

    if (strncmp("TSM", TSM_Header.id, 3) != 0) {
        fclose(fp);
        MessageBox(glfwGetWin32Window(gdl::pc::WindowContext), "File is not a TR Sprite Map File.", "File Not Valid", MB_ICONERROR|MB_OK);
        return(false);
    }

    if (TSM_Header.ver > 0) {
        fclose(fp);
        MessageBox(glfwGetWin32Window(gdl::pc::WindowContext), "New TR Sprite Map File format not supported.", "File Not Valid", MB_ICONERROR|MB_OK);
        return(false);
    }


    if (!CheckSaved())
        return(false);

    NewSet();


    // Load sprite sheets
    for(int i=0; i<TSM_Header.numSheets; i++) {

        int nameLen=0;
        char nameBuff[MAX_PATH];
        char pathBuff[MAX_PATH];

        // Get path name from file being opened
        strcpy(pathBuff, fileName);
        {
            char *slashPos = strrchr(pathBuff, '\\');
            if (slashPos != NULL) *(slashPos+1) = 0x00;
        }

        // Get sheet file entry
        fread(&nameLen, 1, 1, fp);
        fread(nameBuff, 1, nameLen, fp);
        nameBuff[nameLen] = 0x00;

        strcat(pathBuff, nameBuff);
        if (!Sheet::LoadSheet(pathBuff)) {

            char textBuff[MAX_PATH];
            sprintf(textBuff, "The following sheet file is either missing or corrupt:\n%s\n\nWould you like to locate the file yourself?", nameBuff);

            if (MessageBox(glfwGetWin32Window(gdl::pc::WindowContext), textBuff, "Sheet File Not Found", MB_ICONEXCLAMATION|MB_YESNO) == IDNO) {
                NewSet();
                MessageBox(glfwGetWin32Window(gdl::pc::WindowContext), "Could not load sprite map file.", "Sprite Map Load Error", MB_ICONERROR|MB_OK);
                return(false);
            }

            if (!OpenFileDialog(textBuff, FILTERS_IMAGE_FILES, 0)) {
                NewSet();
                MessageBox(glfwGetWin32Window(gdl::pc::WindowContext), "Could not load sprite map file.", "Sprite Map Load Error", MB_ICONERROR|MB_OK);
                return(false);
            }

            if (!Sheet::LoadSheet(textBuff)) {
                NewSet();
                MessageBox(glfwGetWin32Window(gdl::pc::WindowContext), "Could not load image file.", "Sprite Map Load Error", MB_ICONERROR|MB_OK);
                return(false);
            }

        }

    }


    // Load sprite entries
    for(int i=0; i<TSM_Header.numSprites; i++) {

        TSM_EntryStruct TSM_Entry;
        fread(&TSM_Entry, 1, sizeof(TSM_EntryStruct), fp);

        Sprite::AddEntry(
            TSM_Entry.tx1, TSM_Entry.ty1,
            (TSM_Entry.tx2-TSM_Entry.tx1)+1, (TSM_Entry.ty2-TSM_Entry.ty1)+1,
            TSM_Entry.sheetNum-1,
            TSM_Entry.px, TSM_Entry.py
        );

    }

    fclose(fp);
    strcpy(spriteMapFile, fileName);


    // Load names file if it exists
    {

        char namesFile[MAX_PATH];
        strcpy(namesFile, fileName);

        if (strrchr(namesFile, '.') != NULL)
            *(strrchr(namesFile, '.')) = 0x00;

        strcat(namesFile, "_names.h");

        LoadNamesFile(namesFile);

    }

    return(true);

}

bool SaveHeaderFile(const char* fileName) {

    FILE *fp = fopen(fileName, "w");
    if (!fp)
        return(false);


    fprintf(fp, "// Made with Sprite Mapper\n");
    fprintf(fp, "typedef struct {\n");
    fprintf(fp, "\tunsigned short sheet;\n");
    fprintf(fp, "\tunsigned short x,y;\n");
    fprintf(fp, "\tunsigned short w,h;\n");
    fprintf(fp, "\tshort px,py;\n");
    fprintf(fp, "} SPRITE;\n\n");

    fprintf(fp, "// Sheet Files\n");
    fprintf(fp, "const char* spriteSheet[%d] = {\n", Sheet::NumSheets);
    for(int i=0; i<Sheet::NumSheets; i++) {

        if (strrchr(Sheet::SheetFiles[i], '\\') != NULL)
            fprintf(fp, "\t\"%s\",", strrchr(Sheet::SheetFiles[i], '\\')+1);

        fprintf(fp, "\n");

    }
    fprintf(fp, "};\n\n");

    fprintf(fp, "// Sprite Maps\n");
    fprintf(fp, "const SPRITE spriteMap[%d] = {\n", Sprite::NumSprites);
    for(int i=0; i<Sprite::NumSprites; i++) {

        fprintf(fp, "\t{ %d, %d, %d, %d, %d, %d, %d },\n",
            Sprite::Sprites[i].sheet,
            Sprite::Sprites[i].x, Sprite::Sprites[i].y,
            Sprite::Sprites[i].w, Sprite::Sprites[i].h,
            Sprite::Sprites[i].px, Sprite::Sprites[i].py
        );

    }
    fprintf(fp, "};\n");

    fclose(fp);
    return(true);

}

bool LoadNamesFile(const char* fileName) {

    FILE *fp = fopen(fileName, "r");
    if (fp == NULL)
        return(false);


    while(1) {

        char strBuff[256];
        fgets(strBuff, 256, fp);

        if (feof(fp))
            break;

        // Skip comments
        if (strstr(strBuff, "//"))
            continue;

        if (strncmp("#define", strBuff, 7) == 0) {

            // Get name
            char *nameStart = strchr(strBuff, ' ')+1;

            // Get index
            int nameIndex = atoi(strchr(nameStart, ' '));
            *strchr(nameStart, ' ') = 0x00;

            // Set to sprite entry
            strcpy(Sprite::Sprites[nameIndex].name, nameStart);

        }

    }


    fclose(fp);

    return(true);

}

bool SaveNamesFile(const char* fileName) {

    FILE *fp = fopen(fileName, "w");
    if (fp == NULL)
        return(false);

    fputs("// Created with Sprite Mapper\n", fp);

    for(int i=0; i<Sprite::NumSprites; i++) {

        if (strlen(Sprite::Sprites[i].name) > 0) {
            fprintf(fp, "#define %s %d\n", Sprite::Sprites[i].name, i);
        }

    }

    fclose(fp);
    return(true);

}


void Sheet::Init() {

    Sheet::SheetImage = (gdl::Image**)malloc(sizeof(gdl::Image*));
    Sheet::SheetImage[0] = NULL;
    Sheet::SheetFiles = (char**)malloc(sizeof(char*));
    Sheet::SheetFiles[0] = NULL;
    Sheet::SheetAlloc = 1;

}

bool Sheet::LoadSheet(const char* fileName) {

    if (Sheet::NumSheets >= Sheet::SheetAlloc) {
        Sheet::SheetAlloc += 1;
        Sheet::SheetImage = (gdl::Image**)realloc(Sheet::SheetImage, sizeof(gdl::Image*)*SheetAlloc);
        Sheet::SheetFiles = (char**)realloc(Sheet::SheetFiles, sizeof(char*)*SheetAlloc);
    }

    Sheet::SheetImage[Sheet::NumSheets] = new gdl::Image;

    if (!Sheet::SheetImage[Sheet::NumSheets]->LoadImage(fileName)) {
        delete Sheet::SheetImage[Sheet::NumSheets];
        return(false);
    }

    Sheet::SheetFiles[Sheet::NumSheets] = strdup(fileName);

    {
        char name[MAX_PATH];
        StripPathName(name, fileName);
        Sheet::SheetFilesList.AddItem(name, -1);
    }

    Sheet::NumSheets += 1;

    return(true);

}

void Sheet::DeleteSheet(int index) {

    if (index == -1)
        return;
    if (NumSheets == 0)
        return;

    delete Sheet::SheetImage[index];

    for(int i=index; i<NumSheets-1; i++) {
        Sheet::SheetImage[i] = Sheet::SheetImage[i+1];
    }

    Sheet::SheetFilesList.DeleteItem(index);

    for(int i=0; i<Sprite::NumSprites; i++) {

        if (Sprite::Sprites[i].sheet == index) {
            Sprite::DeleteEntry(i);
            i--;
        }

    }

    for(int i=index; i<Sprite::NumSprites; i++) {
        if (Sprite::Sprites[i].sheet > index) {
            Sprite::Sprites[i].sheet--;
        }
    }

    NumSheets--;

    if (Sprite::SpriteList.CurrentSprite > Sprite::NumSprites-1)
        Sprite::SpriteList.CurrentSprite = Sprite::NumSprites-1;

}

void Sheet::AddButtonCallback(int id) {

    char fileNameBuff[MAX_PATH] = "";

    if (OpenFileDialog(fileNameBuff, FILTERS_IMAGE_FILES, 0)) {

        Sheet::LoadSheet(fileNameBuff);
        changesMade = true;

    }

}

void Sheet::RemoveButtonCallback(int id) {

    if (Sheet::NumSheets == 0)
        return;

    int result = MessageBox(
        glfwGetWin32Window(gdl::pc::WindowContext),
        "Are you sure you want to remove the selected sheet file?\n"
        "Any sprites associated to this sheet file will be deleted.",
        "Confirm Sheet Removal",
        MB_YESNO|MB_ICONINFORMATION
    );

    if (result == IDNO)
        return;

    Sheet::DeleteSheet(Sheet::SheetFilesList.selItem);
    changesMade = true;

}

void Sheet::RenameButtonCallback(int id) {

    if (Sheet::SheetFilesList.selItem < 0)
        return;
    if (Sheet::NumSheets == 0)
        return;


    char outFileName[MAX_PATH] = { 0 };
    char sheetsDir[MAX_PATH] = { 0 };

    strcpy(outFileName, Sheet::SheetFilesList.GetItem(Sheet::SheetFilesList.selItem));
    strcpy(sheetsDir, spriteMapFile);

    if (strrchr(sheetsDir, '\\') != NULL)
        *(strrchr(sheetsDir, '\\')+1) = 0x00;


    if (!SaveFileDialog(outFileName, FILTERS_IMAGE_FILES, 0, sheetsDir))
        return;

    strcat(sheetsDir, Sheet::SheetFilesList.GetItem(Sheet::SheetFilesList.selItem));
    MoveFile(sheetsDir, outFileName);

    if (strrchr(outFileName, '\\'))
        Sheet::SheetFilesList.SetItem(strrchr(outFileName, '\\')+1, Sheet::SheetFilesList.selItem);
    else
        Sheet::SheetFilesList.SetItem(outFileName, Sheet::SheetFilesList.selItem);

    changesMade = true;

}


void Sprite::DrawSprite(short x, short y, short cx, short cy, float scale, bool filtered, SpriteEntry* sprite) {

    if (sprite->sheet < 0)
        return;
    if (sprite->sheet > (Sheet::NumSheets-1))
        return;

    // Calculate image's center point (if cx or cy is set as gdl::Centered)
    short tx = x,ty = y;
    float tcx = 0,tcy = 0;

    if (cx == gdl::PCentered) {
        tx = x-(sprite->w/2);
        tcx = (float)sprite->w/2.f;
    } else if (cx == gdl::Centered) {
        tx = x-(sprite->w/2);
        tcx = (int)sprite->w/2;
    } else if (cx == gdl::Pivot) {
        tx = x-sprite->px;
        tcx = sprite->px;
    } else if (cx == gdl::CPivot) {
        tx = (x-sprite->px)-floor(scale/2);
        tcx = sprite->px;
    } else {
        tx -= cx;
        tcx = cx;
    }

    if (cy == gdl::PCentered) {
        ty = y-(sprite->h/2);
        tcy = (float)sprite->h/2.f;
    } else if (cy == gdl::Centered) {
        ty = y-(sprite->h/2);
        tcy = (int)sprite->h/2;
    } else if (cy == gdl::Pivot) {
        ty = y-sprite->py;
        tcy = sprite->py;
    } else if (cy == gdl::CPivot) {
        ty = (y-sprite->py)-floor(scale/2);
        tcy = sprite->py;
    } else {
        ty -= cy;
        tcy = cy;
    }


    glEnable(GL_TEXTURE_2D);

    // Perform rotate and scale transforms
    glPushMatrix();
    glTranslatef(tx+tcx, ty+tcy, 0);
    glScalef(scale, scale, 1);
    glTranslatef(-tcx, -tcy, 0);


    gdl::VERTEX2D verts[4];

    {

        short sheetXsize = Sheet::SheetImage[sprite->sheet]->Xsize();
        short sheetYsize = Sheet::SheetImage[sprite->sheet]->Ysize();

        float u[] = {
            (GLfloat)sprite->x/sheetXsize,
            ((GLfloat)(sprite->x+sprite->w)-0.01f)/sheetXsize,
        };

        float v[] = {
            (GLfloat)sprite->y/sheetYsize,
            ((GLfloat)(sprite->y+sprite->h)-0.01f)/sheetYsize,
        };

        verts[0].x = 0;         verts[0].y = 0;
        verts[0].u = u[0];      verts[0].v = v[0];
        verts[1].x = sprite->w; verts[1].y = 0;
        verts[1].u = u[1];      verts[1].v = v[0];
        verts[2].x = sprite->w; verts[2].y = sprite->h;
        verts[2].u = u[1];      verts[2].v = v[1];
        verts[3].x = 0;         verts[3].y = sprite->h;
        verts[3].u = u[0];      verts[3].v = v[1];

    }

    glBindTexture(GL_TEXTURE_2D, Sheet::SheetImage[sprite->sheet]->Texture.TexID());

    if (filtered) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // Setup pointers
    glVertexPointer(2, GL_SHORT, sizeof(gdl::VERTEX2D), (void*)&verts[0].x);
    glTexCoordPointer(2, GL_FLOAT, sizeof(gdl::VERTEX2D), (void*)&verts[0].u);

    // Render!
    glDrawArrays(GL_QUADS, 0, 4);

    glPopMatrix();

    if (filtered) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

}

void Sprite::SpriteViewCallback(int x, int y, int width, int height, gui::HScrollBar *hScrollBar, gui::VScrollBar *vScrollBar) {

    int x2 = x+(width-1);
    int y2 = y+(height-1);

    int boxCenterX = (x+1)+((width-1)/2);
    int boxCenterY = (y+1)+((height-1)/2);

    Checks::Draw(x, y, x2, y2, gdl::Color::White);


    if ((SpriteList.CurrentSprite >= 0) && (SpriteList.CurrentSprite < NumSprites)) {

        gdl::SetClipBox(x+1, y+1, x2-1, y2-1);

        if (!Sprite::CenterPivotCheck.ticked) {
            DrawSprite(boxCenterX, boxCenterY, gdl::PCentered, gdl::PCentered, zoomRatio, Sprite::FilteredCheck.ticked, &Sprites[SpriteList.CurrentSprite]);
        } else {
            DrawSprite(boxCenterX, boxCenterY, gdl::CPivot, gdl::CPivot, zoomRatio, Sprite::FilteredCheck.ticked, &Sprites[SpriteList.CurrentSprite]);
        }

        gdl::ResetClipBox();

    }

    if (Sprite::CenterPivotCheck.ticked) {
        gdl::DrawLine(boxCenterX, y+1, boxCenterX, y+(height-2), RGBA(255, 127, 0, 191));
        gdl::DrawLine(x+1, boxCenterY, x+(width-2), boxCenterY, RGBA(255, 127, 0, 191));
    }

    hScrollBar->disabled = true;
    vScrollBar->disabled = true;

}

void Sprite::Init() {

    Sprites = (SpriteEntry*)malloc(sizeof(SpriteEntry));
    memset(&Sprites[0], 0x00, sizeof(SpriteEntry));
    SpritesAlloc = 1;

}

void Sprite::AddEntry(int x, int y, int w, int h, int sheet, int px, int py) {

    if (NumSprites >= SpritesAlloc) {
        SpritesAlloc++;
        Sprites = (SpriteEntry*)realloc(Sprites, sizeof(SpriteEntry)*SpritesAlloc);
    }

    memset(&Sprites[NumSprites], 0x00, sizeof(SpriteEntry));
    Sprites[NumSprites].x = x;
    Sprites[NumSprites].y = y;
    Sprites[NumSprites].w = w;
    Sprites[NumSprites].h = h;
    Sprites[NumSprites].sheet = sheet;
    Sprites[NumSprites].px = px;
    Sprites[NumSprites].py = py;

    NumSprites++;

}

void Sprite::DeleteEntry(int index) {

    for(int i=index; i<Sprite::NumSprites; i++) {
        Sprite::Sprites[i] = Sprite::Sprites[i+1];
    }

    Sprite::NumSprites--;

}

void Sprite::Deinit() {

    free(Sprites);

}

void Sprite::AddSpriteButtonCallback(int index) {

    AddEntry();
    SpriteList.CurrentSprite = Sprite::NumSprites-1;
    changesMade = true;

}

void Sprite::MapSpriteButtonCallback(int index) {

    if (Sprite::NumSprites == 0)
        return;

    if (Sprite::Sprites[Sprite::SpriteList.CurrentSprite].sheet == -1) {
        if (Sheet::SheetFilesList.selItem < 0) {
            MessageBox(glfwGetWin32Window(gdl::pc::WindowContext), "Select a sheet file to map the selected sprite to first.", "No Sheet Selected", MB_OK|MB_ICONINFORMATION);
            return;
        }
    }


    typedef struct {
        int x1,y1;
        int x2,y2;
    } SelBoxStruct;

    SelBoxStruct tempSelBox = {
        Sprite::Sprites[Sprite::SpriteList.CurrentSprite].x,
        Sprite::Sprites[Sprite::SpriteList.CurrentSprite].y,
        Sprite::Sprites[Sprite::SpriteList.CurrentSprite].x+(Sprite::Sprites[Sprite::SpriteList.CurrentSprite].w-1),
        Sprite::Sprites[Sprite::SpriteList.CurrentSprite].y+(Sprite::Sprites[Sprite::SpriteList.CurrentSprite].h-1)
    };

    int sheetFile;
    if (Sprite::Sprites[Sprite::SpriteList.CurrentSprite].sheet == -1) {
        sheetFile = Sheet::SheetFilesList.selItem;
    } else {
        sheetFile = Sprite::Sprites[Sprite::SpriteList.CurrentSprite].sheet;
    }

    short sheetScrollX=0,sheetScrollY=0;
    short sheetClickX=0,sheetClickY=0;
    bool sheetDragClick = false;


    //tempEntry.sheet = -1;
    SelBoxStruct selBox = {0};
    bool selBoxClick = false;

    bool  selBoxDragClick = false;
    short selBoxDragState = 0;
    short selBoxClickX = 0;
    short selBoxClickY = 0;
    short selBoxWidth = 0;
    short selBoxHeight = 0;

    glfwPostEmptyEvent();

    while(!gdl::pc::WindowShouldClose()) {

        /*
        gdl::Keyboard::KeyPressed = false;
        gdl::Keyboard::KeyChar = 0;
        gdl::Keyboard::KeyCode = 0;
        */
        glfwWaitEvents();

        if (sheetDragClick == false) {

            if (gdl::Keyboard::Key.Code == gdl::Keyboard::KEY_ESCAPE)
                break;

            if (gdl::Keyboard::Key.Code == gdl::Keyboard::KEY_ENTER) {
                Sprite::Sprites[Sprite::SpriteList.CurrentSprite].x = tempSelBox.x1;
                Sprite::Sprites[Sprite::SpriteList.CurrentSprite].y = tempSelBox.y1;
                Sprite::Sprites[Sprite::SpriteList.CurrentSprite].w = (tempSelBox.x2-tempSelBox.x1)+1;
                Sprite::Sprites[Sprite::SpriteList.CurrentSprite].h = (tempSelBox.y2-tempSelBox.y1)+1;
                Sprite::Sprites[Sprite::SpriteList.CurrentSprite].sheet = sheetFile;
                changesMade = true;
                break;
            }

            switch(tolower(gdl::Keyboard::Key.Char)) {
            case 'a':   // Zoom in
                if (Sprite::MapperZoom < 8) {
                    Sprite::MapperZoom += 1;
                }
                break;
            case 'z':   // Zoom out
                if (Sprite::MapperZoom > 1) {
                    Sprite::MapperZoom -= 1;
                }
                break;
            }

        }


        short sheetXdim = Sheet::SheetImage[sheetFile]->Xsize()*Sprite::MapperZoom;
        short sheetYdim = Sheet::SheetImage[sheetFile]->Ysize()*Sprite::MapperZoom;


        // Sheet drag scrolling
        if (gdl::Mouse::MouseInfo.rightClick) {

            sheetClickX = sheetScrollX+gdl::Mouse::MouseInfo.x;
            sheetClickY = sheetScrollY+gdl::Mouse::MouseInfo.y;
            sheetDragClick = true;

        } else {

            if (sheetDragClick) {

                sheetScrollX = (sheetClickX-gdl::Mouse::MouseInfo.x);
                sheetScrollY = (sheetClickY-gdl::Mouse::MouseInfo.y);

                if (!gdl::Mouse::MouseInfo.right)
                    sheetDragClick = false;

            }

        }

        if (sheetXdim > gdl::ScreenXres) {

            if (sheetScrollX < -64)
                sheetScrollX = -64;
            if (sheetScrollX > ((sheetXdim-gdl::ScreenXres)+64))
                sheetScrollX = ((sheetXdim-gdl::ScreenXres)+64);

        } else {

            if (sheetScrollX > 64)
                sheetScrollX = 64;
            if (sheetScrollX < -(gdl::ScreenXres-sheetXdim+64))
                sheetScrollX = -(gdl::ScreenXres-sheetXdim+64);

        }

        if (sheetYdim > gdl::ScreenYres) {

            if (sheetScrollY < -64)
                sheetScrollY = -64;
            if (sheetScrollY > ((sheetYdim-gdl::ScreenYres)+64))
                sheetScrollY = ((sheetYdim-gdl::ScreenYres)+64);

        } else {

            if (sheetScrollY > 64)
                sheetScrollY = 64;
            if (sheetScrollY < -(gdl::ScreenYres-sheetYdim+64))
                sheetScrollY = -(gdl::ScreenYres-sheetYdim+64);

        }


        if (gdl::Mouse::MouseInfo.leftClick) {

            SelBoxStruct drawSelBox;

            drawSelBox.x1 = (tempSelBox.x1*Sprite::MapperZoom)-sheetScrollX;
            drawSelBox.y1 = (tempSelBox.y1*Sprite::MapperZoom)-sheetScrollY;
            drawSelBox.x2 = ((tempSelBox.x2*Sprite::MapperZoom)-sheetScrollX)+(Sprite::MapperZoom-1);
            drawSelBox.y2 = ((tempSelBox.y2*Sprite::MapperZoom)-sheetScrollY)+(Sprite::MapperZoom-1);

            if (gui::InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y,
            drawSelBox.x1-16, drawSelBox.y1-16, drawSelBox.x2+16, drawSelBox.y2+16)) {

                // Selection drag
                if (gui::InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y,
                drawSelBox.x1, drawSelBox.y1, drawSelBox.x2, drawSelBox.y2)) {

                    selBoxClickX = tempSelBox.x1-((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom);
                    selBoxClickY = tempSelBox.y1-((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom);
                    selBoxWidth  = tempSelBox.x2-tempSelBox.x1;
                    selBoxHeight = tempSelBox.y2-tempSelBox.y1;

                    selBoxDragState = 0;
                    selBoxDragClick = true;

                // Left drag
                } else if (gui::InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y,
                drawSelBox.x1-16, drawSelBox.y1, drawSelBox.x2, drawSelBox.y2)) {

                    selBoxClickX = tempSelBox.x1-((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom);
                    selBoxDragState = 1;
                    selBoxDragClick = true;

                // Right drag
                } else if (gui::InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y,
                drawSelBox.x2, drawSelBox.y1, drawSelBox.x2+16, drawSelBox.y2)) {

                    selBoxClickX = tempSelBox.x2-((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom);
                    selBoxDragState = 2;
                    selBoxDragClick = true;

                // Top
                } else if (gui::InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y,
                drawSelBox.x1, drawSelBox.y1-16, drawSelBox.x2, drawSelBox.y1)) {

                    selBoxClickY = tempSelBox.y1-((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom);
                    selBoxDragState = 3;
                    selBoxDragClick = true;

                // Bottom
                } else if (gui::InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y,
                drawSelBox.x1, drawSelBox.y2, drawSelBox.x2, drawSelBox.y2+16)) {

                    selBoxClickY = tempSelBox.y2-((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom);
                    selBoxDragState = 4;
                    selBoxDragClick = true;

                // Top-left
                } else if (gui::InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y,
                drawSelBox.x1-16, drawSelBox.y1-16, drawSelBox.x1, drawSelBox.y1)) {

                    selBoxClickX = tempSelBox.x1-((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom);
                    selBoxClickY = tempSelBox.y1-((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom);
                    selBoxDragState = 5;
                    selBoxDragClick = true;

                // Top-right
                } else if (gui::InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y,
                drawSelBox.x2, drawSelBox.y1-16, drawSelBox.x2+16, drawSelBox.y1)) {

                    selBoxClickX = tempSelBox.x2-((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom);
                    selBoxClickY = tempSelBox.y1-((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom);
                    selBoxDragState = 6;
                    selBoxDragClick = true;

                // Bottom-left
                } else if (gui::InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y,
                drawSelBox.x1-16, drawSelBox.y2, drawSelBox.x1, drawSelBox.y2+16)) {

                    selBoxClickX = tempSelBox.x1-((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom);
                    selBoxClickY = tempSelBox.y2-((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom);
                    selBoxDragState = 7;
                    selBoxDragClick = true;

                // Bottom-right
                } else if (gui::InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y,
                drawSelBox.x2, drawSelBox.y2, drawSelBox.x2+16, drawSelBox.y2+16)) {

                    selBoxClickX = tempSelBox.x2-((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom);
                    selBoxClickY = tempSelBox.y2-((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom);
                    selBoxDragState = 8;
                    selBoxDragClick = true;

                }

            } else if (gui::InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y,
            -sheetScrollX, -sheetScrollY, -sheetScrollX+sheetXdim-1, -sheetScrollY+sheetYdim-1)) {

                selBox.x1 = (gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom;
                selBox.y1 = (gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom;
                selBoxClick = true;

            }

        }

        // For sprite region selection
        if (selBoxClick) {

            selBox.x2 = (gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom;
            selBox.y2 = (gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom;

            if (!gdl::Mouse::MouseInfo.left) {

                if (selBox.x2 > selBox.x1) {
                    tempSelBox.x1 = selBox.x1;
                    tempSelBox.x2 = selBox.x2;
                } else {
                    tempSelBox.x1 = selBox.x2;
                    tempSelBox.x2 = selBox.x1;
                }

                if (selBox.y2 > selBox.y1) {
                    tempSelBox.y1 = selBox.y1;
                    tempSelBox.y2 = selBox.y2;
                } else {
                    tempSelBox.y1 = selBox.y2;
                    tempSelBox.y2 = selBox.y1;
                }

                selBoxClick = false;
                glfwPostEmptyEvent();

            }

        }


        // Selection drag
        if (selBoxDragClick) {

            switch(selBoxDragState) {
            case 0:
                tempSelBox.x1 = ((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom)+selBoxClickX;
                tempSelBox.y1 = ((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom)+selBoxClickY;
                tempSelBox.x2 = tempSelBox.x1 + selBoxWidth;
                tempSelBox.y2 = tempSelBox.y1 + selBoxHeight;
                break;
            case 1: // Left
                tempSelBox.x1 = ((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom)+selBoxClickX;
                break;
            case 2: // Right
                tempSelBox.x2 = ((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom)+selBoxClickX;
                break;
            case 3: // Top
                tempSelBox.y1 = ((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom)+selBoxClickY;
                break;
            case 4: // Bottom
                tempSelBox.y2 = ((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom)+selBoxClickY;
                break;
            case 5: // Top-left
                tempSelBox.x1 = ((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom)+selBoxClickX;
                tempSelBox.y1 = ((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom)+selBoxClickY;
                break;
            case 6: // Top-right
                tempSelBox.x2 = ((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom)+selBoxClickX;
                tempSelBox.y1 = ((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom)+selBoxClickY;
                break;
            case 7: // Bottom-left
                tempSelBox.x1 = ((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom)+selBoxClickX;
                tempSelBox.y2 = ((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom)+selBoxClickY;
                break;
            case 8: // Bottom-right
                tempSelBox.x2 = ((gdl::Mouse::MouseInfo.x+sheetScrollX)/Sprite::MapperZoom)+selBoxClickX;
                tempSelBox.y2 = ((gdl::Mouse::MouseInfo.y+sheetScrollY)/Sprite::MapperZoom)+selBoxClickY;
                break;
            }

            if (gdl::Mouse::MouseInfo.left == false)
                selBoxDragClick = false;

        }




        gdl::PrepDisplay();
        Checks::Draw(0, 0, gdl::ScreenXres, gdl::ScreenYres, gdl::Color::White);

        Sheet::SheetImage[sheetFile]->Put(-sheetScrollX, -sheetScrollY, gdl::Color::White, 0, 0, Sprite::MapperZoom);


        if (selBoxClick) {

            short selXpos,selXsize;
            short selYpos,selYsize;

            if (selBox.x2 > selBox.x1) {
                selXpos = (selBox.x1*Sprite::MapperZoom)-sheetScrollX;
                selXsize = ((selBox.x2-selBox.x1)*Sprite::MapperZoom)+(Sprite::MapperZoom-1);
            } else {
                selXpos = (selBox.x2*Sprite::MapperZoom)-sheetScrollX;
                selXsize = ((selBox.x1-selBox.x2)*Sprite::MapperZoom)+(Sprite::MapperZoom-1);
            }

            if (selBox.y2 > selBox.y1) {
                selYpos = (selBox.y1*Sprite::MapperZoom)-sheetScrollY;
                selYsize = ((selBox.y2-selBox.y1)*Sprite::MapperZoom)+(Sprite::MapperZoom-1);
            } else {
                selYpos = (selBox.y2*Sprite::MapperZoom)-sheetScrollY;
                selYsize = ((selBox.y1-selBox.y2)*Sprite::MapperZoom)+(Sprite::MapperZoom-1);
            }

            gdl::DrawBox(selXpos, selYpos, selXpos+selXsize, selYpos+selYsize, gdl::Color::Yellow);

        } else {

            SelBoxStruct drawSelBox;

            drawSelBox.x1 = (tempSelBox.x1*Sprite::MapperZoom)-sheetScrollX;
            drawSelBox.y1 = (tempSelBox.y1*Sprite::MapperZoom)-sheetScrollY;
            drawSelBox.x2 = ((tempSelBox.x2*Sprite::MapperZoom)-sheetScrollX)+(Sprite::MapperZoom-1);
            drawSelBox.y2 = ((tempSelBox.y2*Sprite::MapperZoom)-sheetScrollY)+(Sprite::MapperZoom-1);

            gdl::DrawLine(drawSelBox.x1-16, drawSelBox.y1, drawSelBox.x2+16, drawSelBox.y1, gdl::Color::White);
            gdl::DrawLine(drawSelBox.x1-16, drawSelBox.y2, drawSelBox.x2+16, drawSelBox.y2, gdl::Color::White);
            gdl::DrawLine(drawSelBox.x1, drawSelBox.y1-16, drawSelBox.x1, drawSelBox.y2+16, gdl::Color::White);
            gdl::DrawLine(drawSelBox.x2, drawSelBox.y1-16, drawSelBox.x2, drawSelBox.y2+16, gdl::Color::White);

            gdl::DrawBox(drawSelBox.x1-16, drawSelBox.y1-16, drawSelBox.x2+16, drawSelBox.y2+16, gdl::Color::Yellow);

        }


        gdl::Display();

    }

    glfwPostEmptyEvent();

}

void Sprite::DeleteSpriteButtonCallback(int index) {

    if ((Sprite::SpriteList.CurrentSprite < 0) || (Sprite::SpriteList.CurrentSprite > Sprite::NumSprites))
        return;

    Sprite::DeleteEntry(Sprite::SpriteList.CurrentSprite);

    if (Sprite::SpriteList.CurrentSprite > Sprite::NumSprites-1)
        Sprite::SpriteList.CurrentSprite = Sprite::NumSprites-1;

    changesMade = true;

}

Sprite::SpriteList::SpriteList(int x, int y, int width, int height) {

    SpriteList::x = x;
    SpriteList::y = y;
    SpriteList::width = width;
    SpriteList::height = height;
    SpriteList::CurrentSprite = -1;

    SpriteList::ScrollBar.disabled = true;

}

void Sprite::SpriteList::SetPos(int x, int y) {

    SpriteList::x = x;
    SpriteList::y = y;

}

void Sprite::SpriteList::SetSize(int width, int height) {

    SpriteList::width = width;
    SpriteList::height = height;

}

void Sprite::SpriteList::DoSpriteList() {

    gui::DrawPanel(
        SpriteList::x, SpriteList::y,
        SpriteList::x+(SpriteList::width-21), SpriteList::y+(SpriteList::height-1),
        gdl::Color::White, gui::PanelStyle::InPanel
    );


    if ((68*Sprite::NumSprites) > (SpriteList::height-2)) {

        SpriteList::ScrollBar.maxVal = (70*Sprite::NumSprites+1)-(SpriteList::height-1);
        SpriteList::ScrollBar.disabled = false;

    } else {

        SpriteList::ScrollBar.maxVal = 0;
        SpriteList::ScrollBar.value = 0;
        SpriteList::ScrollBar.disabled = true;

    }


    if (gui::InBox(gdl::Mouse::MouseInfo.x, gdl::Mouse::MouseInfo.y, SpriteList::x+1, SpriteList::y+1, SpriteList::x+(SpriteList::width-22), SpriteList::y+(SpriteList::height-2))) {

        SpriteList::ScrollBar.value -= 16*gdl::Mouse::MouseInfo.yScroll;

        if (SpriteList::ScrollBar.value < 0)
            SpriteList::ScrollBar.value = 0;
        if (SpriteList::ScrollBar.value > SpriteList::ScrollBar.maxVal)
            SpriteList::ScrollBar.value = SpriteList::ScrollBar.maxVal;

        if (gdl::Mouse::MouseInfo.leftClick) {

            int sel = (((gdl::Mouse::MouseInfo.y-SpriteList::y)-1)+SpriteList::ScrollBar.value)/70;

            if (sel < Sprite::NumSprites)
                SpriteList::CurrentSprite = sel;

        }

    }

    if (Sprite::NumSprites == 0) {
        SpriteList::CurrentSprite = -1;
    } else {
        if (SpriteList::CurrentSprite < 0)
            SpriteList::CurrentSprite = 0;
    }


    SpriteList::ScrollBar.SetPos(SpriteList::x+(SpriteList::width-18), SpriteList::y);
    SpriteList::ScrollBar.SetSize(17, SpriteList::height);
    SpriteList::ScrollBar.DoScrollBar();


    gdl::SetClipBox(SpriteList::x+1, SpriteList::y+1, SpriteList::x+(SpriteList::width-22), SpriteList::y+(SpriteList::height-2));

    int iy = (SpriteList::y+1)-(SpriteList::ScrollBar.value%70);
    int startIndex = SpriteList::ScrollBar.value/70;
    char sheetIndex[32];

    for(int i=startIndex; i<startIndex+((((SpriteList::height-2)+69)/70)+1); i++) {

        if (i >= Sprite::NumSprites)
            break;

        if (Sprite::Sprites[i].sheet == -1) {
            sprintf(sheetIndex, "< undef >");
        } else {
            sprintf(sheetIndex, "%d", Sprite::Sprites[i].sheet);
        }

        if (i == SpriteList::CurrentSprite) {

            gdl::DrawBoxF(SpriteList::x+1, iy, SpriteList::x+(SpriteList::width-22), iy+69, gdl::Color::Blue);
            font.Printf((SpriteList::x+3)+68, iy+2, 1.f, gdl::Color::White, "Sprite #%d", i);
            font.Printf((SpriteList::x+3)+68, iy+11, 1.f, gdl::Color::White, "Size: %dx%d", Sprite::Sprites[i].w, Sprite::Sprites[i].h);
            font.Printf((SpriteList::x+3)+68, iy+20, 1.f, gdl::Color::White, "Pivot: %dx%d", Sprite::Sprites[i].px, Sprite::Sprites[i].py);
            font.Printf((SpriteList::x+3)+68, iy+29, 1.f, gdl::Color::White, "Sheet: %s", sheetIndex);
            font.Printf((SpriteList::x+3)+68, iy+58, 1.f, gdl::Color::White, "%s", Sprite::Sprites[i].name);
            gdl::DrawLine((SpriteList::x+3)+68, iy+67, (SpriteList::x+3)+186, iy+67, gdl::Color::White);

        } else {

            font.Printf((SpriteList::x+3)+68, iy+2, 1.f, gdl::Color::Black, "Sprite #%d", i);
            font.Printf((SpriteList::x+3)+68, iy+11, 1.f, gdl::Color::Black, "Size: %dx%d", Sprite::Sprites[i].w, Sprite::Sprites[i].h);
            font.Printf((SpriteList::x+3)+68, iy+20, 1.f, gdl::Color::Black, "Pivot: %d,%d", Sprite::Sprites[i].px, Sprite::Sprites[i].py);
            font.Printf((SpriteList::x+3)+68, iy+29, 1.f, gdl::Color::Black, "Sheet: %s", sheetIndex);
            font.Printf((SpriteList::x+3)+68, iy+58, 1.f, gdl::Color::Black, "%s", Sprite::Sprites[i].name);
            gdl::DrawLine((SpriteList::x+3)+68, iy+67, (SpriteList::x+3)+186, iy+67, gdl::Color::Black);

        }

        gui::DrawPanel(SpriteList::x+3, iy+2, (SpriteList::x+3)+65, iy+67, gdl::Color::White, gui::PanelStyle::InFrame);
        Checks::Draw(SpriteList::x+4, iy+3, (SpriteList::x+4)+64, (iy+1)+66, gdl::Color::White);

        if (Sprite::Sprites[i].sheet >= 0) {

            float previewScale = 1.f;

            if (Sprite::Sprites[i].w > Sprite::Sprites[i].h) {
                if (Sprite::Sprites[i].w > 64) {
                    previewScale = 1.f - (float)(Sprite::Sprites[i].w-64)/Sprite::Sprites[i].w;
                }
            } else {
                if (Sprite::Sprites[i].h > 64) {
                    previewScale = 1.f - (float)(Sprite::Sprites[i].h-64)/Sprite::Sprites[i].h;
                }
            }

            Sprite::DrawSprite(SpriteList::x+4, iy+3, 0, 0, previewScale, false, &Sprite::Sprites[i]);

        }

        iy += 70;

    }

    gdl::ResetClipBox();

    //gdl::DrawBox(SpriteList::x+1, SpriteList::y+1, SpriteList::x+(SpriteList::width-22), SpriteList::y+(SpriteList::height-2), gdl::Color::White);

}

void Sprite::SpriteList::ScrollToSelection() {

    if ((70*SpriteList::CurrentSprite) < SpriteList::ScrollBar.value) {

        SpriteList::ScrollBar.value = 70*SpriteList::CurrentSprite;

    } else if (((70*SpriteList::CurrentSprite)+69) > (SpriteList::ScrollBar.value+(SpriteList::height-3))) {

        SpriteList::ScrollBar.value = ((70*SpriteList::CurrentSprite)+69)-(SpriteList::height-3);

    }

}


bool OpenFileDialog(char* fileNameBuff, const char* filters, int filterIndex, const char* initialDir) {

    OPENFILENAME dialog;
    memset(&dialog, 0x00, sizeof(OPENFILENAME));

    char CurrentDir[MAX_PATH];
    memset(CurrentDir, 0x00, MAX_PATH);
    GetCurrentDirectory(MAX_PATH, CurrentDir);
    strcat(CurrentDir, "\\");

    dialog.lStructSize     = sizeof(OPENFILENAME); // SEE NOTE BELOW
    dialog.hwndOwner       = glfwGetWin32Window(gdl::pc::WindowContext);
    dialog.lpstrFilter     = filters;
    dialog.nFilterIndex    = filterIndex;
    dialog.lpstrFile       = fileNameBuff;
    dialog.nMaxFile        = MAX_PATH;
    dialog.Flags           = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    dialog.lpstrInitialDir = CurrentDir;

    bool result = GetOpenFileName(&dialog);
    SetCurrentDirectory(CurrentDir);
    return(result);

}

bool SaveFileDialog(char* fileNameBuff, const char* filters, int filterIndex, const char* initialDir) {

    OPENFILENAME dialog;
    memset(&dialog, 0x00, sizeof(OPENFILENAME));

    char CurrentDir[MAX_PATH];
    memset(CurrentDir, 0x00, MAX_PATH);
    GetCurrentDirectory(MAX_PATH, CurrentDir);
    strcat(CurrentDir, "\\");

    dialog.lStructSize     = sizeof(OPENFILENAME); // SEE NOTE BELOW
    dialog.hwndOwner       = glfwGetWin32Window(gdl::pc::WindowContext);
    dialog.lpstrFilter     = filters;
    dialog.nFilterIndex    = filterIndex;
    dialog.lpstrFile       = fileNameBuff;
    dialog.nMaxFile        = MAX_PATH;
    dialog.Flags           = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

    if (initialDir != NULL)
        dialog.lpstrInitialDir = initialDir;
    else
        dialog.lpstrInitialDir = CurrentDir;

    bool result = GetSaveFileName(&dialog);
    filterIndex = dialog.nFilterIndex;
    SetCurrentDirectory(CurrentDir);
    return(result);

}


void StripPathName(char *outName, const char *pathName) {

    for(int i=strlen(pathName)-1; i>=0; i--) {

        if (pathName[i] == '\\') {
            strcpy(outName, &pathName[i+1]);
            return;
        }

    }

    strcpy(outName, pathName);

}
