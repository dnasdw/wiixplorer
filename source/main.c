/*
 *      loadMii loader v0.3
 *      main.c
 *      http://code.google.com/p/loadmii
 *
 *      Copyright 2009 The Lemon Man
 *      Thanks to luccax, Wiipower, Aurelio and crediar
 *      usbGecko powered by Nuke
 *      GX Graphics & mem2 load added by Dimok (thanks to Tantric)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include <gccore.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ogc/machine/processor.h>

#include "video.h"
#include "background_image.h"
#include "dolloader.h"
#include "filelist.h"
#include "cfg.h"
#include "fatmounter.h"

void __exception_setreload(int t);

int main(int argc, char **argv)
{
	u32 cookie;
	FILE *exeFile = NULL;
	void * exeBuffer = (void *)EXECUTABLE_MEM_ADDR;
	u32 exeSize = 0;
	u32 exeEntryPointAddress = 0;
	entrypoint exeEntryPoint;
	__exception_setreload(0);

    IOS_ReloadIOS(58);

	/* int videomod */
	InitVideo();
	/* get imagedata */
	u8 * imgdata = GetImageData();
	fadein(imgdata);
	/* check devices */
	SDCard_Init();
	USBDevice_Init();

	char cfgpath[256];
	bool result = false;

	sprintf(cfgpath, "sd:/config/WiiXplorer/WiiXplorer.cfg");
	result = cfg_parsefile(cfgpath, &cfg_set);
	if(!result) //no cfg-File on SD: try USB:
	{
		sprintf(cfgpath, "usb:/config/WiiXplorer/WiiXplorer.cfg");
		result = cfg_parsefile(cfgpath, &cfg_set);
	}

    if(result)
    {
        sprintf(cfgpath, "%sboot.dol", update_path);
		/* Open dol File and check exist */
        exeFile = fopen (cfgpath, "rb");
        if (exeFile==NULL)
        {
            sprintf(cfgpath, "%sboot.elf", update_path);
            exeFile = fopen (cfgpath,"rb");
        }
        if (exeFile==NULL)
			result = false;
        else
        result = true;
    }

	if(!result) // non cfg-File loaded or update_path not set
	{
		/* Open dol File and check exist */
		sprintf(cfgpath, "sd:/apps/WiiExplorer/boot.dol");
		exeFile = fopen (cfgpath ,"rb");
		if (exeFile==NULL)
		{
			sprintf(cfgpath, "sd:/apps/WiiExplorer/boot.elf");
			exeFile = fopen (cfgpath ,"rb");
		}
		if (exeFile==NULL)
		{
			sprintf(cfgpath, "usb:/apps/WiiExplorer/boot.dol");
			exeFile = fopen (cfgpath ,"rb");
		}
		if (exeFile==NULL)
		{
			sprintf(cfgpath, "usb:/apps/WiiExplorer/boot.elf");
			exeFile = fopen (cfgpath ,"rb");
		}
		if (exeFile==NULL)
		{
			sprintf(cfgpath, "sd:/apps/WiiXplorer/boot.dol");
			exeFile = fopen (cfgpath ,"rb");
		}
		if (exeFile==NULL)
		{
			sprintf(cfgpath, "sd:/apps/WiiXplorer/boot.elf");
			exeFile = fopen (cfgpath ,"rb");
		}
		if (exeFile==NULL)
		{
			sprintf(cfgpath, "usb:/apps/WiiXplorer/boot.dol");
			exeFile = fopen (cfgpath ,"rb");
		}
		if (exeFile==NULL)
		{
			sprintf(cfgpath, "usb:/apps/WiiXplorer/boot.elf");
			exeFile = fopen (cfgpath ,"rb");
		}
		// if nothing found exiting
		if (exeFile==NULL)
		{
            fadeout(imgdata);
            fclose (exeFile);
            SDCard_deInit();
            USBDevice_deInit();
            StopGX();
            free(imgdata);
			SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
		}
	}

	fseek (exeFile, 0, SEEK_END);
	exeSize = ftell(exeFile);
	rewind (exeFile);

	if(fread (exeBuffer, 1, exeSize, exeFile) != exeSize)
	{
		fadeout(imgdata);
        fclose (exeFile);
        SDCard_deInit();
        USBDevice_deInit();
        StopGX();
        free(imgdata);
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}
	fclose (exeFile);

	/* load entry point */
	struct __argv args;
	bzero(&args, sizeof(args));
	args.argvMagic = ARGV_MAGIC;
	args.length = strlen(cfgpath) + 2;
	args.commandLine = (char*)malloc(args.length);
	if (!args.commandLine) SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	strcpy(args.commandLine, cfgpath);
	args.commandLine[args.length - 1] = '\0';
	args.argc = 1;
	args.argv = &args.commandLine;
	args.endARGV = args.argv + 1;

	u8 * appboot_buff = (u8 *) malloc(app_booter_dol_size);
	if(!appboot_buff)
	{
		fadeout(imgdata);
        SDCard_deInit();
        USBDevice_deInit();
        StopGX();
        free(imgdata);
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}

    memcpy(appboot_buff, app_booter_dol, app_booter_dol_size);

	exeEntryPointAddress = load_dol_image(appboot_buff, &args);

    if(appboot_buff)
        free(appboot_buff);

	fadeout(imgdata);
	SDCard_deInit();
	USBDevice_deInit();
	StopGX();
	free(imgdata);

	if (exeEntryPointAddress == 0)
	{
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}

	exeEntryPoint = (entrypoint) exeEntryPointAddress;
	/* cleaning up and load dol */
	SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	_CPU_ISR_Disable (cookie);
	__exception_closeall ();
	exeEntryPoint ();
	_CPU_ISR_Restore (cookie);
	return 0;
}
