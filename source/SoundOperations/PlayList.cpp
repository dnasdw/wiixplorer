/***************************************************************************
 * Copyright (C) 2010
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
 * for WiiXplorer 2010
 ***************************************************************************/
#include <sys/dir.h>
#include <unistd.h>
#include "Controls/Application.h"
#include "Memory/Resources.h"
#include "FileOperations/DirList.h"
#include "PlayList.hpp"

#define PAGE_SIZE	8

PlayList::PlayList()
	: GuiFrame(0, 0)
{
	SetVisible(false);
	listOffset = 0;
	selectedItem = 0;
	listChanged = true;
	Minimized = true;
	width = 332;
	height = 352;

	btnSoundClick = NULL;
	btnSoundOver = NULL;
	menu_selectionData = NULL;
	playlistImgData = NULL;
	trigA = NULL;
	playlistImg = NULL;
	PlayListBtn = NULL;
	scrollbar = NULL;
}

PlayList::~PlayList()
{
	Hide();
	Save();
	ClearList();
}

bool PlayList::Save()
{
	char filepath[1024];
	snprintf(filepath, sizeof(filepath), "%s", Settings.ConfigPath);

	char * ptr = strrchr(filepath, '/');
	if(ptr)
		ptr[0] = '\0';

	strcat(filepath, "/WiiXplorer_PlayList.txt");

	FILE * file = fopen(filepath, "wb");
	if (!file)
		return false;

	fprintf(file, "# WiiXplorer Music Playlist\n");
	fprintf(file, "# Note: This file is automatically generated\n\n");

	for(u32 i = 0; i < FileList.size(); i++)
		fprintf(file, "%s\n", FileList.at(i));

	fclose(file);

	return true;
}

bool PlayList::LoadList()
{
	char filepath[1024];
	char line[1024];
	snprintf(filepath, sizeof(filepath), "%s", Settings.ConfigPath);

	char * ptr = strrchr(filepath, '/');
	if(ptr)
		ptr[0] = '\0';

	strcat(filepath, "/WiiXplorer_PlayList.txt");

	FILE * file = fopen(filepath, "rb");
	if (!file)
		return false;

	while (fgets(line, sizeof(line), file))
	{
		if (line[0] == '#') continue;

		std::string sline(line);

		if(sline.size() < 4) continue;

		size_t pos = 0;

		do
		{
			pos = sline.find_first_of('\n');

			if(pos != std::string::npos)
				sline.erase(pos, 1);
		}
		while(pos != std::string::npos);

		do
		{
			pos = sline.find_first_of('\r');

			if(pos != std::string::npos)
				sline.erase(pos, 1);
		}
		while(pos != std::string::npos);

		AddEntrie(sline.c_str());
	}
	fclose(file);

	return true;
}

void PlayList::OnButtonClick(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	if(sender == PlayListBtn)
	{
		if(Minimized && parentElement)
		{
			int PositionX = this->GetLeft()-parentElement->GetLeft();
			int PositionY = this->GetTop()-parentElement->GetTop();
			int PosDyn = 0;
			while(PosDyn < GetHeight()/2+93)
			{
				PosDyn += 15;
				SetPosition(PositionX, PositionY-PosDyn);
				Application::Instance()->updateEvents();
			}
			Minimized = false;
		}
		else if(parentElement)
		{
			Minimized = true;
			int PositionX = this->GetLeft()-parentElement->GetLeft();
			int PositionY = this->GetTop()-parentElement->GetTop();
			int PosDyn = 0;
			while(PosDyn < GetHeight()/2+93)
			{
				PosDyn += 15;
				SetPosition(PositionX, PositionY+PosDyn);
				Application::Instance()->updateEvents();
			}
		}
	}
}

void PlayList::OnButtonListClick(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	for(u32 i = 0; i < ListBtn.size(); i++)
	{
		if(sender == ListBtn[i])
		{
			ItemClicked(this, i);
			break;
		}
	}
}

void PlayList::Show()
{
	btnSoundClick = Resources::GetSound("button_click.wav");
	btnSoundOver = Resources::GetSound("button_over.wav");

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);

	menu_selectionData = Resources::GetImageData("bg_browser_selection.png");

	playlistImgData = Resources::GetImageData("playlist.png");
	playlistImg = new GuiImage(playlistImgData);
	playlistImg->SetBounds(cutBoundsRect);
	Append(playlistImg);

	scrollbar = new Scrollbar(240);
	scrollbar->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	scrollbar->SetPosition(-17, 50);
	scrollbar->SetScrollSpeed(Settings.ScrollSpeed);
	scrollbar->SetBounds(cutBoundsRect);
	scrollbar->listChanged.connect(this, &PlayList::OnListChange);
	Append(scrollbar);

	PlayListBtn = new GuiButton(50, 33);
	PlayListBtn->SetPosition(115, 10);
	PlayListBtn->SetSoundClick(btnSoundClick);
	PlayListBtn->SetSoundOver(btnSoundOver);
	PlayListBtn->SetTrigger(trigA);
	PlayListBtn->Clicked.connect(this, &PlayList::OnButtonClick);
	Append(PlayListBtn);

	width = playlistImgData->GetWidth();
	height = playlistImgData->GetHeight();

	for(int i = 0; i < PAGE_SIZE; i++)
	{
		GuiImage * TmpImg = new GuiImage(menu_selectionData);
		TmpImg->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
		TmpImg->SetScaleX(0.5);
		ListBtnImgOver.push_back(TmpImg);

		GuiText * TmpTxt = new GuiText((char*) NULL, 18, (GXColor){0, 0, 0, 255});
		TmpTxt->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
		TmpTxt->SetMaxWidth(width - 95, DOTTED);
		ListBtnTxt.push_back(TmpTxt);

		GuiButton * TmpButton = new GuiButton(width-100, 20);
		TmpButton->SetLabel(TmpTxt);
		TmpButton->SetImageOver(TmpImg);
		TmpButton->SetPosition(40, 62+30*i+3);
		TmpButton->SetTrigger(trigA);
		TmpButton->SetSoundClick(btnSoundClick);
		TmpButton->SetState(STATE_DISABLED);
		TmpButton->SetVisible(false);
		TmpButton->StateChanged.connect(this, &PlayList::OnListStateChange);
		TmpButton->Clicked.connect(this, &PlayList::OnButtonListClick);
		ListBtn.push_back(TmpButton);

		Append(TmpButton);
	}

	SetVisible(true);
	listChanged = true;
}

void PlayList::Hide()
{
	SetVisible(false);

	RemoveAll();

	Resources::Remove(btnSoundClick);
	Resources::Remove(btnSoundOver);
	Resources::Remove(menu_selectionData);
	Resources::Remove(playlistImgData);

	for(u32 i = 0; i < ListBtn.size(); i++)
	{
		delete ListBtn[i];
		delete ListBtnTxt[i];
		delete ListBtnImgOver[i];
	}
	ListBtn.clear();
	ListBtnTxt.clear();
	ListBtnImgOver.clear();

	if(trigA)
		delete trigA;
	if(playlistImg)
		delete playlistImg;
	if(PlayListBtn)
		delete PlayListBtn;
	if(scrollbar)
		delete scrollbar;

	btnSoundOver = NULL;
	btnSoundClick = NULL;
	menu_selectionData = NULL;
	playlistImgData = NULL;
	trigA = NULL;
	playlistImg = NULL;
	PlayListBtn = NULL;
	scrollbar = NULL;
}

void PlayList::AddEntrie(const char * filepath)
{
	if(!filepath)
		return;

	char * NewEntrie = new char[strlen(filepath)+1];

	if(!NewEntrie)
		return;

	sprintf(NewEntrie, "%s", filepath);

	FileList.push_back(NewEntrie);
	listChanged = true;
}

void PlayList::RemoveEntrie(int pos)
{
	if(pos < 0 || pos >= (int) FileList.size())
		return;

	if(FileList.at(pos) != NULL)
		delete [] FileList.at(pos);

	FileList.erase(FileList.begin()+pos);
	listChanged = true;
}

void PlayList::ClearList()
{
	for(u32 i = 0; i < FileList.size(); i++)
	{
		if(FileList.at(i) != NULL)
		{
			delete [] FileList.at(i);
			FileList.at(i) = NULL;
		}
	}

	FileList.clear();
	std::vector<char *>().swap(FileList);
	listOffset = 0;
	selectedItem = 0;

	for(u32 i = 0; i < ListBtn.size(); i++)
	{
		ListBtn[i]->SetVisible(false);
		ListBtn[i]->SetState(STATE_DISABLED);
	}

	listChanged = true;
}

int PlayList::FindFile(const char * filepath)
{
	if(!filepath)
		return -1;

	for(u32 i = 0; i < FileList.size(); i++)
	{
		if(strcasecmp(filepath, FileList[i]) == 0)
			return i;
	}

	return -1;
}

const char * PlayList::at(int pos)
{
	if(pos < 0 || pos >= (int) FileList.size())
		return NULL;

	return FileList[pos];
}

bool PlayList::ParsePath(const char * filepath)
{
	char currentPath[1024];
	sprintf(currentPath, "%s", filepath);

	char * pathptr = strrchr(currentPath, '/');
	if(pathptr)
	{
		pathptr++;
		pathptr[0] = 0;
	}
	else
		return false;

	DirList dir(currentPath, Settings.FileExtensions.GetAudio(), DirList::Files | DirList::CheckSubfolders);

	for(int i = 0; i < dir.GetFilecount(); ++i)
		AddEntrie(dir.GetFilepath(i));

	return true;
}

void PlayList::OnListStateChange(GuiElement *sender, int s, int c UNUSED)
{
	if(s != STATE_SELECTED)
		return;

	for(int i = 0; i < PAGESIZE; i++)
	{
		if(sender == ListBtn[i])
		{
			if(s == STATE_SELECTED)
			{
				ListBtnTxt[selectedItem]->SetMaxWidth(width - 95, DOTTED);
				ListBtnTxt[i]->SetMaxWidth(width - 115, SCROLL_HORIZONTAL);
			}
			break;
		}
	}
}

void PlayList::OnListChange(int selItem, int selIndex)
{
	selectedItem = selItem;
	listOffset = selIndex;

	if(listChanged && FileList.size() > PAGE_SIZE)
	{
		while(listOffset+PAGE_SIZE > (int) FileList.size())
			--listOffset;
	}

	for(int i = 0; i < (int) PAGE_SIZE; i++)
	{
		if(i >= (int) ListBtn.size())
			continue;

		if(listOffset+i < (int) FileList.size())
		{
			if(ListBtn[i]->GetState() == STATE_DISABLED)
				ListBtn[i]->SetState(STATE_DEFAULT);

			ListBtn[i]->SetVisible(true);

			char * filename = strrchr(FileList[listOffset+i], '/');
			if(filename)
				filename++;

			ListBtnTxt[i]->SetText(filename);
		}
		else
		{
			ListBtn[i]->SetVisible(false);
			ListBtn[i]->SetState(STATE_DISABLED);
		}
	}
}

void PlayList::Draw()
{
	if(!this->IsVisible())
		return;

	playlistImg->Draw();
	scrollbar->Draw();

	if(Minimized)
		return;

	for(u32 i = 0; i < ListBtn.size(); i++)
	{
		ListBtn[i]->Draw();
	}
}

void PlayList::Update(GuiTrigger * t)
{
	if(!this->IsVisible())
		return;

	PlayListBtn->Update(t);
	scrollbar->Update(t);

	if(listChanged && FileList.size() > PAGE_SIZE)
	{
		while(listOffset+PAGE_SIZE > (int) FileList.size())
			--listOffset;
	}

	for(int i = 0; i < (int) PAGE_SIZE; i++)
	{
		if(i >= (int) ListBtn.size())
			continue;

		if(i != selectedItem && ListBtn[i]->GetState() == STATE_SELECTED)
		{
			ListBtn[i]->ResetState();
			ListBtnTxt[i]->SetMaxWidth(width - 95, DOTTED);
		}
		else if(i == selectedItem && ListBtn[i]->GetState() == STATE_DEFAULT)
			ListBtn[selectedItem]->SetState(STATE_SELECTED, t->chan);

		int currChan = t->chan;

		if(t->wpad.ir.valid && !ListBtn[i]->IsInside(t->wpad.ir.x, t->wpad.ir.y))
			t->chan = -1;

		ListBtn[i]->Update(t);
		t->chan = currChan;

		if(ListBtn[i]->GetState() == STATE_SELECTED)
			selectedItem = i;
	}

	scrollbar->SetPageSize(PAGE_SIZE);
	scrollbar->SetRowSize(0);
	scrollbar->SetSelectedItem(selectedItem);
	scrollbar->SetSelectedIndex(listOffset);
	scrollbar->SetEntrieCount(FileList.size());
}

