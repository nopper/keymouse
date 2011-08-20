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

#ifndef APP_H
#define APP_H

#include <sys/time.h>
#include <linux/input.h>
#include "configfile.h"
#include "shortcut.h"

#define MAX_MODS 8

#define MOD1	1
#define MOD2	1 << 1
#define MOD3	1 << 2
#define MOD4	1 << 3
#define MOD5	1 << 4
#define MOD6	1 << 5
#define MOD7	1 << 6
#define MOD8	1 << 7

/*! \brief The main class for keymouse
 * \author Francesco Piccinno <stack.box@gmail.com>
 */
class App
{
public:
	/*! \brief Default constructor
	 * \param file path to config file
	 */
	App(const char *file);
	
	/*! \brief Constructor copy */
	App(App& old);
	
	/*! \brief Default destructor */
	~App();
	
	/*! \brief Start the application main loop
	 * \return -1 on error or 0 if everything is ok
	 */
	int run();

	/*! \brief Stop the main loop
	 * \param destroy
	 * \return
	 * 	true if you want to destroy the mouse device
	 * 	false if you want to hold his value (see also SIGHUP handling)
	 */
	void stop(bool destroy);

	/*! \brief Return the path to config file */
	const string& getFileName();
private:
	/*! \brief Called by constructors to allocate the object */
	void allocateObject();

	/*! \brief Return a pointer to an input_event structure input
	 * \brief representing the current event of the device
	 */
	struct input_event* readKeyCode();

	/*! \brief Parse a single keycode
	 * \param pressed pressed or released event
	 * \param code represents the keycode
	 */
	void parseKey(bool pressed, unsigned short code);

	/*! \brief Check if the code is a mod or a button key
	 * \return true if it is
	 */
	bool isMod(unsigned short code);

	/*! \brief Update the keycode sequence.
	 * \return true if the sequence is correct and can be evaluated.
	 * \param code current keycode
	 * \param pressed pressed or released event
	 */
	bool updateSequence(unsigned short code, bool pressed);

	/*! \brief Return a char representing the status of m_modes */
	char makeDigest();
	
	/*! \brief Notifies the locked status
	 * \param flag if locked or not
	 */
	void notifyLock(bool flag);
private:
	int m_fd;		//! file descriptor for the device
	ConfigFile m_config;	//! config object to get options
	struct timeval m_time;	//! current time
	
	Device *m_dev;		//! mouse device
	
	bool
		m_dump,		//! dump flag
		m_touchscreen,	//! touchscreen flag, ie give absolute coords instead of relative
		m_exiting;	//! exiting flag
	
	int m_touchscreen_max_x;	//! Touchscreen x coord max
	int m_touchscreen_max_y;	//! Touchscreen y coord max
	int m_touchscreen_autorepeat;	//! autorepeat abs coords, ie finger down.
	int m_touchscreen_autorepeat_rate;	//! ::select() timeout
	int m_abs_x;		//! Touchscreen x coord
	int m_abs_y;		//! Touchscreen y coord

	int m_maxAccel;		//! accel limit

	mod_key *m_modes;	//! modes
	unsigned short *m_keys;	//! bkeys
	
	unsigned int
		m_maxModes,	//! limit for modes
		m_maxKeys,	//! limit for bkeys
		m_notify;	//! type of notice
	
	char *m_notifyProg, **m_notifyProgArgs;
	
	vector<unsigned short> m_ordModes;	//! key sequence vector
	map<char, vector<Shortcut> > m_shortcuts; //! shortcuts
};

#endif
