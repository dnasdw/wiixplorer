/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * menu.h
 * Menu flow routines - handles all menu logic
 ***************************************************************************/

#ifndef _MENU_H_
#define _MENU_H_

#include <ogcsys.h>
#include "main.h"

#define THREAD_SLEEP    100
#define KBSIZE          1024.0
#define MBSIZE          1048576.0
#define GBSIZE          1073741824.0

void InitThreads();
void ExitGUIThreads();
void MainMenu (int menuitem);
void ResumeGui();
void HaltGui();

enum
{
	MENU_EXIT = -1,
	MENU_NONE,
	MENU_SETTINGS,
	MENU_BROWSE_DEVICE,
	MENU_SMB_SETTINGS,
	MENU_FTP_SETTINGS,
	MENU_LANGUAGE_BROWSE,
	MENU_FTP,
	MENU_NETWORK_SETTINGS
};

#endif
