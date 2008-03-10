/*
      Copyright (C) 2008 by Francesco Piccinno
      stack.box@gmail.com
      BSD License
*/

#include "shortcut.h"
#include <linux/input.h>

bool Shortcut::locked = false;

Shortcut::Shortcut(const std::string& value, const std::string& act, const mod_key *mods) : m_action(act), m_digest('\0')
{
	int index = 0;
	bool is_code = true;
	std::string::size_type i = 0;
	std::string mod("");
	
	for (; i < value.length(); i++)
	{
		if (value[i] != ':')
		{
			mod += value[i];
			continue;
		}
		
		index = ::atoi(mod.c_str());

		if (is_code)
		{
			is_code = false;
			m_code = (unsigned short)index;
		}
		else
			m_digest |= 1 << index;
		
		mod = "";
	}
}


Shortcut::~Shortcut()
{
}


char Shortcut::getDigest()
{
	return m_digest;
}


int Shortcut::execAction(Device& dev, int pressed, unsigned short code, float mul)
{
	float x = 0.0, y = 0.0;

	if (code != 0 && m_code != code)
		return false;
	
	if (m_action == "lock")
	{
		locked = !locked;
		
		Logger::Log(DEBUG, "Locked: %s", (locked) ? "true" : "false");
		
		return -1;
	}
	
	if (m_action == "click")
		return dev.sendClick(BTN_LEFT, pressed);
	
	if (m_action == "rclick")
		return dev.sendClick(BTN_RIGHT, pressed);
	
	if (m_action == "3click")
		return dev.sendClick(BTN_MIDDLE, pressed);
	
	if (m_action == "right" || m_action == "rup" || m_action == "rdown")
		x = +1;
	if (m_action == "left" || m_action == "lup" || m_action == "ldown")
		x = -1;
	if (m_action == "down" || m_action == "ldown" || m_action == "rdown")
		y = +1;
	if (m_action == "up" || m_action == "lup" || m_action == "rup")
		y = -1;
	
	x *= mul;
	y *= mul;
	
	dev.moveTo(x, y);

	return 0;
}
