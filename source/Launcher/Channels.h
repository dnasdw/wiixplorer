/***************************************************************************
 * Copyright (C) 2010
 * by dude
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
 * Channel Launcher Class
 *
 * for WiiXplorer 2010
 ***************************************************************************/

#ifndef _CHANNELS_H_
#define _CHANNELS_H_

#include <vector>

typedef struct
{
	u64  title;
	char name[256];
} Channel;

class Channels
{
    public:
        Channels();
        ~Channels();

		void Launch(Channel channel);
		Channel Get(int i) { return channels.at(i); }
		int Count() { return channels.size(); }
		char *GetName(int i) { return channels.at(i).name; }

	private:
		std::vector<Channel> channels;

		bool GetNameFromApp(u64 title, char *name);
		u64* GetChannels(u32* cnt);
		void Search();
};

#endif
