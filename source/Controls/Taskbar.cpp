/****************************************************************************
 * Copyright (C) 2009-2011 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include <time.h>
#include "menu.h"
#include "Taskbar.h"
#include "Controls/Application.h"
#include "Memory/Resources.h"
#include "Prompts/PromptWindows.h"
#include "Prompts/ProgressWindow.h"
#include "Prompts/PopUpMenu.h"
#include "DeviceControls/DeviceHandler.hpp"
#include "DeviceControls/PartitionFormatterGUI.hpp"
#include "Launcher/Applications.h"
#include "Launcher/Channels.h"
#include "Launcher/OperaBooter.hpp"
#include "network/networkops.h"
#include "SoundOperations/MusicPlayer.h"
#include "input.h"
#include "sys.h"

#include "Menus/Explorer.h"
#include "Menus/Settings/MainSettingsMenu.h"

enum
{
	APPS = 0,
	CHANNELS,
	URLS,
	BOOTMII,
	FORMATTER,
	SETTINGS,
	FTPSERVER,
	REMOUNT,
	RESTART,
	EXIT,
};

Taskbar *Taskbar::instance = NULL;

Taskbar::Taskbar()
	: GuiFrame(0, 0)
{
	WifiData = NULL;
	WifiImg = NULL;

	taskbarImgData = Resources::GetImageData("taskbar.png");
	taskbarImg = new GuiImage(taskbarImgData);

	width = taskbarImg->GetWidth();
	height = taskbarImg->GetHeight();

	timeTxt = new GuiText((char *) NULL, 20, (GXColor) {40, 40, 40, 255});
	timeTxt->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
	timeTxt->SetPosition(width-82, -1);
	timeTxt->SetFont(Resources::GetFile("clock.ttf"), Resources::GetFileSize("clock.ttf"));

	soundClick = Resources::GetSound("button_click.wav");
	soundOver = Resources::GetSound("button_over.wav");
	trigA = new SimpleGuiTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);

	startBtn = new PictureButton("start.png", "start_over.png", soundClick, soundOver);
	startBtn->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
	startBtn->SetPosition(23, -2);
	startBtn->SetSelectable(false);
	startBtn->SetTrigger(trigA);
	startBtn->Clicked.connect(this, &Taskbar::OnStartButtonClick);

	HeadPhonesData = Resources::GetImageData("player_icon.png");
	HeadPhonesImg = new GuiImage(HeadPhonesData);
	Musicplayer = new GuiButton(HeadPhonesData->GetWidth(), HeadPhonesData->GetHeight());
	Musicplayer->SetImage(HeadPhonesImg);
	Musicplayer->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
	Musicplayer->SetTrigger(trigA);
	Musicplayer->SetPosition(458, 0);
	Musicplayer->SetEffectGrow();
	Musicplayer->Clicked.connect(this, &Taskbar::OnMusicPlayerClick);

	Append(taskbarImg);
	Append(startBtn);
	Append(Musicplayer);
	Append(timeTxt);

	SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	SetPosition(0, -15);
}

Taskbar::~Taskbar()
{
	RemoveAll();

	Resources::Remove(taskbarImgData);
	Resources::Remove(HeadPhonesData);
	Resources::Remove(WifiData);

	delete taskbarImg;
	delete HeadPhonesImg;
	delete WifiImg;

	delete startBtn;
	delete Musicplayer;
	delete timeTxt;

	delete trigA;

	Resources::Remove(soundClick);
	Resources::Remove(soundOver);

	for(u32 i = 0; i < Tasks.size(); i++)
		delete Tasks[i];
}

void Taskbar::AddTask(Task * t)
{
	t->SetPosition(105+Tasks.size()*100, 0);
	t->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Tasks.push_back(t);
	Append(t);
}

void Taskbar::RemoveTask(Task * t)
{
	for(u32 i = 0; i < Tasks.size(); i++)
	{
		if(Tasks[i] == t)
		{
			Remove(Tasks[i]);
			Application::Instance()->PushForDelete(Tasks[i]);
			Tasks.erase(Tasks.begin()+i);
			break;
		}
	}
}

void Taskbar::Draw()
{
	if(frameCount % 60 == 0) //! Update time value every sec
	{
		char timetxt[20];
		time_t currenttime = time(0);
		struct tm * timeinfo = localtime(&currenttime);

		if(Settings.ClockMode == 0)
			strftime(timetxt, sizeof(timetxt), "%H:%M:%S", timeinfo);
		else
			strftime(timetxt, sizeof(timetxt), "%I:%M:%S", timeinfo);

		timeTxt->SetText(timetxt);
	}

	if(WifiImg == NULL && IsNetworkInit())
	{
		WifiData = Resources::GetImageData("network_wireless.png");
		WifiImg = new GuiImage(WifiData);
		WifiImg->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
		WifiImg->SetPosition(418, 0);
		Append(WifiImg);
	}

	GuiFrame::Draw();
}

void Taskbar::OnStartButtonClick(GuiButton *sender, int pointer, const POINT &p UNUSED)
{
	PopUpMenu *StartMenu = new PopUpMenu(screenwidth/2-width/2-2, Settings.ShowFormatter ? 75 : 105);
	StartMenu->AddItem(tr("Apps"), "apps.png", true);
	StartMenu->AddItem(tr("Channels"), "channels.png", true);
	StartMenu->AddItem(tr("URL List"), "opera_icon.png", true);
	StartMenu->AddItem(tr("BootMii"), "BootMii.png");
	if(Settings.ShowFormatter)
		StartMenu->AddItem(tr("Formatter"), "usbstorage.png");
	StartMenu->AddItem(tr("Settings"), "settings.png");
	StartMenu->AddItem(tr("FTP Server"), "network.png");
	StartMenu->AddItem(tr("Remount"), "refresh.png");
	StartMenu->AddItem(tr("Restart"), "system_restart.png");
	StartMenu->AddItem(tr("Exit"), "system_log_out.png");
	StartMenu->Finish();
	StartMenu->ItemClicked.connect(this, &Taskbar::OnStartmenuItemClick);

	//! Finish update with disabled sender to close opened menus
	sender->SetState(STATE_DISABLED);
	Application::Instance()->Update(&userInput[pointer]);
	sender->SetState(STATE_DEFAULT);

	Application::Instance()->UpdateOnly(StartMenu);
	Application::Instance()->Append(StartMenu);
}

void Taskbar::OnStartmenuItemClick(PopUpMenu *menu, int item)
{
	if(item >= FORMATTER && !Settings.ShowFormatter)
		item++;

	if(item < 0 || item >= BOOTMII)
	{
		Application::Instance()->Remove(menu);
		Application::Instance()->UpdateOnly(NULL);
		menu->Close();
	}

	if (item == APPS)
	{
		PopUpMenu *AppsMenu = new PopUpMenu(0, 0);

		Applications *Apps = new Applications(Settings.AppPath);

		for (int i = 0; i < Apps->Count(); i++)
			AppsMenu->AddItem(Apps->GetName(i));

		AppsMenu->Finish();
		AppsMenu->SetUserData(Apps);
		AppsMenu->ItemClicked.connect(this, &Taskbar::OnAppsMenuClick);
		menu->OpenSubMenu(item, AppsMenu);
	}
	else if (item == CHANNELS)
	{
		PopUpMenu *ChannelsMenu = new PopUpMenu(0, 0);

		for (int i = 0; i < Channels::Instance()->Count(); i++)
			ChannelsMenu->AddItem(Channels::Instance()->GetName(i));

		ChannelsMenu->Finish();
		ChannelsMenu->ItemClicked.connect(this, &Taskbar::OnChannelsMenuClick);
		menu->OpenSubMenu(item, ChannelsMenu);
	}
	else if (item == URLS)
	{
		PopUpMenu * LinksMenu = new PopUpMenu(0, 0);
		OperaBooter *Booter = new OperaBooter(Settings.LinkListPath);

		LinksMenu->AddItem(tr("Add Link"));

		for (int i = 0; i < Booter->GetCount(); i++)
		{
			const char * name = Booter->GetName(i);
			if(name)
				LinksMenu->AddItem(name);
		}

		LinksMenu->Finish();
		LinksMenu->SetUserData(Booter);
		LinksMenu->ItemClicked.connect(this, &Taskbar::OnUrlsMenuClick);
		menu->OpenSubMenu(item, LinksMenu);
	}
	else if (item == BOOTMII)
	{
		int res = WindowPrompt(tr("Do you want to start BootMii?"), 0, tr("Yes"), tr("No"));
		if(res)
		{
			ExitApp();
			if(IOS_ReloadIOS(254) < 0)
				RebootApp();
		}
	}
	else if (item == FORMATTER)
	{
		PartitionFormatterGui * PartFormatter = new PartitionFormatterGui();
		PartFormatter->DimBackground(true);
		PartFormatter->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
		Application::Instance()->UpdateOnly(PartFormatter);
		Application::Instance()->Append(PartFormatter);
	}
	else if (item == SETTINGS)
	{
		Application::Instance()->GetExplorer()->hide();

		MainSettingsMenu *menu = new MainSettingsMenu(Application::Instance()->GetExplorer());
		Application::Instance()->Append(menu);
	}
	else if (item == FTPSERVER)
	{
		//menu = MENU_FTP;
	}
	else if (item == REMOUNT)
	{
		if (WindowPrompt(tr("Do you want to remount the devices?"), 0, tr("Yes"), tr("Cancel")))
		{
			//StartProgress(tr("Remounting all devices."), AUTO_THROBBER);
			//ShowProgress(0, 1, tr("Please wait..."));
			DeviceHandler::Instance()->UnMountAll();
			DeviceHandler::Instance()->MountAll();
			//StopProgress();
		}
	}
	else if (item == RESTART)
	{
		if (WindowPrompt(tr("Do you want to reboot WiiXplorer?"), 0, tr("Yes"), tr("Cancel")))
		{
			RebootApp();
		}
	}
	else if (item == EXIT)
	{
		if (WindowPrompt(tr("Do you want to exit WiiXplorer?"), 0, tr("Yes"), tr("Cancel")))
		{
			Application::Instance()->quit();
		}
	}
}

void Taskbar::OnAppsMenuClick(PopUpMenu *menu, int item)
{
	Applications *Apps = (Applications *) menu->GetUserData();

	if(item >= 0)
	{
		PopUpMenu *parent = (PopUpMenu *) menu->GetParent();
		Application::Instance()->Remove(parent);
		Application::Instance()->UpdateOnly(NULL);
		parent->Close();

		if(WindowPrompt(tr("Do you want to start the app?"), Apps->GetName(item), tr("Yes"), tr("Cancel")))
			Apps->Launch(item);
	}

	menu->Close();
	delete Apps;
}

void Taskbar::OnChannelsMenuClick(PopUpMenu *menu, int item)
{
	if(item >= 0)
	{
		PopUpMenu *parent = (PopUpMenu *) menu->GetParent();
		Application::Instance()->Remove(parent);
		Application::Instance()->UpdateOnly(NULL);
		parent->Close();

		if(WindowPrompt(tr("Do you want to start the channel?"), Channels::Instance()->GetName(item), tr("Yes"), tr("Cancel")))
			Channels::Instance()->Launch(item);
	}

	menu->Close();
}

void Taskbar::OnUrlsMenuClick(PopUpMenu *menu, int item)
{
	OperaBooter *Booter = (OperaBooter *) menu->GetUserData();

	if(item >= 0)
	{
		PopUpMenu *parent = (PopUpMenu *) menu->GetParent();
		Application::Instance()->Remove(parent);
		Application::Instance()->UpdateOnly(NULL);
		parent->Close();
	}

	if(item == 0)
		Booter->AddLink();

	else if (item > 0)
	{
		int res = WindowPrompt(tr("How should this URL be opened?"), Booter->GetLink(item-1), tr("Internet Channel"), tr("Download Link"), tr("Remove Link"), tr("Cancel"));
		if(res == 1)
			Booter->Launch(item-1);
		else if(res == 2)
			Booter->DownloadFile(item-1);
		else if(res == 3)
			Booter->RemoveLink(item-1);
	}
	menu->Close();
	delete Booter;
}

void Taskbar::OnMusicPlayerClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	MusicPlayer::Instance()->Show();
}
