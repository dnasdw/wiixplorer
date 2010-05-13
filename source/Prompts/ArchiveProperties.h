/***************************************************************************
 * Copyright (C) 2009
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 * ArchiveProperties.cpp
 *
 * for WiiXplorer 2009
 ***************************************************************************/
 #ifndef __ARCHIVEPROPERTIES_H_
 #define __ARCHIVEPROPERTIES_H_

#include <gctypes.h>
#include <unistd.h>

#include "libwiigui/gui.h"
#include "ArchiveOperations/ArchiveBrowser.h"
#include "Memory/Resources.h"

class ArchiveProperties : public GuiWindow
{
    public:
        ArchiveProperties(ArchiveFileStruct * File);
        ~ArchiveProperties();
        int GetChoice();
    private:
        void OnButtonClick(GuiElement *sender, int pointer, POINT p);

        int choice;
        ArchiveFileStruct ArchiveFile;
        u64 foldersize;
        u64 oldfoldersize;
        u32 filecount;

        GuiImage * dialogBoxImg;
        GuiImage * TitleImg;
        GuiImage * arrowUpImg;
        GuiImage * arrowUpImgOver;

        GuiImageData * dialogBox;
        GuiImageData * titleData;
        GuiImageData * arrowUp;
        GuiImageData * arrowUpOver;

        GuiSound * btnClick;

        GuiText * TitleTxt;
        GuiText * filepathTxt;
        GuiText * filecountTxt;
        GuiText * filecountTxtVal;
        GuiText * filesizeTxt;
        GuiText * filesizeTxtVal;
        GuiText * filesizeCompTxt;
        GuiText * filesizeCompTxtVal;
        GuiText * filetypeTxt;
        GuiText * filetypeTxtVal;
        GuiText * last_modifTxt;
        GuiText * last_modifTxtVal;

        GuiButton * CloseBtn;

        SimpleGuiTrigger * trigA;
        GuiTrigger * trigB;
};

#endif
