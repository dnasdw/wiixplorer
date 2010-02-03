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
 * MusicLoader.cpp
 *
 * for WiiXplorer 2009
 ***************************************************************************/
#include <gctypes.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "libwiigui/gui.h"
#include "libwiigui/gui_bgm.h"
#include "Controls/MainWindow.h"
#include "Prompts/PromptWindows.h"
#include "Prompts/ProgressWindow.h"
#include "menu.h"
#include "main.h"
#include "FileOperations/fileops.h"
#include "Language/gettext.h"
#include "sys.h"


/****************************************************************************
* LoadMusic
***************************************************************************/
void LoadMusic(const char *filepath)
{
    bool result = GuiBGM::Instance()->Load(filepath, false);

    if(result)
    {
        GuiBGM::Instance()->Play();
        GuiBGM::Instance()->ParsePath(filepath);
    }
}
