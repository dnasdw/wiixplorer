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
 * Explorer.cpp
 *
 * for WiiXplorer 2009
 ***************************************************************************/
#include "Explorer.h"
#include "menu.h"
#include "Controls/MainWindow.h"
#include "Controls/Clipboard.h"
#include "Controls/Taskbar.h"
#include "Memory/Resources.h"
#include "FileOperations/ProcessChoice.h"
#include "Prompts/PromptWindows.h"
#include "Prompts/ProgressWindow.h"
#include "FileStartUp/FileStartUp.h"
#include "FileOperations/filebrowser.h"
#include "FileOperations/fileops.h"
#include "sys.h"

Explorer::Explorer()
    :GuiWindow(0, 0)
{
    this->Setup();
}

Explorer::Explorer(const char *path)
    :GuiWindow(0, 0)
{
    this->Setup();
    this->LoadPath(path);
}

Explorer::~Explorer()
{
    MainWindow::Instance()->ResumeGui();
    SetEffect(EFFECT_FADE, -50);
    while(this->GetEffect() > 0) usleep(100);

    MainWindow::Instance()->HaltGui();
    if(parentElement)
        ((GuiWindow *) parentElement)->Remove(this);

    RemoveAll();

	Resources::Remove(btnSoundClick);
	Resources::Remove(btnSoundOver);
	Resources::Remove(creditsImgData);
    Resources::Remove(Background);
    Resources::Remove(Address);
    Resources::Remove(Refresh);
    Resources::Remove(sdstorage);
    Resources::Remove(usbstorage);
    Resources::Remove(usbstorage_blue);
    Resources::Remove(networkstorage);
	Resources::Remove(ftpstorage);
    Resources::Remove(dvd_ImgData);

    delete BackgroundImg;
    delete creditsImg;
    delete deviceImg;
    delete AdressbarImg;
    delete RefreshImg;

    delete AdressText;

    delete RefreshBtn;
    delete CreditsBtn;
    delete deviceSwitchBtn;
    delete Adressbar;
    delete clickmenuBtn;
    delete BackInDirBtn;

    delete trigA;
    delete trigPlus;
    delete trigBackInDir;

    if(Credits)
        delete Credits;
    if(Device_Menu)
        delete Device_Menu;
    if(RightClick)
        delete RightClick;

    if(iconBrowser)
        delete iconBrowser;
    if(listBrowser)
        delete listBrowser;

    if(ArcBrowser)
        delete ArcBrowser;

    delete DeviceBrowser;
}

void Explorer::Setup()
{
	menu = MENU_NONE;
    listBrowser = NULL;
    iconBrowser = NULL;
    fileBrowser = NULL;
    Device_Menu = NULL;
    ArcBrowser = NULL;
    RightClick = NULL;
    Credits = NULL;

    DeviceBrowser = new FileBrowser();
    CurBrowser = DeviceBrowser;

	SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	SetPosition(0, 50);

    trigA = new SimpleGuiTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigPlus = new GuiTrigger();
    trigPlus->SetButtonOnlyTrigger(-1, WiiControls.ContextMenuButton | ClassicControls.ContextMenuButton << 16, GCControls.ContextMenuButton);
	trigBackInDir = new GuiTrigger();
    trigBackInDir->SetButtonOnlyTrigger(-1, WiiControls.UpInDirectory | ClassicControls.UpInDirectory << 16, GCControls.UpInDirectory);

	btnSoundClick = Resources::GetSound("button_click.wav");
	btnSoundOver = Resources::GetSound("button_over.wav");

    creditsImgData = Resources::GetImageData("WiiXplorer.png");
	Background = Resources::GetImageData("bg_browser.png");
	Address = Resources::GetImageData("addressbar_textbox.png");
	Refresh = Resources::GetImageData("refresh2.png");

	sdstorage = Resources::GetImageData("sdstorage.png");
	usbstorage = Resources::GetImageData("usbstorage.png");
	usbstorage_blue = Resources::GetImageData("usbstorage_blue.png");
	networkstorage = Resources::GetImageData("networkstorage.png");
	ftpstorage = Resources::GetImageData("ftpstorage.png");
    dvd_ImgData = Resources::GetImageData("dvdstorage.png");

    width = Background->GetWidth();
    height = Background->GetHeight();
	BackgroundImg = new GuiImage(Background);

    if(Settings.BrowserMode == ICONBROWSER)
    {
        iconBrowser = new IconFileBrowser(CurBrowser, width, 252);
        fileBrowser = iconBrowser;
    }
    else
    {
        listBrowser = new ListFileBrowser(CurBrowser, width, 252);
        fileBrowser = listBrowser;
    }

	fileBrowser->SetPosition(0, 53);

    creditsImg = new GuiImage(creditsImgData);
	CreditsBtn = new GuiButton(creditsImgData->GetWidth(), creditsImgData->GetHeight());
	CreditsBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	CreditsBtn->SetPosition(fileBrowser->GetLeft()+235, fileBrowser->GetTop()+262);
	CreditsBtn->SetImage(creditsImg);
	CreditsBtn->SetSoundClick(btnSoundClick);
	CreditsBtn->SetSoundOver(btnSoundOver);
	CreditsBtn->SetTrigger(trigA);
	CreditsBtn->SetEffectGrow();
    CreditsBtn->Clicked.connect(this, &Explorer::OnButtonClick);

	deviceImg = new GuiImage(sdstorage);

	deviceSwitchBtn = new GuiButton(deviceImg->GetWidth(), deviceImg->GetHeight());
	deviceSwitchBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	deviceSwitchBtn->SetPosition(fileBrowser->GetLeft()+20, fileBrowser->GetTop()-38);
	deviceSwitchBtn->SetImage(deviceImg);
	deviceSwitchBtn->SetSoundClick(btnSoundClick);
	deviceSwitchBtn->SetSoundOver(btnSoundOver);
	deviceSwitchBtn->SetTrigger(trigA);
	deviceSwitchBtn->SetEffectGrow();
    deviceSwitchBtn->Clicked.connect(this, &Explorer::OnButtonClick);

    AdressText = new GuiText((char *) NULL, 20, (GXColor) {0, 0, 0, 255});
	AdressText->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	AdressText->SetPosition(18, 0);
	AdressText->SetMaxWidth(Address->GetWidth()-45-Refresh->GetWidth(), SCROLL_HORIZONTAL);
	AdressbarImg = new GuiImage(Address);
	Adressbar = new GuiButton(Address->GetWidth()-Refresh->GetWidth()-5, Address->GetHeight());
	Adressbar->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	Adressbar->SetPosition(fileBrowser->GetLeft()+62, fileBrowser->GetTop()-38);
	Adressbar->SetImage(AdressbarImg);
	Adressbar->SetLabel(AdressText);
	Adressbar->SetSoundClick(btnSoundClick);
	Adressbar->SetSoundOver(btnSoundOver);
	Adressbar->SetRumble(false);
	Adressbar->SetTrigger(trigA);

    RefreshImg = new GuiImage(Refresh);
    RefreshImg->SetScale(0.8);
    RefreshBtn = new GuiButton(Refresh->GetWidth(), Refresh->GetHeight());
    RefreshBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
    RefreshBtn->SetPosition(Adressbar->GetLeft()+Adressbar->GetWidth()-5, Adressbar->GetTop()+4);
    RefreshBtn->SetImage(RefreshImg);
	RefreshBtn->SetSoundClick(btnSoundClick);
	RefreshBtn->SetSoundOver(btnSoundOver);
	RefreshBtn->SetTrigger(trigA);
	RefreshBtn->SetEffectGrow();
    RefreshBtn->Clicked.connect(this, &Explorer::OnButtonClick);

	clickmenuBtn = new GuiButton(fileBrowser->GetWidth(), fileBrowser->GetHeight());
	clickmenuBtn->SetPosition(fileBrowser->GetLeft(), fileBrowser->GetTop());
	clickmenuBtn->SetTrigger(trigPlus);
    clickmenuBtn->Clicked.connect(this, &Explorer::OnButtonClick);

    BackInDirBtn = new GuiButton(0, 0);
    BackInDirBtn->SetTrigger(trigBackInDir);
    BackInDirBtn->Clicked.connect(this, &Explorer::BackInDirectory);

	Append(BackgroundImg);
	Append(clickmenuBtn);
	Append(CreditsBtn);
	Append(Adressbar);
	Append(RefreshBtn);
	Append(deviceSwitchBtn);
	Append(fileBrowser);
	Append(BackInDirBtn);

    SetEffect(EFFECT_FADE, 50);
}

int Explorer::LoadPath(const char * path)
{
    if(CurBrowser != DeviceBrowser)
        return -1;

	int filecount = DeviceBrowser->BrowsePath(path);
	if(filecount < 0)
	{
		int choice = WindowPrompt(tr("Error:"),
		tr("Unable to load path."),
		tr("Retry"),
		tr("Close"));

		if(choice)
			return LoadPath(path);
		else
			return -2;
	}

	DeviceBrowser->ResetMarker();
    AdressText->SetText(DeviceBrowser->GetCurrentPath());
    Settings.LastUsedPath.assign(DeviceBrowser->GetCurrentPath());
	SetDeviceImage();
	return filecount;
}

void Explorer::SetDeviceImage()
{
    if(CurBrowser != DeviceBrowser)
        return;

    const char * currentroot = DeviceBrowser->GetRootDir();
    if(strncmp(currentroot, DeviceName[SD], 2) == 0)
    {
        deviceImg->SetImage(sdstorage);
    }
    else if(strncmp(currentroot, DeviceName[USB1], 3) == 0)
    {
        const char * FSName = DeviceHandler::PathToFSName(DeviceBrowser->GetCurrentPath());

        if(FSName && strncmp(FSName, "NTF", 3) != 0)
            deviceImg->SetImage(usbstorage);
        else
            deviceImg->SetImage(usbstorage_blue);
    }
    else if(strncmp(currentroot, DeviceName[SMB1], 3) == 0)
    {
        deviceImg->SetImage(networkstorage);
    }
    else if(strncmp(currentroot, DeviceName[FTP1], 3) == 0)
    {
        deviceImg->SetImage(ftpstorage);
    }
	else if(strncmp(currentroot, DeviceName[DVD], 3) == 0)
    {
        deviceImg->SetImage(dvd_ImgData);
    }
}

int Explorer::GetMenuChoice()
{
    CheckBrowserChanges();
    CheckDeviceMenu();
    CheckRightClick();
    ShowCredits(Credits);

    if(Adressbar->GetState() == STATE_CLICKED)
    {
        char entered[1024];
        strcpy(entered, CurBrowser->GetCurrentPath());
        if(OnScreenKeyboard(entered, sizeof(entered)))
        {
            LoadPath(entered);
        }
        Adressbar->ResetState();
    }

	return menu;
}

void Explorer::CheckBrowserChanges()
{
    //!This will be changed later
	if(fileBrowser->GetState() == STATE_CLICKED)
    {
        fileBrowser->ResetState();
        // check corresponding browser entry
        if(CurBrowser->IsCurrentDir())
        {
            int result = CurBrowser->ChangeDirectory();
            if(result > 0)
            {
                fileBrowser->SetSelected(0);
                CurBrowser->SetPageIndex(0);
                CurBrowser->ResetMarker();
                fileBrowser->TriggerUpdate();
                AdressText->SetText(CurBrowser->GetCurrentPath());
                Settings.LastUsedPath.assign(DeviceBrowser->GetCurrentPath());
            }
            else if(result == 0)
            {
                if(!ArcBrowser)
                    return;

                CurBrowser = DeviceBrowser;
                fileBrowser->SetBrowser(CurBrowser);
                delete ArcBrowser;
                ArcBrowser = NULL;
                fileBrowser->TriggerUpdate();
                AdressText->SetTextf("%s", CurBrowser->GetCurrentPath());
            }
            else
            {
                ShowError(tr("Can't browse that path."));
                menu = MENU_BROWSE_DEVICE;
            }
        }
        else
        {
            char filepath[MAXPATHLEN];
            int result = 0;

            SetState(STATE_DISABLED);
            fileBrowser->SetTriggerUpdate(false);
            Taskbar::Instance()->SetTriggerUpdate(false);

            if(CurBrowser == ArcBrowser)
                result = ArcBrowser->ExecuteCurrent(filepath);

            else if(CurBrowser == DeviceBrowser)
                snprintf(filepath, sizeof(filepath), "%s", CurBrowser->GetCurrentSelectedFilepath());

            if(result >= 0)
                result = FileStartUp(filepath);

            SetState(STATE_DEFAULT);
            fileBrowser->SetTriggerUpdate(true);
            Taskbar::Instance()->SetTriggerUpdate(true);

            if(result == ARCHIVE)
            {
                if(ArcBrowser)
                    delete ArcBrowser;

                ArcBrowser = new ArchiveBrowser(filepath);
                CurBrowser = ArcBrowser;
                fileBrowser->SetBrowser(CurBrowser);
                AdressText->SetText(CurBrowser->GetCurrentPath());
            }
            else if(result == REFRESH_BROWSER)
            {
                CurBrowser->Refresh();
                fileBrowser->TriggerUpdate();
            }
            else if(result == RELOAD_BROWSER)
            {
                menu = MENU_BROWSE_DEVICE;
            }
        }
    }
}

void Explorer::CheckDeviceMenu()
{
    if(Device_Menu != NULL)
    {
        SetState(STATE_DISABLED);
        fileBrowser->SetTriggerUpdate(false);
        Taskbar::Instance()->SetTriggerUpdate(false);
        Append(Device_Menu);

        int device_choice = -1;
        while(device_choice == -1 && Device_Menu != NULL)
        {
            usleep(100);

            if(shutdown)
                Sys_Shutdown();
            else if(reset)
                Sys_Reboot();

            device_choice = Device_Menu->GetChoice();
        }
        delete Device_Menu;
        Device_Menu = NULL;

        if(device_choice >= 0)
        {
            if(ArcBrowser)
            {
                CurBrowser = DeviceBrowser;
                fileBrowser->SetBrowser(CurBrowser);
                delete ArcBrowser;
                ArcBrowser = NULL;
            }
            LoadPath(fmt("%s:/", DeviceName[device_choice]));
            fileBrowser->SetSelected(0);
            fileBrowser->TriggerUpdate();
            AdressText->SetText(CurBrowser->GetCurrentPath());
            Settings.LastUsedPath.assign(DeviceBrowser->GetCurrentPath());
        }
        SetState(STATE_DEFAULT);
        fileBrowser->SetTriggerUpdate(true);
        Taskbar::Instance()->SetTriggerUpdate(true);
    }
}

void Explorer::CheckRightClick()
{
    if(RightClick != NULL)
    {
        SetState(STATE_DISABLED);
        fileBrowser->SetTriggerUpdate(false);
        Taskbar::Instance()->SetTriggerUpdate(false);
        MainWindow::Instance()->Append(RightClick);

        int RightClick_choice = -1;
        while(RightClick_choice == -1 && RightClick != NULL)
        {
            usleep(100);

            if(shutdown)
                Sys_Shutdown();
            else if(reset)
                Sys_Reboot();

            RightClick_choice = RightClick->GetChoice();
        }

        delete RightClick;
        RightClick = NULL;

        if(ArcBrowser)
        {
            if(RightClick_choice >= 0)
            {
                ProcessArcChoice(ArcBrowser, RightClick_choice, DeviceBrowser->GetCurrentPath());
                CurBrowser->Refresh();
                fileBrowser->TriggerUpdate();
            }
        }

        else if(!ArcBrowser && RightClick_choice >= 0)
        {
            if(RightClick_choice >= 0)
            {
                ProcessChoice(DeviceBrowser, RightClick_choice);
                if(RightClick_choice >= PASTE)
                {
                    CurBrowser->Refresh();
                    fileBrowser->TriggerUpdate();
                }
            }
        }
        this->SetState(STATE_DEFAULT);
        fileBrowser->SetTriggerUpdate(true);
        Taskbar::Instance()->SetTriggerUpdate(true);
    }
}

void Explorer::OnButtonClick(GuiButton *sender, int pointer UNUSED, POINT p)
{
    if(sender == CreditsBtn)
    {
        Credits = new CreditWindow();
    }

    else if(sender == deviceSwitchBtn)
    {
        Device_Menu = new DeviceMenu(deviceSwitchBtn->GetLeft()-5-this->GetLeft(), deviceSwitchBtn->GetTop()+deviceSwitchBtn->GetHeight()-this->GetTop());
    }

    else if(sender == clickmenuBtn)
    {
        if(clickmenuBtn->IsInside(clickmenuBtn->GetLeft()+p.x, clickmenuBtn->GetTop()-10+p.y))
        {
            if(ArcBrowser)
            {
                RightClick = new PopUpMenu(clickmenuBtn->GetLeft()+p.x, clickmenuBtn->GetTop()+p.y);
                RightClick->AddItem(tr("Paste"));
                RightClick->AddItem(tr("Extract"));
                RightClick->AddItem(tr("Extract All"));
                RightClick->AddItem(tr("Properties"));
                RightClick->Finish();
            }
            else
            {
                RightClick = new PopUpMenu(clickmenuBtn->GetLeft()+p.x, clickmenuBtn->GetTop()+p.y);
                RightClick->AddItem(tr("Cut"));
                RightClick->AddItem(tr("Copy"));
                RightClick->AddItem(tr("Paste"));
                RightClick->AddItem(tr("Rename"));
                RightClick->AddItem(tr("Delete"));
                RightClick->AddItem(tr("New Folder"));
                RightClick->AddItem(tr("Add to zip"));
                RightClick->AddItem(tr("MD5 Check"));
                RightClick->AddItem(tr("Properties"));
                RightClick->Finish();
            }
        }
    }
    else if(sender == RefreshBtn)
    {
        CurBrowser->Refresh();
        fileBrowser->TriggerUpdate();
    }

    sender->ResetState();
}

void Explorer::SetState(int s)
{
	for (u8 i = 0; i < _elements.size(); i++)
	{
		try { _elements.at(i)->SetState(s); }
		catch (const std::exception& e) { }
	}
}

void Explorer::BackInDirectory(GuiButton *sender, int pointer UNUSED, POINT p UNUSED)
{
    CurBrowser->BackInDirectory();
    fileBrowser->SetSelected(0);
    CurBrowser->SetPageIndex(0);
    CurBrowser->ResetMarker();
    CurBrowser->Refresh();
    fileBrowser->TriggerUpdate();
    AdressText->SetText(CurBrowser->GetCurrentPath());
    Settings.LastUsedPath.assign(DeviceBrowser->GetCurrentPath());

    sender->ResetState();
}
