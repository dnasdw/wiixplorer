/****************************************************************************
 * libwiigui
 *
 * Tantric 2009
 *
 * gui_filebrowser.cpp
 *
 * GUI class definitions
 ***************************************************************************/

#include "gui_filebrowser.h"

/**
 * Constructor for the GuiFileBrowser class.
 */
GuiFileBrowser::GuiFileBrowser(FileBrowser * filebrowser, int w, int h)
{
	width = w;
	height = h;
	selectedItem = 0;
	numEntries = 0;
	browser = filebrowser;
	selectable = true;
	listChanged = true; // trigger an initial list update
	triggerdisabled = false; // trigger disable
	focus = 1; // allow focus

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	trigHeldA = new GuiTrigger;
	trigHeldA->SetHeldTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	btnSoundOver = new GuiSound(button_over_pcm, button_over_pcm_size, SOUND_PCM);
	btnSoundClick = new GuiSound(button_click_pcm, button_click_pcm_size, SOUND_PCM);

	bgFileSelectionEntry = new GuiImageData(bg_browser_selection_png);
	fileArchives = new GuiImageData(icon_archives_png);
	fileDefault = new GuiImageData(icon_default_png);
	fileFolder = new GuiImageData(icon_folder_png);
	fileGFX = new GuiImageData(icon_gfx_png);
	filePLS = new GuiImageData(icon_pls_png);
	fileSFX = new GuiImageData(icon_sfx_png);
	fileTXT = new GuiImageData(icon_txt_png);
	fileXML = new GuiImageData(icon_xml_png);

	scrollbar = new GuiImageData(scrollbar_png);
	scrollbarImg = new GuiImage(scrollbar);
	scrollbarImg->SetParent(this);
	scrollbarImg->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	scrollbarImg->SetPosition(-10, 20);

	arrowDown = new GuiImageData(scrollbar_arrowdown_png);
	arrowDownImg = new GuiImage(arrowDown);
	arrowDownOver = new GuiImageData(scrollbar_arrowdown_over_png);
	arrowDownOverImg = new GuiImage(arrowDownOver);
	arrowUp = new GuiImageData(scrollbar_arrowup_png);
	arrowUpImg = new GuiImage(arrowUp);
	arrowUpOver = new GuiImageData(scrollbar_arrowup_over_png);
	arrowUpOverImg = new GuiImage(arrowUpOver);
	scrollbarBox = new GuiImageData(scrollbar_box_png);
	scrollbarBoxImg = new GuiImage(scrollbarBox);
	scrollbarBoxOver = new GuiImageData(scrollbar_box_over_png);
	scrollbarBoxOverImg = new GuiImage(scrollbarBoxOver);

	arrowUpBtn = new GuiButton(arrowUpImg->GetWidth(), arrowUpImg->GetHeight());
	arrowUpBtn->SetParent(this);
	arrowUpBtn->SetImage(arrowUpImg);
	arrowUpBtn->SetImageOver(arrowUpOverImg);
	arrowUpBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	arrowUpBtn->SetPosition(-10, 0);
	arrowUpBtn->SetSelectable(false);
	arrowUpBtn->SetClickable(false);
	arrowUpBtn->SetHoldable(true);
	arrowUpBtn->SetTrigger(trigHeldA);
	arrowUpBtn->SetSoundOver(btnSoundOver);
	arrowUpBtn->SetSoundClick(btnSoundClick);

	arrowDownBtn = new GuiButton(arrowDownImg->GetWidth(), arrowDownImg->GetHeight());
	arrowDownBtn->SetParent(this);
	arrowDownBtn->SetImage(arrowDownImg);
	arrowDownBtn->SetImageOver(arrowDownOverImg);
	arrowDownBtn->SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	arrowDownBtn->SetPosition(-10, 0);
	arrowDownBtn->SetSelectable(false);
	arrowDownBtn->SetClickable(false);
	arrowDownBtn->SetHoldable(true);
	arrowDownBtn->SetTrigger(trigHeldA);
	arrowDownBtn->SetSoundOver(btnSoundOver);
	arrowDownBtn->SetSoundClick(btnSoundClick);

	scrollbarBoxBtn = new GuiButton(scrollbarBoxImg->GetWidth(), scrollbarBoxImg->GetHeight());
	scrollbarBoxBtn->SetParent(this);
	scrollbarBoxBtn->SetImage(scrollbarBoxImg);
	scrollbarBoxBtn->SetImageOver(scrollbarBoxOverImg);
	scrollbarBoxBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	scrollbarBoxBtn->SetPosition(-10, 0);
	scrollbarBoxBtn->SetMinY(0);
	scrollbarBoxBtn->SetMaxY(136);
	scrollbarBoxBtn->SetSelectable(false);
	scrollbarBoxBtn->SetClickable(false);
	scrollbarBoxBtn->SetHoldable(true);
	scrollbarBoxBtn->SetTrigger(trigHeldA);

	for(int i=0; i<PAGESIZE; i++)
	{
		fileListText[i] = new GuiText(NULL,20, (GXColor){0, 0, 0, 255});
		fileListText[i]->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
		fileListText[i]->SetPosition(10,0);
		fileListText[i]->SetMaxWidth(this->GetWidth() - (arrowDownImg->GetWidth()+50), DOTTED);

		fileListTextOver[i] = new GuiText(NULL,20, (GXColor){0, 0, 0, 255});
		fileListTextOver[i]->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
		fileListTextOver[i]->SetPosition(10,0);
		fileListTextOver[i]->SetMaxWidth(this->GetWidth() - (arrowDownImg->GetWidth()+60), SCROLL_HORIZONTAL);

		fileListBg[i] = new GuiImage(bgFileSelectionEntry);
		fileListArchives[i] = new GuiImage(fileArchives);
		fileListDefault[i] = new GuiImage(fileDefault);
		fileListFolder[i] = new GuiImage(fileFolder);
		fileListGFX[i] = new GuiImage(fileGFX);
		fileListPLS[i] = new GuiImage(filePLS);
		fileListSFX[i] = new GuiImage(fileSFX);
		fileListTXT[i] = new GuiImage(fileTXT);
		fileListXML[i] = new GuiImage(fileXML);

		fileList[i] = new GuiButton(507,30);
		fileList[i]->SetParent(this);
		fileList[i]->SetLabel(fileListText[i]);
		fileList[i]->SetLabelOver(fileListTextOver[i]);
		fileList[i]->SetImageOver(fileListBg[i]);
		fileList[i]->SetPosition(7,30*i+3);
		fileList[i]->SetTrigger(trigA);
		fileList[i]->SetRumble(false);
		fileList[i]->SetSoundClick(btnSoundClick);
		fileList[i]->Clicked.connect(this, &GuiFileBrowser::OnClicked);
	}
}

/**
 * Destructor for the GuiFileBrowser class.
 */
GuiFileBrowser::~GuiFileBrowser()
{
	delete arrowUpBtn;
	delete arrowDownBtn;
	delete scrollbarBoxBtn;

	delete scrollbarImg;
	delete arrowDownImg;
	delete arrowDownOverImg;
	delete arrowUpImg;
	delete arrowUpOverImg;
	delete scrollbarBoxImg;
	delete scrollbarBoxOverImg;

	delete bgFileSelectionEntry;
	delete fileArchives;
	delete fileDefault;
	delete fileFolder;
	delete fileGFX;
	delete filePLS;
	delete fileSFX;
	delete fileTXT;
	delete fileXML;
	delete scrollbar;
	delete arrowDown;
	delete arrowDownOver;
	delete arrowUp;
	delete arrowUpOver;
	delete scrollbarBox;
	delete scrollbarBoxOver;

	delete btnSoundOver;
	delete btnSoundClick;
	delete trigHeldA;
	delete trigA;

	for(int i=0; i<PAGESIZE; i++)
	{
		delete fileListText[i];
		delete fileListTextOver[i];
		delete fileList[i];
		delete fileListBg[i];
		delete fileListArchives[i];
		delete fileListDefault[i];
		delete fileListFolder[i];
		delete fileListGFX[i];
		delete fileListPLS[i];
		delete fileListSFX[i];
		delete fileListTXT[i];
		delete fileListXML[i];
	}
}

void GuiFileBrowser::OnClicked(GuiElement *sender, int pointer, POINT p)
{
state = STATE_CLICKED;
}

void GuiFileBrowser::SetFocus(int f)
{
	focus = f;

	for(int i=0; i<PAGESIZE; i++)
		fileList[i]->ResetState();

	if(f == 1)
		fileList[selectedItem]->SetState(STATE_SELECTED);
}

void GuiFileBrowser::DisableTriggerUpdate(bool set)
{
	triggerdisabled = set;
}

void GuiFileBrowser::ResetState()
{
	state = STATE_DEFAULT;
	stateChan = -1;
	selectedItem = 0;

	for(int i=0; i<PAGESIZE; i++)
	{
		fileList[i]->ResetState();
	}
}

void GuiFileBrowser::TriggerUpdate()
{
	listChanged = true;
}

/**
 * Draw the button on screen
 */
void GuiFileBrowser::Draw()
{
	if(!this->IsVisible())
		return;

	for(int i=0; i<PAGESIZE; i++)
	{
		fileList[i]->Draw();
	}

	scrollbarImg->Draw();
	arrowUpBtn->Draw();
	arrowDownBtn->Draw();
	scrollbarBoxBtn->Draw();

	this->UpdateEffects();
}

void GuiFileBrowser::Update(GuiTrigger * t)
{
	if(state == STATE_DISABLED || !t || triggerdisabled)
		return;

	int position = 0;
	int positionWiimote = 0;

	arrowUpBtn->Update(t);
	arrowDownBtn->Update(t);
	scrollbarBoxBtn->Update(t);

	// move the file listing to respond to wiimote cursor movement
	if(scrollbarBoxBtn->GetState() == STATE_HELD &&
		scrollbarBoxBtn->GetStateChan() == t->chan &&
		t->wpad->ir.valid &&
		browser->GetEntrieCount() > PAGESIZE
		)
	{
		scrollbarBoxBtn->SetPosition(-10,0);
		positionWiimote = t->wpad->ir.y - 60 - scrollbarBoxBtn->GetTop();

		if(positionWiimote < scrollbarBoxBtn->GetMinY())
			positionWiimote = scrollbarBoxBtn->GetMinY();
		else if(positionWiimote > scrollbarBoxBtn->GetMaxY())
			positionWiimote = scrollbarBoxBtn->GetMaxY();

		int pageIndex = (positionWiimote * browser->GetEntrieCount())/136.0 - selectedItem;

		if(pageIndex <= 0)
		{
			pageIndex = 0;
		}
		else if(pageIndex+PAGESIZE >= browser->GetEntrieCount())
		{
			pageIndex = browser->GetEntrieCount()-PAGESIZE;
		}
		browser->SetPageIndex(pageIndex);
		listChanged = true;
		focus = false;
	}

	if(arrowDownBtn->GetState() == STATE_HELD && arrowDownBtn->GetStateChan() == t->chan)
	{
		t->wpad->btns_h |= WPAD_BUTTON_DOWN;
		if(!this->IsFocused())
			((GuiWindow *)this->GetParent())->ChangeFocus(this);
	}
	else if(arrowUpBtn->GetState() == STATE_HELD && arrowUpBtn->GetStateChan() == t->chan)
	{
		t->wpad->btns_h |= WPAD_BUTTON_UP;
		if(!this->IsFocused())
			((GuiWindow *)this->GetParent())->ChangeFocus(this);
	}

	// pad/joystick navigation
	//if(!focus)
	//{
		//goto endNavigation; // skip navigation
	//	listChanged = false;
	//}

	if(t->Right())
	{
		if(browser->GetPageIndex() < browser->GetEntrieCount() && browser->GetEntrieCount() > PAGESIZE)
		{
			int pageIndex = browser->GetPageIndex() + PAGESIZE;
			if(pageIndex+PAGESIZE >= browser->GetEntrieCount())
				pageIndex = browser->GetEntrieCount()-PAGESIZE;
			browser->SetPageIndex(pageIndex);
			listChanged = true;
		}
	}
	else if(t->Left())
	{
		if(browser->GetPageIndex() > 0)
		{
			int pageIndex = browser->GetPageIndex() - PAGESIZE;
			if(pageIndex < 0)
				pageIndex = 0;
			browser->SetPageIndex(pageIndex);
			listChanged = true;
		}
	}
	else if(t->Down())
	{
		if(browser->GetPageIndex() + selectedItem + 1 < browser->GetEntrieCount())
		{
			if(selectedItem == PAGESIZE-1)
			{
				// move list down by 1
				browser->SetPageIndex(browser->GetPageIndex()+1);
				listChanged = true;
			}
			else if(fileList[selectedItem+1]->IsVisible())
			{
				fileList[selectedItem]->ResetState();
				fileList[++selectedItem]->SetState(STATE_SELECTED, t->chan);
			}
		}
	}
	else if(t->Up())
	{
		if(selectedItem == 0 &&	 browser->GetPageIndex() + selectedItem > 0)
		{
			// move list up by 1
			browser->SetPageIndex(browser->GetPageIndex()-1);
			listChanged = true;
		}
		else if(selectedItem > 0)
		{
			fileList[selectedItem]->ResetState();
			fileList[--selectedItem]->SetState(STATE_SELECTED, t->chan);
		}
	}

	//endNavigation:
	if(numEntries != browser->GetEntrieCount()) {
	    numEntries = browser->GetEntrieCount();
        listChanged = true;
	}

	for(int i=0; i<PAGESIZE; i++)
	{
		if(listChanged)
		{
			if(browser->GetPageIndex()+i < browser->GetEntrieCount())
			{
				if(fileList[i]->GetState() == STATE_DISABLED)
					fileList[i]->SetState(STATE_DEFAULT);

				fileList[i]->SetVisible(true);

				fileListText[i]->SetText(browser->GetItemDisplayname(browser->GetPageIndex()+i));
				fileListTextOver[i]->SetText(browser->GetItemDisplayname(browser->GetPageIndex()+i));

				if(browser->IsDir(browser->GetPageIndex()+i)) // directory
				{
					fileList[i]->SetIcon(fileListFolder[i]);
					fileListText[i]->SetPosition(30,0);
					fileListTextOver[i]->SetPosition(30,0);
				}
				else
				{
				    char *fileext = strrchr(browser->GetItemDisplayname(browser->GetPageIndex()+i), '.');
					fileListText[i]->SetPosition(32,0);
					fileListTextOver[i]->SetPosition(32,0);
				    if(fileext)
				    {
                        if(strcasecmp(fileext, ".png") == 0 || strcasecmp(fileext, ".jpg") == 0 || strcasecmp(fileext, ".gif") == 0 ||
						   strcasecmp(fileext, ".tga") == 0 || strcasecmp(fileext, ".tpl") == 0 || strcasecmp(fileext, ".bmp") == 0) {
                            fileList[i]->SetIcon(fileListGFX[i]);
                        } else if(strcasecmp(fileext, ".mp3") == 0 || strcasecmp(fileext, ".ogg") == 0 || strcasecmp(fileext, ".wav") == 0) {
                            fileList[i]->SetIcon(fileListSFX[i]);
                        } else if(strcasecmp(fileext, ".pls") == 0 || strcasecmp(fileext, ".m3u") == 0) {
                            fileList[i]->SetIcon(fileListPLS[i]);
                        } else if(strcasecmp(fileext, ".txt") == 0) {
                            fileList[i]->SetIcon(fileListTXT[i]);
                        } else if(strcasecmp(fileext, ".xml") == 0) {
                            fileList[i]->SetIcon(fileListXML[i]);
                        } else if(strcasecmp(fileext, ".rar") == 0 || strcasecmp(fileext, ".zip") == 0) {
                            fileList[i]->SetIcon(fileListArchives[i]);
                        } else {
                            fileList[i]->SetIcon(fileListDefault[i]);
                        }
				    } else {
                        fileList[i]->SetIcon(fileListDefault[i]);
				    }
				}
			}
			else
			{
				fileList[i]->SetVisible(false);
				fileList[i]->SetState(STATE_DISABLED);
			}
		}

		if(i != selectedItem && fileList[i]->GetState() == STATE_SELECTED)
			fileList[i]->ResetState();
		else if(focus && i == selectedItem && fileList[i]->GetState() == STATE_DEFAULT)
			fileList[selectedItem]->SetState(STATE_SELECTED, t->chan);

		int currChan = t->chan;

		if(t->wpad->ir.valid && !fileList[i]->IsInside(t->wpad->ir.x, t->wpad->ir.y))
			t->chan = -1;

		fileList[i]->Update(t);
		t->chan = currChan;

		if(fileList[i]->GetState() == STATE_SELECTED)
		{
			selectedItem = i;
			browser->SetSelectedIndex(browser->GetPageIndex() + i);
		}
	}

	// update the location of the scroll box based on the position in the file list
	if(positionWiimote > 0)
	{
		position = positionWiimote; // follow wiimote cursor
	}
	else
	{
		position = 136*(browser->GetPageIndex() + PAGESIZE/2.0) / (browser->GetEntrieCount()*1.0);

		if(browser->GetPageIndex()/(PAGESIZE/2.0) < 1)
			position = 0;
		else if((browser->GetPageIndex()+PAGESIZE)/(PAGESIZE*1.0) >= (browser->GetEntrieCount())/(PAGESIZE*1.0))
			position = 136;
	}

	scrollbarBoxBtn->SetPosition(-10,position+36);

	listChanged = false;

	if(updateCB)
		updateCB(this);
}
