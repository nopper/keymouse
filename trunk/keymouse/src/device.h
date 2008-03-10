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

#ifndef DEVICE_H
#define DEVICE_H

#include "logger.h"

/*! \brief Class that represents a mouse device (uinput)
 * \author Francesco Piccinno <stack.box@gmail.com>
 */
class Device
{
public:
	/*! \brief Default constructor */
	Device();

	/*! \brief Default destructor */
	~Device();

	/*! \brief Allocate the device
	 * \return true if the device was correctly allocated
	 * \param dev_str the uinput deviceA
	 */
	bool allocate(const char *dev_str);

	/*! \brief Moves the pointer (relative)
	 * \param rel_x x coord
	 * \param rel_y y coord
	 */
	void moveTo(int rel_x, int rel_y);

	/*! \brief Return true if the device is valid */
	bool isValid() { return m_valid; };

	/*! \brief Send a click (returns true if ok)
	 * \param btn BTN_{LEFT | RIGHT | MIDDLE}
	 * \param pressed pressed or released
	 */
	bool sendClick(int btn, int pressed);
private:
	int m_fd;	//! file descriptor
	bool m_valid;	//! valid flag
};

#endif
