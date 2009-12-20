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
 * for Wii-FileXplorer 2009
 ***************************************************************************/
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Settings.h"
#include "FileOperations/fileops.h"
#include "Language/gettext.h"

#define DEFAULT_APP_PATH    "sd:/apps/WiiExplorer/"
#define CONFIGPATH          "sd:/config/WiiXplorer/"
#define CONFIGNAME          "WiiXplorer.cfg"
#define LANGPATH      	    "sd:/config/WiiXplorer/Languages/"

Settings::Settings()
{
    this->SetDefault();
}

Settings::~Settings()
{
}

void Settings::SetDefault()
{
    MountMethod = 0;
    MountNTFS = off;
    CurrentUser = 0;
    AutoConnect = off;
    sprintf(CustomFontPath, "%sfont.ttf", CONFIGPATH);
    strcpy(LanguagePath, "");
    strcpy(UpdatePath, DEFAULT_APP_PATH);

    for(int i = 0; i < MAXSMBUSERS; i++) {
        strcpy(SMBUser[i].Host, "");
        strcpy(SMBUser[i].User, "");
        strcpy(SMBUser[i].Password, "");
        strcpy(SMBUser[i].SMBName, "");
	}
}

bool Settings::Save()
{
    char filepath[300];
    snprintf(filepath, sizeof(filepath), "%s%s", CONFIGPATH, CONFIGNAME);

    CreateSubfolder(CONFIGPATH);
    file = fopen(filepath, "w");

    if(!file) {
        fclose(file);
        return false;
    }

    fprintf(file, "# WiiXplorer Settingsfile\n");
	fprintf(file, "# Note: This file is automatically generated\n\n");
	fprintf(file, "# Main Settings\n\n");
	fprintf(file, "MountMethod = %d\n", MountMethod);
	fprintf(file, "CurrentUser = %d\n", CurrentUser);
	fprintf(file, "AutoConnect = %d\n", AutoConnect);
	fprintf(file, "LanguagePath = %s\n", LanguagePath);
	fprintf(file, "MusicVolume = %d\n", MusicVolume);
	fprintf(file, "MountNTFS = %d\n", MountNTFS);
	fprintf(file, "CustomFontPath = %s\n", CustomFontPath);
	fprintf(file, "UpdatePath = %s\n", UpdatePath);

	fprintf(file, "\n# SMB Setup Information\n\n");
    for(int i = 0; i < MAXSMBUSERS; i++) {
        fprintf(file, "SMBUser[%d].Host = %s\n", i+1, SMBUser[i].Host);
        fprintf(file, "SMBUser[%d].User = %s\n", i+1, SMBUser[i].User);
        fprintf(file, "SMBUser[%d].Password = %s\n", i+1, SMBUser[i].Password);
        fprintf(file, "SMBUser[%d].SMBName = %s\n\n", i+1, SMBUser[i].SMBName);
	}

	fclose(file);

	return true;
}

bool Settings::Load()
{
	char line[300];
    char filepath[300];
    snprintf(filepath, sizeof(filepath), "%s%s", CONFIGPATH, CONFIGNAME);

	file = fopen(filepath, "r");
	if (!file) {
		return false;
	}
	while (fgets(line, sizeof(line), file)) {

		if (line[0] == '#') continue;

        this->ParseLine(line);
	}
	fclose(file);

	return true;

}

bool Settings::LoadLanguage(const char *path, int language)
{
    bool ret = false;

    if(language >= 0 || !path) {

        if(language < 0)
            return false;

        char filepath[300];

        if(language == APP_DEFAULT) {
            strcpy(LanguagePath, "");
            gettextCleanUp();
            return true;
        }
        else if(language == CONSOLE_DEFAULT) {
            return this->LoadLanguage(NULL, CONF_GetLanguage()+2);
        }
        else if(language == JAPANESE) {
            snprintf(filepath, sizeof(filepath), "%s/japanese.lang", LANGPATH);
        }
        else if(language == ENGLISH) {
            snprintf(filepath, sizeof(filepath), "%s/english.lang", LANGPATH);
        }
        else if(language == GERMAN) {
            snprintf(filepath, sizeof(filepath), "%s/german.lang", LANGPATH);
        }
        else if(language == FRENCH) {
            snprintf(filepath, sizeof(filepath), "%s/french.lang", LANGPATH);
        }
        else if(language == SPANISH) {
            snprintf(filepath, sizeof(filepath), "%s/spanish.lang", LANGPATH);
        }
        else if(language == ITALIAN) {
            snprintf(filepath, sizeof(filepath), "%s/italian.lang", LANGPATH);
        }
        else if(language == DUTCH) {
            snprintf(filepath, sizeof(filepath), "%s/dutch.lang", LANGPATH);
        }
        else if(language == S_CHINESE) {
            snprintf(filepath, sizeof(filepath), "%s/s_chinese.lang", LANGPATH);
        }
        else if(language == T_CHINESE) {
            snprintf(filepath, sizeof(filepath), "%s/t_chinese.lang", LANGPATH);
        }
        else if(language == KOREAN) {
            snprintf(filepath, sizeof(filepath), "%s/korean.lang", LANGPATH);
        }

        ret = gettextLoadLanguage(filepath);
        if(ret)
            strncpy(LanguagePath, filepath, sizeof(this->LanguagePath));

    } else {

        ret = gettextLoadLanguage(path);
        if(ret)
            strncpy(LanguagePath, path, sizeof(this->LanguagePath));
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

    if (strcmp(name, "MountMethod") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			MountMethod = i;
		}
		return true;
	}
	else if (strcmp(name, "CurrentUser") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			CurrentUser = i;
		}
		return true;
	}
	else if (strcmp(name, "AutoConnect") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			AutoConnect = i;
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
	else if (strcmp(name, "CustomFontPath") == 0) {
        strncpy(CustomFontPath, value, sizeof(CustomFontPath));
		return true;
	}
	else if (strcmp(name, "UpdatePath") == 0) {
        strncpy(UpdatePath, value, sizeof(UpdatePath));
		return true;
	}
	else {
	    char temp[80];
	    int n = 0;

	    for(n = 0; n < MAXSMBUSERS; n++) {
	        sprintf(temp, "SMBUser[%d].Host", n+1);
            if (strcmp(name, temp) == 0) {
                strncpy(SMBUser[n].Host, value, sizeof(SMBUser[n].Host));
                return true;
            }
            sprintf(temp, "SMBUser[%d].User", n+1);
            if (strcmp(name, temp) == 0) {
                strncpy(SMBUser[n].User, value, sizeof(SMBUser[n].User));
                return true;
            }
            sprintf(temp, "SMBUser[%d].Password", n+1);
            if (strcmp(name, temp) == 0) {
                strncpy(SMBUser[n].Password, value, sizeof(SMBUser[n].Password));
                return true;
            }
            sprintf(temp, "SMBUser[%d].SMBName", n+1);
            if (strcmp(name, temp) == 0) {
                strncpy(SMBUser[n].SMBName, value, sizeof(SMBUser[n].SMBName));
                return true;
            }
	    }
	}

    return false;
}

void Settings::ParseLine(char *line)
{
    char temp[200], name[200], value[200];

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
