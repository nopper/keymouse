/****************************************************************************
 *   Copyright (C) 2008 by Francesco Piccinno                               *
 *   stack.box@gmail.com                                                    *
 *                                                                          *
 *   Permission is hereby granted, free of charge, to any person obtaining  *
 *   a copy of this software and associated documentation files (the        *
 *   "Software"), to deal in the Software without restriction, including    *
 *   without limitation the rights to use, copy, modify, merge, publish,    *
 *   distribute, sublicense, and/or sell copies of the Software, and to     *
 *   permit persons to whom the Software is furnished to do so, subject to  *
 *   the following conditions:                                              *
 *                                                                          *
 *   The above copyright notice and this permission notice shall be         *
 *   included in all copies or substantial portions of the Software.        *
 *                                                                          *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. *
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR      *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,  *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 ****************************************************************************/

#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <string>
#include <X11/Xlib.h>
#include "device.h"

/*! \brief Valid actions for config file
 * \author Francesco Piccinno <stack.box@gmail.com>
 */
static const char *actions[] = {
	"left",
	"right",
	"up",
	"down",
	"lup",
	"ldown",
	"rup",
	"rdown",
	"click",
	"rclick",
	"3click",
	"lock",
	NULL
};

/*! \brief Struct representig a keycode
 * \author Francesco Piccinno <stack.box@gmail.com>
 */
typedef struct s_mod_key
{
	unsigned short code;	//! key code
	bool pressed;		//! pressed or not
} mod_key;

/*! \brief Shortcut class
 * \author Francesco Piccinno <stack.box@gmail.com>
 */
class Shortcut
{
public:
	/*! \brief Default constructor
	 * \param value the value of shortcut like ("53:0:1:")
	 * \param act the action for shortcut (see also actions)
	 * \param mods an array containing the used mods
	 */
	Shortcut(const std::string& value, const std::string& act, const mod_key *mods);

	/*! \brief Default destructor */
	~Shortcut();
	
	/*! \brief Return the digest for the shortcut */
	char getDigest();

	/*!
	 * \brief Execute the proper action for digest
	 * 	\return +1 if action was a *click
	 * 	\return -1 if action was a lock
	 * 	\return  0 for anything else
	 * \param dev is the reference to a device instance
	 * \param pressed if the action is *click send a press/release event
	 * \param mul is the multiplier for moving the cursor with accel
	 */
	int execAction(Device& dev, int pressed, unsigned short code, float mul);
private:
	char m_digest;		//! the digest for shortcut
	std::string m_action;	//! the action name
	unsigned short m_code;	//! the keycode that discriminates the current shortcut
public:
	static bool locked;	//! locked flag (for lock action, usefull for moving/resizing windows)
};

#endif
