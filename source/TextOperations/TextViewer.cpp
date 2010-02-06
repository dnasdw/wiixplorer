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
 * TextViewer.cpp
 *
 * for WiiXplorer 2009
 ***************************************************************************/
#include <gctypes.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "libwiigui/gui.h"
#include "Controls/MainWindow.h"
#include "Prompts/PromptWindows.h"
#include "Prompts/ProgressWindow.h"
#include "menu.h"
#include "FileOperations/fileops.h"
#include "sys.h"
#include "TextEditor.h"


/****************************************************************************
* TextViewer (temporary menu)
***************************************************************************/
void TextViewer(const char *filepath)
{
    bool exitwindow = false;

    u8 *file = NULL;
    u64 filesize = 0;

    int ret = LoadFileToMemWithProgress(tr("Loading file:"), filepath, &file, &filesize);
    if(ret < 0)
    {
        return;
    }

    //To check if enough memory available for strcpy in GuiText
    char * filetext = (char*) malloc(filesize);
    if(!filetext)
    {
        free(file);
        ShowError(tr("Not enough memory."));
        return;
    }

    snprintf(filetext, filesize, "%s", (char *) file);

    free(file);
    file = NULL;

    TextEditor * Editor = new TextEditor(filetext, 9, filepath);
    Editor->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
    Editor->SetPosition(0, 0);

    free(filetext);
    filetext = NULL;

    MainWindow::Instance()->SetState(STATE_DISABLED);
    MainWindow::Instance()->SetDim(true);
    MainWindow::Instance()->Append(Editor);
    MainWindow::Instance()->ChangeFocus(Editor);

    while(!exitwindow)
    {
        usleep(100);

        if(shutdown)
            Sys_Shutdown();
        else if(reset)
            Sys_Reboot();

        else if(Editor->GetState() == STATE_CLOSED)
            exitwindow = true;
    }

    delete Editor;
    Editor = NULL;

    MainWindow::Instance()->SetState(STATE_DEFAULT);
    MainWindow::Instance()->SetDim(false);
    ResumeGui();
}
