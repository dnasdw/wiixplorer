 /****************************************************************************
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
 * Settings.cpp
 *
 * Settings Class
 * for WiiXplorer 2009
 ***************************************************************************/
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Settings.h"
#include "FileOperations/fileops.h"
#include "Language/gettext.h"
#include "Tools/tools.h"

#define DEFAULT_APP_PATH    "apps/WiiExplorer/"
#define CONFIGPATH          "config/WiiXplorer/"
#define CONFIGNAME          "WiiXplorer.cfg"
#define LANGPATH      	    "config/WiiXplorer/Languages/"

Settings::Settings()
{
    strcpy(BootDevice, "sd:/");
    snprintf(ConfigPath, sizeof(ConfigPath), "%s%s%s", BootDevice, CONFIGPATH, CONFIGNAME);
    this->SetDefault();
}

Settings::~Settings()
{
}

void Settings::SetDefault()
{
    MountMethod = 0;
    MountNTFS = 0;
    MusicVolume = 80;
    CurrentSMBUser = 0;
	CurrentFTPUser = 0;
    BGMLoopMode = 1;
	SlideshowDelay = 4;
	ImageFadeSpeed = 20;
	KeyboardDeleteDelay = 15;
    AutoConnect = 0;
    UpdateMetaxml = 1;
    UpdateIconpng = 1;
    ClockMode = 0;
    ScrollSpeed = 5;
    BrowserMode = 0;
    ScreenshotFormat = 0;
    SoundblockCount = 8;
    SoundblockSize = 8192;
    LoadMusicToMem = 0;
    DeleteTempPath = 1;
    CopyThreadPrio = 100;
    CopyThreadBackPrio = 30;
    Rumble = 1;
    HideSystemFiles = 1;
    PDFLoadZoom = 1.0f;
    sprintf(CustomFontPath, "%s%sfont.ttf", BootDevice, CONFIGPATH);
    sprintf(LanguagePath, "%s%s", BootDevice, LANGPATH);
    sprintf(UpdatePath, "%s%s", BootDevice, DEFAULT_APP_PATH);
    sprintf(AppPath, "%sapps/", BootDevice);
    sprintf(TempPath, "%s/Temp/", UpdatePath);
    sprintf(ScreenshotPath, "%s", UpdatePath);
    strcpy(MusicPath, "");
    sprintf(MPlayerPath, "%sapps/mplayer_ce/boot.dol", BootDevice);

    for(int i = 0; i < MAXSMBUSERS; i++) {
        strcpy(SMBUser[i].Host, "");
        strcpy(SMBUser[i].User, "");
        strcpy(SMBUser[i].Password, "");
        strcpy(SMBUser[i].SMBName, "");
	}

	for(int i = 0; i < MAXFTPUSERS; i++) {
        strcpy(FTPUser[i].Host, "");
        strcpy(FTPUser[i].User, "");
        strcpy(FTPUser[i].Password, "");
        strcpy(FTPUser[i].FTPPath, "/");
        FTPUser[i].Port = 21;
		FTPUser[i].Passive = 0;
	}

    FTPServer.AutoStart = 0;
	strcpy(FTPServer.Password, "");
	FTPServer.Port = 21;

    FileExtensions.SetDefault();
    Controls.SetDefault();

    DefaultColors();
}

void Settings::DefaultColors()
{
    BackgroundUL = GXCOLORTORGBA(((GXColor){67, 71, 72, 255}));
    BackgroundUR = GXCOLORTORGBA(((GXColor){67, 71, 72, 255}));
    BackgroundBR = GXCOLORTORGBA(((GXColor){100, 110, 110, 255}));
    BackgroundBL = GXCOLORTORGBA(((GXColor){100, 110, 110, 255}));
    ProgressUL = GXCOLORTORGBA(((GXColor){239, 28, 28, 255}));
    ProgressUR = GXCOLORTORGBA(((GXColor){79, 153, 239, 255}));
    ProgressBR = GXCOLORTORGBA(((GXColor){59, 159, 223, 255}));
    ProgressBL = GXCOLORTORGBA(((GXColor){199, 71, 99, 255}));
    ProgressEmptyUL = GXCOLORTORGBA(((GXColor){237, 240, 245, 255}));
    ProgressEmptyUR = GXCOLORTORGBA(((GXColor){237, 240, 245, 255}));
    ProgressEmptyBR = GXCOLORTORGBA(((GXColor){197, 202, 210, 255}));
    ProgressEmptyBL = GXCOLORTORGBA(((GXColor){197, 202, 210, 255}));
}

bool Settings::Save()
{
    char filepath[100];
    char filedest[100];
	char password[100];

    snprintf(filepath, sizeof(filepath), "%s", ConfigPath);
    snprintf(filedest, sizeof(filedest), "%s", ConfigPath);
    char * tmppath = strrchr(filedest, '/');
    if(tmppath)
    {
        tmppath++;
        tmppath[0] = '\0';
    }

    if(!CreateSubfolder(filedest))
    {
        //!Try the other device and standard path
        if(strcmp(BootDevice, "usb:/") == 0)
            strcpy(BootDevice, "sd:/");
        else
            strcpy(BootDevice, "usb:/");

        snprintf(filepath, sizeof(filepath), "%s%s%s", BootDevice, CONFIGPATH, CONFIGNAME);
        snprintf(filedest, sizeof(filedest), "%s%s", BootDevice, CONFIGPATH);

        if(!CreateSubfolder(filedest))
            return false;
    }

    file = fopen(filepath, "w");
    if(!file)
    {
        fclose(file);
        return false;
    }

    fprintf(file, "# WiiXplorer Settingsfile\n");
	fprintf(file, "# Note: This file is automatically generated\n\n");
	fprintf(file, "# Main Settings\n\n");
	fprintf(file, "MountMethod = %d\n", MountMethod);
	fprintf(file, "CurrentSMBUser = %d\n", CurrentSMBUser);
	fprintf(file, "CurrentFTPUser = %d\n", CurrentFTPUser);
	fprintf(file, "LanguagePath = %s\n", LanguagePath);
	fprintf(file, "MusicVolume = %d\n", MusicVolume);
	fprintf(file, "MountNTFS = %d\n", MountNTFS);
	fprintf(file, "BGMLoopMode = %d\n", BGMLoopMode);
	fprintf(file, "AutoConnect = %d\n", AutoConnect);
	fprintf(file, "UpdateMetaxml = %d\n", UpdateMetaxml);
	fprintf(file, "UpdateIconpng = %d\n", UpdateIconpng);
	fprintf(file, "ClockMode = %d\n", ClockMode);
	fprintf(file, "ScrollSpeed = %d\n", ScrollSpeed);
	fprintf(file, "BrowserMode = %d\n", BrowserMode);
	fprintf(file, "SoundblockCount = %d\n", SoundblockCount);
	fprintf(file, "SoundblockSize = %d\n", SoundblockSize);
	fprintf(file, "LoadMusicToMem = %d\n", LoadMusicToMem);
	fprintf(file, "PDFLoadZoom = %0.2f\n", PDFLoadZoom);
	fprintf(file, "ScreenshotFormat = %d\n", ScreenshotFormat);
	fprintf(file, "MusicPath = %s\n", MusicPath);
	fprintf(file, "MPlayerPath = %s\n", MPlayerPath);
	fprintf(file, "CustomFontPath = %s\n", CustomFontPath);
	fprintf(file, "UpdatePath = %s\n", UpdatePath);
	fprintf(file, "AppPath = %s\n", AppPath);
	fprintf(file, "TempPath = %s\n", TempPath);
	fprintf(file, "ScreenshotPath = %s\n", ScreenshotPath);
	fprintf(file, "SlideshowDelay = %d\n", SlideshowDelay);
	fprintf(file, "ImageFadeSpeed = %d\n", ImageFadeSpeed);
	fprintf(file, "KeyboardDeleteDelay = %d\n", KeyboardDeleteDelay);
	fprintf(file, "DeleteTempPath = %d\n", DeleteTempPath);
	fprintf(file, "CopyThreadPrio = %d\n", CopyThreadPrio);
	fprintf(file, "CopyThreadBackPrio = %d\n", CopyThreadBackPrio);
	fprintf(file, "Rumble = %d\n", Rumble);
	fprintf(file, "HideSystemFiles = %d\n", HideSystemFiles);

	fprintf(file, "\n# Fileextensions assignment.\n\n");
	fprintf(file, "FileExtensions.VideoFiles = %s\n", FileExtensions.GetVideo());
	fprintf(file, "FileExtensions.AudioFiles = %s\n", FileExtensions.GetAudio());
	fprintf(file, "FileExtensions.ImageFiles = %s\n", FileExtensions.GetImage());
	fprintf(file, "FileExtensions.ArchiveFiles = %s\n", FileExtensions.GetArchive());
	fprintf(file, "FileExtensions.HomebrewFiles = %s\n", FileExtensions.GetHomebrew());
	fprintf(file, "FileExtensions.FontFiles = %s\n", FileExtensions.GetFont());
	fprintf(file, "FileExtensions.LanguageFiles = %s\n", FileExtensions.GetLanguageFiles());
	fprintf(file, "FileExtensions.WiiBinaryFiles = %s\n", FileExtensions.GetWiiBinary());
	fprintf(file, "FileExtensions.PDFFiles = %s\n", FileExtensions.GetPDF());
	fprintf(file, "FileExtensions.WiiXplorerMovies = %s\n", FileExtensions.GetWiiXplorerMovies());

	fprintf(file, "\n# Color Settings.\n\n");
	fprintf(file, "BackgroundUL = %u\n", BackgroundUL);
	fprintf(file, "BackgroundUR = %u\n", BackgroundUR);
	fprintf(file, "BackgroundBR = %u\n", BackgroundBR);
	fprintf(file, "BackgroundBL = %u\n", BackgroundBL);
	fprintf(file, "ProgressUL = %u\n", ProgressUL);
	fprintf(file, "ProgressUR = %u\n", ProgressUR);
	fprintf(file, "ProgressBR = %u\n", ProgressBR);
	fprintf(file, "ProgressBL = %u\n", ProgressBL);
	fprintf(file, "ProgressEmptyUL = %u\n", ProgressEmptyUL);
	fprintf(file, "ProgressEmptyUR = %u\n", ProgressEmptyUR);
	fprintf(file, "ProgressEmptyBR = %u\n", ProgressEmptyBR);
	fprintf(file, "ProgressEmptyBL = %u\n", ProgressEmptyBL);

	fprintf(file, "\n# SMB Setup Information\n\n");
    for(int i = 0; i < MAXSMBUSERS; i++) {
        fprintf(file, "SMBUser[%d].Host = %s\n", i+1, SMBUser[i].Host);
        fprintf(file, "SMBUser[%d].User = %s\n", i+1, SMBUser[i].User);
        password[0] = 0;
		if (strcmp(SMBUser[i].Password, "") != 0)
            EncryptString(SMBUser[i].Password, password);
		fprintf(file, "SMBUser[%d].CPassword = %s\n", i+1, password);
        fprintf(file, "SMBUser[%d].SMBName = %s\n\n", i+1, SMBUser[i].SMBName);
	}

	fprintf(file, "\n# FTP Setup Information\n\n");
    for(int i = 0; i < MAXFTPUSERS; i++) {
        fprintf(file, "FTPUser[%d].Host = %s\n", i+1, FTPUser[i].Host);
        fprintf(file, "FTPUser[%d].User = %s\n", i+1, FTPUser[i].User);
        password[0] = 0;
        if (strcmp(FTPUser[i].Password, "") != 0)
            EncryptString(FTPUser[i].Password, password);
        fprintf(file, "FTPUser[%d].CPassword = %s\n", i+1, password);
        fprintf(file, "FTPUser[%d].FTPPath = %s\n", i+1, FTPUser[i].FTPPath);
		fprintf(file, "FTPUser[%d].Port = %d\n", i+1, FTPUser[i].Port);
		fprintf(file, "FTPUser[%d].Passive = %d\n\n", i+1, FTPUser[i].Passive);
	}

	fprintf(file, "\n# FTP Server Setup Information\n\n");
	fprintf(file, "FTPServer.AutoStart = %d\n", FTPServer.AutoStart);
    password[0] = 0;
    if (strcmp(FTPServer.Password, "") != 0)
        EncryptString(FTPServer.Password, password);
	fprintf(file, "FTPServer.CPassword = %s\n", password);
	fprintf(file, "FTPServer.Port = %d\n", FTPServer.Port);
	fclose(file);

	Controls.Save();

	return true;
}

bool Settings::Load(int argc, char *argv[])
{
    char testpath[100];
    bool found = false;

    //! Try first standard SD Path
    snprintf(ConfigPath, sizeof(ConfigPath), "sd:/%s%s", CONFIGPATH, CONFIGNAME);
    found = CheckFile(ConfigPath);
    if(!found)
    {
        //! Try standard USB Path
        snprintf(testpath, sizeof(testpath), "usb:/%s%s", CONFIGPATH, CONFIGNAME);
        found = CheckFile(testpath);
        if(found)
        {
            snprintf(ConfigPath, sizeof(ConfigPath), "%s", testpath);
            strcpy(BootDevice, "usb:/");
            found = true;
        }
    }
    if(!found)
    {
        //! Try alternative SD Path
        snprintf(testpath, sizeof(testpath), "sd:/apps/WiiXplorer/%s", CONFIGNAME);
        found = CheckFile(testpath);
        if(found)
        {
            snprintf(ConfigPath, sizeof(ConfigPath), "%s", testpath);
            strcpy(BootDevice, "sd:/");
            found = true;
        }
    }
    if(!found)
    {
        //! Try alternative SD Path
        snprintf(testpath, sizeof(testpath), "sd:/apps/WiiExplorer/%s", CONFIGNAME);
        found = CheckFile(testpath);
        if(found)
        {
            snprintf(ConfigPath, sizeof(ConfigPath), "%s", testpath);
            strcpy(BootDevice, "sd:/");
            found = true;
        }
    }
    if(!found)
    {
        //! Try alternative USB Path
        snprintf(testpath, sizeof(testpath), "usb:/apps/WiiXplorer/%s", CONFIGNAME);
        found = CheckFile(testpath);
        if(found)
        {
            snprintf(ConfigPath, sizeof(ConfigPath), "%s", testpath);
            strcpy(BootDevice, "usb:/");
            found = true;
        }
    }
    if(!found)
    {
        //! Try alternative USB Path
        snprintf(testpath, sizeof(testpath), "usb:/apps/WiiExplorer/%s", CONFIGNAME);
        found = CheckFile(testpath);
        if(found)
        {
            snprintf(ConfigPath, sizeof(ConfigPath), "%s", testpath);
            strcpy(BootDevice, "usb:/");
            found = true;
        }
    }
    if(!found)
    {
        //! If all failed check argv and set standard path
        if (argc >= 1)
        {
            if (!strncasecmp(argv[0], "usb:/", 5))
            {
                strcpy(BootDevice, "usb:/");
                snprintf(ConfigPath, sizeof(ConfigPath), "usb:/%s%s", CONFIGPATH, CONFIGNAME);
                found = true;
            }
            else
            {
                strcpy(BootDevice, "sd:/");
                snprintf(ConfigPath, sizeof(ConfigPath), "sd:/%s%s", CONFIGPATH, CONFIGNAME);
                found = true;
            }
        }
    }

    SetDefault();

    return Load();
}

bool Settings::Load()
{
	char line[1024];
    char filepath[300];
    snprintf(filepath, sizeof(filepath), "%s", ConfigPath);

    if(!CheckFile(filepath))
        return false;

	file = fopen(filepath, "r");
	if (!file)
	{
        fclose(file);
        return false;
	}

	while (fgets(line, sizeof(line), file))
	{
		if (line[0] == '#') continue;

        this->ParseLine(line);
	}
	fclose(file);

	char * ptr = strrchr(filepath, '/');
	if(ptr)
        ptr[0] = '\0';

	Controls.Load(filepath);

	return true;

}

bool Settings::LoadLanguage(const char *path, int language)
{
    bool ret = false;

    if(language >= 0 || !path) {

        if(language < 0)
            return false;

        char filepath[150];
        char langpath[150];
        snprintf(langpath, sizeof(langpath), "%s", LanguagePath);
        if(langpath[strlen(langpath)-1] != '/')
        {
            char * ptr = strrchr(langpath, '/');
            if(ptr)
            {
                ptr++;
                ptr[0] = '\0';
            }
        }

        if(language == APP_DEFAULT)
        {
            strcpy(LanguagePath, langpath);
            gettextCleanUp();
            return true;
        }
        else if(language == CONSOLE_DEFAULT)
        {
            return LoadLanguage(NULL, CONF_GetLanguage()+2);
        }
        else if(language == JAPANESE)
        {
            snprintf(filepath, sizeof(filepath), "%s/japanese.lang", langpath);
        }
        else if(language == ENGLISH)
        {
            snprintf(filepath, sizeof(filepath), "%s/english.lang", langpath);
        }
        else if(language == GERMAN)
        {
            snprintf(filepath, sizeof(filepath), "%s/german.lang", langpath);
        }
        else if(language == FRENCH)
        {
            snprintf(filepath, sizeof(filepath), "%s/french.lang", langpath);
        }
        else if(language == SPANISH)
        {
            snprintf(filepath, sizeof(filepath), "%s/spanish.lang", langpath);
        }
        else if(language == ITALIAN)
        {
            snprintf(filepath, sizeof(filepath), "%s/italian.lang", langpath);
        }
        else if(language == DUTCH)
        {
            snprintf(filepath, sizeof(filepath), "%s/dutch.lang", langpath);
        }
        else if(language == S_CHINESE)
        {
            snprintf(filepath, sizeof(filepath), "%s/s_chinese.lang", langpath);
        }
        else if(language == T_CHINESE)
        {
            snprintf(filepath, sizeof(filepath), "%s/t_chinese.lang", langpath);
        }
        else if(language == KOREAN)
        {
            snprintf(filepath, sizeof(filepath), "%s%s/korean.lang", BootDevice, langpath);
        }

        ret = gettextLoadLanguage(filepath);
        if(ret)
            strncpy(LanguagePath, filepath, sizeof(LanguagePath));

    } else {

        ret = gettextLoadLanguage(path);
        if(ret)
            strncpy(LanguagePath, path, sizeof(LanguagePath));
    }

    return ret;
}

bool Settings::Reset()
{
    this->SetDefault();

    if(this->Save())
        return true;

	return false;
}

bool Settings::SetSetting(char *name, char *value)
{
    int i = 0;
	char password[100];

    if (strcmp(name, "MountMethod") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			MountMethod = i;
		}
		return true;
	}
	else if (strcmp(name, "CurrentSMBUser") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			CurrentSMBUser = i;
		}
		return true;
	}
	else if (strcmp(name, "CurrentFTPUser") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			CurrentFTPUser = i;
		}
		return true;
	}
	else if (strcmp(name, "LanguagePath") == 0) {
        strncpy(LanguagePath, value, sizeof(LanguagePath));
		return true;
	}
	else if (strcmp(name, "MusicVolume") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			MusicVolume = i;
		}
		return true;
	}
	else if (strcmp(name, "MountNTFS") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			MountNTFS = i;
		}
		return true;
	}
	else if (strcmp(name, "BGMLoopMode") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			BGMLoopMode = i;
		}
		return true;
	}
	else if (strcmp(name, "AutoConnect") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			AutoConnect = i;
		}
		return true;
	}
	else if (strcmp(name, "UpdateMetaxml") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			UpdateMetaxml = i;
		}
		return true;
	}
	else if (strcmp(name, "UpdateIconpng") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			UpdateIconpng = i;
		}
		return true;
	}
	else if (strcmp(name, "ClockMode") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClockMode = i;
		}
		return true;
	}
	else if (strcmp(name, "ScrollSpeed") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ScrollSpeed = i;
		}
		return true;
	}
	else if (strcmp(name, "BrowserMode") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			BrowserMode = i;
		}
		return true;
	}
	else if (strcmp(name, "ScreenshotFormat") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ScreenshotFormat = i;
		}
		return true;
	}
	else if (strcmp(name, "SoundblockCount") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			SoundblockCount = i;
		}
		return true;
	}
	else if (strcmp(name, "SoundblockSize") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			SoundblockSize = i;
		}
		return true;
	}
	else if (strcmp(name, "LoadMusicToMem") == 0) {
		if (sscanf(value, "%d", &i) == 1)
			LoadMusicToMem = i;
		return true;
	}
	else if (strcmp(name, "PDFLoadZoom") == 0) {
		PDFLoadZoom = atof(value);
		return true;
	}
	else if (strcmp(name, "CustomFontPath") == 0) {
        strncpy(CustomFontPath, value, sizeof(CustomFontPath));
		return true;
	}
	else if (strcmp(name, "MusicPath") == 0) {
        strncpy(MusicPath, value, sizeof(MusicPath));
		return true;
	}
	else if (strcmp(name, "MPlayerPath") == 0) {
        strncpy(MPlayerPath, value, sizeof(MPlayerPath));
		return true;
	}
	else if (strcmp(name, "UpdatePath") == 0) {
        strncpy(UpdatePath, value, sizeof(UpdatePath));
		return true;
	}
	else if (strcmp(name, "AppPath") == 0) {
        strncpy(AppPath, value, sizeof(AppPath));
		return true;
	}
	else if (strcmp(name, "TempPath") == 0) {
        strncpy(TempPath, value, sizeof(TempPath));
		return true;
	}
	else if (strcmp(name, "ScreenshotPath") == 0) {
        strncpy(ScreenshotPath, value, sizeof(ScreenshotPath));
		return true;
	}
	else if (strcmp(name, "SlideshowDelay") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			SlideshowDelay = i;
		}
		return true;
	}
	else if (strcmp(name, "ImageFadeSpeed") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ImageFadeSpeed = i;
		}
		return true;
	}
	else if (strcmp(name, "KeyboardDeleteDelay") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			KeyboardDeleteDelay = i;
		}
		return true;
	}
	else if (strcmp(name, "DeleteTempPath") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			DeleteTempPath = i;
		}
		return true;
	}
	else if (strcmp(name, "CopyThreadPrio") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			CopyThreadPrio = i;
		}
		return true;
	}
	else if (strcmp(name, "CopyThreadBackPrio") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			CopyThreadBackPrio = i;
		}
		return true;
	}
	else if (strcmp(name, "Rumble") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			Rumble = i;
		}
		return true;
	}
	else if (strcmp(name, "HideSystemFiles") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			HideSystemFiles = i;
		}
		return true;
	}
	else if (strcmp(name, "BackgroundUL") == 0) {
		sscanf(value, "%u", &BackgroundUL);
		return true;
	}
	else if (strcmp(name, "BackgroundUR") == 0) {
		sscanf(value, "%u", &BackgroundUR);
		return true;
	}
	else if (strcmp(name, "BackgroundBR") == 0) {
		sscanf(value, "%u", &BackgroundBR);
		return true;
	}
	else if (strcmp(name, "BackgroundBL") == 0) {
		sscanf(value, "%u", &BackgroundBL);
		return true;
	}
	else if (strcmp(name, "ProgressUL") == 0) {
		sscanf(value, "%u", &ProgressUL);
		return true;
	}
	else if (strcmp(name, "ProgressUR") == 0) {
		sscanf(value, "%u", &ProgressUR);
		return true;
	}
	else if (strcmp(name, "ProgressBR") == 0) {
		sscanf(value, "%u", &ProgressBR);
		return true;
	}
	else if (strcmp(name, "ProgressBL") == 0) {
		sscanf(value, "%u", &ProgressBL);
		return true;
	}
	else if (strcmp(name, "ProgressEmptyUL") == 0) {
		sscanf(value, "%u", &ProgressEmptyUL);
		return true;
	}
	else if (strcmp(name, "ProgressEmptyUR") == 0) {
		sscanf(value, "%u", &ProgressEmptyUR);
		return true;
	}
	else if (strcmp(name, "ProgressEmptyBR") == 0) {
		sscanf(value, "%u", &ProgressEmptyBR);
		return true;
	}
	else if (strcmp(name, "ProgressEmptyBL") == 0) {
		sscanf(value, "%u", &ProgressEmptyBL);
		return true;
	}
	else if (strcmp(name, "FileExtensions.VideoFiles") == 0) {
        FileExtensions.SetVideo(value);
		return true;
	}
	else if (strcmp(name, "FileExtensions.AudioFiles") == 0) {
        FileExtensions.SetAudio(value);
		return true;
	}
	else if (strcmp(name, "FileExtensions.ImageFiles") == 0) {
        FileExtensions.SetImage(value);
		return true;
	}
	else if (strcmp(name, "FileExtensions.ArchiveFiles") == 0) {
        FileExtensions.SetArchive(value);
		return true;
	}
	else if (strcmp(name, "FileExtensions.HomebrewFiles") == 0) {
        FileExtensions.SetHomebrew(value);
		return true;
	}
	else if (strcmp(name, "FileExtensions.FontFiles") == 0) {
        FileExtensions.SetFont(value);
		return true;
	}
	else if (strcmp(name, "FileExtensions.WiiBinaryFiles") == 0) {
        FileExtensions.SetWiiBinary(value);
		return true;
	}
	else if (strcmp(name, "FileExtensions.LanguageFiles") == 0) {
        FileExtensions.SetLanguageFiles(value);
		return true;
	}
	else if (strcmp(name, "FileExtensions.PDFFiles") == 0) {
        FileExtensions.SetPDF(value);
		return true;
	}
	else if (strcmp(name, "FileExtensions.WiiXplorerMovies") == 0) {
        FileExtensions.SetWiiXplorerMovies(value);
		return true;
	}
    else if (strcmp(name, "FTPServer.AutoStart") == 0) {
        if (sscanf(value, "%d", &i) == 1) {
            FTPServer.AutoStart = i;
        }
        return true;
    }
    else if (strcmp(name, "FTPServer.CPassword") == 0) {
        if (strcmp(value, "") != 0)
            DecryptString(value, password);
        strncpy(FTPServer.Password, ((strcmp(value, "") != 0) ? password : value), sizeof(FTPServer.Password));
        return true;
    }
    else if (strcmp(name, "FTPServer.Port") == 0) {
        if (sscanf(value, "%d", &i) == 1) {
            FTPServer.Port = i;
        }
        return true;
    }
	else {
	    char temp[80];
	    int n = 0;

	    for(n = 0; n < MAXSMBUSERS; n++) {
	        sprintf(temp, "SMBUser[%d].Host", n+1);
            if (stricmp(name, temp) == 0) {
                strncpy(SMBUser[n].Host, value, sizeof(SMBUser[n].Host));
                return true;
            }
            sprintf(temp, "SMBUser[%d].User", n+1);
            if (stricmp(name, temp) == 0) {
                strncpy(SMBUser[n].User, value, sizeof(SMBUser[n].User));
                return true;
            }
            sprintf(temp, "SMBUser[%d].CPassword", n+1);
            if (stricmp(name, temp) == 0) {
                if (strcmp(value, "") != 0)
                    DecryptString(value, password);
                strncpy(SMBUser[n].Password, ((strcmp(value, "") != 0) ? password : value), sizeof(SMBUser[n].Password));
                return true;
            }
            sprintf(temp, "SMBUser[%d].SMBName", n+1);
            if (stricmp(name, temp) == 0) {
                strncpy(SMBUser[n].SMBName, value, sizeof(SMBUser[n].SMBName));
                return true;
            }
	    }

	    for(n = 0; n < MAXFTPUSERS; n++) {
	        sprintf(temp, "FTPUser[%d].Host", n+1);
            if (stricmp(name, temp) == 0) {
                strncpy(FTPUser[n].Host, value, sizeof(FTPUser[n].Host));
                return true;
            }
            sprintf(temp, "FTPUser[%d].User", n+1);
            if (stricmp(name, temp) == 0) {
                strncpy(FTPUser[n].User, value, sizeof(FTPUser[n].User));
                return true;
            }
            sprintf(temp, "FTPUser[%d].CPassword", n+1);
            if (stricmp(name, temp) == 0) {
                if (strcmp(value, "") != 0)
                    DecryptString(value, password);
                strncpy(FTPUser[n].Password, ((strcmp(value, "") != 0) ? password : value), sizeof(FTPUser[n].Password));
                return true;
            }
            sprintf(temp, "FTPUser[%d].FTPPath", n+1);
            if (stricmp(name, temp) == 0) {
                strncpy(FTPUser[n].FTPPath, value, sizeof(FTPUser[n].FTPPath));
                return true;
            }
            sprintf(temp, "FTPUser[%d].Port", n+1);
            if (stricmp(name, temp) == 0) {
				if (sscanf(value, "%d", &i) == 1) {
					FTPUser[n].Port = i;
				}
                return true;
            }
            sprintf(temp, "FTPUser[%d].Passive", n+1);
            if (stricmp(name, temp) == 0) {
				if (sscanf(value, "%d", &i) == 1) {
					FTPUser[n].Passive = i;
				}
                return true;
            }
		}
	}

    return false;
}

void Settings::ParseLine(char *line)
{
    char temp[1024], name[1024], value[1024];

    strncpy(temp, line, sizeof(temp));

    char * eq = strchr(temp, '=');

    if(!eq) return;

    *eq = 0;

    this->TrimLine(name, temp, sizeof(name));
    this->TrimLine(value, eq+1, sizeof(value));

	this->SetSetting(name, value);
}

void Settings::TrimLine(char *dest, char *src, int size)
{
	int len;
	while (*src == ' ') src++;
	len = strlen(src);
	while (len > 0 && strchr(" \r\n", src[len-1])) len--;
	if (len >= size) len = size-1;
	strncpy(dest, src, len);
	dest[len] = 0;
}
