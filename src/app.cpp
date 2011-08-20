/*
      Copyright (C) 2008 by Francesco Piccinno
      stack.box@gmail.com
      BSD License
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "app.h"

using namespace std;

App::App(const char *file) : m_config(file), m_dev(NULL), m_ordModes(), m_exiting(false)
{
	allocateObject();
}


App::App(App& old) : m_config(old.getFileName().c_str()), m_ordModes(), m_exiting(false)
{
	Logger::Log(INFO, "Reloading options ..");
	
	allocateObject();
	
	m_dev = old.m_dev;
	
	if (m_dump)
		delete m_dev;
}


void App::allocateObject()
{
	m_dump  = static_cast <bool> (m_config.getOptionAsInt("dump", 0));

	m_maxModes = static_cast <unsigned int> (m_config.getOptionAsInt("max modes", 6));
	m_maxAccel = m_config.getOptionAsInt("max accel", 10);
	m_notify = m_config.getOptionAsInt("notify", 0);
	m_touchscreen = m_config.getOptionAsInt("touchscreen", 0);	
	m_touchscreen_autorepeat = m_config.getOptionAsInt("touchscreen_autorepeat", 0);	
	m_touchscreen_autorepeat_rate = m_config.getOptionAsInt("touchscreen_autorepeat_rate", 0);	
	m_touchscreen_max_x = m_config.getOptionAsInt("touchscreen_max_x", 10);	
	m_touchscreen_max_y = m_config.getOptionAsInt("touchscreen_max_y", 10);	

	std::vector<string> tokens;
	m_config.getOptionAndSplit("notify exec", " ", tokens);

	if (!tokens.empty())
	{
		m_notifyProg = strdup(tokens[0].c_str());
		
		m_notifyProgArgs = new char* [tokens.size()];

		memset(m_notifyProgArgs, 0, tokens.size() - 1);

		m_notifyProgArgs[0] = strdup(m_notifyProg);
		
		for (int i = 2; i < tokens.size() - 1; i++)
			m_notifyProgArgs[i - 1] = strdup(tokens[i].c_str());
	}
	
	tokens.clear();
	m_config.getOptionAndSplit("modes", ",", tokens);

	if (MAX_MODS < m_maxModes) m_maxModes = MAX_MODS;
	if (tokens.size() < MAX_MODS) m_maxModes = tokens.size();
	
	m_modes = new mod_key[m_maxModes];
	memset(m_modes, 0, sizeof(m_modes));

	Logger::Log(DEBUG, "Allocating %d modes keys.", m_maxModes);

	for (int i = 0; i < m_maxModes; i++)
	{
		Logger::Log(DEBUG, "Converting: %s -> %d@%d", tokens[i].c_str(), ::atoi(tokens[i].c_str()), i);
		
		m_modes[i].code = static_cast <unsigned short> (::atoi(tokens[i].c_str()));
		m_modes[i].pressed = false;
	}

	tokens.clear();
	m_config.getOptionAndSplit("bkeys", ",", tokens);
	m_maxKeys = tokens.size();

	m_keys = new unsigned short[m_maxKeys];
	memset(m_keys, 0, sizeof(m_keys));

	for (int i = 0; i < m_maxKeys; i++)
	{
		Logger::Log(DEBUG, "Converting: %s -> %d", tokens[i].c_str(), ::atoi(tokens[i].c_str()));
		m_keys[i] = static_cast <unsigned short> (::atoi(tokens[i].c_str()));
	}

	int i = 0;
	const char *p = actions[i];
	
	while (p != NULL) {
		
		Shortcut s(m_config.getOption(p), p, m_modes);
		m_shortcuts[s.getDigest()].push_back(s);
		
		Logger::Log(DEBUG, "%d shortcuts for digets %d", m_shortcuts[s.getDigest()].size(), static_cast <int> (s.getDigest()));
		
		p = actions[++i];
	}
}


const string& App::getFileName()
{
	return m_config.getFileName();
}


void App::stop(bool destroy)
{
	if (m_exiting)
		return;
	
	m_exiting = true;
	
	if (destroy)
	{
		delete m_dev;
		m_dev = NULL;
	}
}


App::~App()
{
	stop(true);

	delete [] m_modes;
	delete [] m_keys;

	if (m_notifyProg)
	{
		free(m_notifyProg);
	
		for (int i = 0; m_notifyProgArgs[i] != NULL; i++)
			free(m_notifyProgArgs[i]);
		
		delete [] m_notifyProgArgs;
	}
	
	// TODO: free map ecc..
}


struct input_event* App::readKeyCode()
{
	int ret;
	static struct input_event event;
	
	if ((ret = ::read(m_fd, &event, sizeof(struct input_event))) < sizeof(struct input_event))
		return NULL;
	
	return &event;
}


bool App::isMod(unsigned short code)
{
	for (int i = 0; i < m_maxModes; i++)
		if (m_modes[i].code == code)
			return true;
	
	for (int i = 0; i < m_maxKeys; i++)
		if (m_keys[i] == code)
			return true;

	return false;
}


int App::run()
{
	int ready, i;
	fd_set fds;
	struct input_event  *keycode;
	unsigned short last_keycode;
	unsigned int last_keyvalue;
	
	if ((m_fd = ::open(m_config.getOption("device").c_str(), O_RDWR | O_NDELAY)) < 0)
	{
		Logger::Log(ERROR, "Unable to open() the device: %s", m_config.getOption("device").c_str());
		Logger::Log(ERROR, "Mumble muble .. do you have the right perm (+rw)?");
		return -1;
	}
	
	if (!m_dump)
	{
		if (!m_dev)
			m_dev = new Device(m_touchscreen, m_touchscreen_max_x, m_touchscreen_max_y);

		if (!m_dev->isValid() && !m_dev->allocate(m_config.getOption("udevice").c_str()))
		{
			Logger::Log(ERROR, "Unabe to open() uinput device: %s", m_config.getOption("udevice").c_str());
			Logger::Log(ERROR, "Uhm.. very strange.. try to modprobe uinput or change the perm");
			return -1;
		}
	}
	
	gettimeofday(&m_time, NULL);
	
	while (!m_exiting && m_fd != -1)
	{
		FD_ZERO(&fds);
		FD_SET(m_fd, &fds);

		struct timeval t;
		t.tv_sec = 0;
		if(m_touchscreen_autorepeat_rate) 
		{
			t.tv_usec = m_touchscreen_autorepeat_rate;
		} 
		else 
		{
			t.tv_usec = 300;
		}

		ready = ::select(m_fd + 1, &fds, NULL, NULL, &t);
		
		if (ready == -1 && !m_exiting)
		{
			Logger::Log(ERROR, "Error in select()");
			return -1;
		}
		else if (ready == 0) 
		{
			static int keydown = 0;
		        if( m_touchscreen && m_touchscreen_autorepeat && ! m_dump ) 
			{
				if(last_keyvalue) // if holding key down, repeat movement
				{
					parseKey(true, last_keycode);
					keydown = true;
				}
				else if(keydown) // else key up event, change pressure to 0, keeping current coords
				{
					m_dev->moveTo(m_dev->m_abs_x, m_dev->m_abs_y, last_keyvalue);
					keydown = false;
				}
			}
			continue;
		}
		else
		{
			/* Skip null or syn packet */
			bool pressed = true;
			
			if ((keycode = readKeyCode()) == NULL || keycode->type != EV_KEY)
				continue;
			
			if (keycode->value == 0)
				pressed = false;

			last_keycode = keycode->code;
			last_keyvalue = keycode->value;

			m_time.tv_usec = keycode->time.tv_usec - m_time.tv_usec;
			m_time.tv_sec = keycode->time.tv_sec - m_time.tv_sec;
			
			parseKey(pressed, keycode->code);
			
			//m_time = keycode->time;
			gettimeofday(&m_time, NULL);
		}
	}

	if (m_fd != -1)
	{
		close(m_fd);
		m_fd = -1;
	}

	return 0;
}


bool App::updateSequence(unsigned short code, bool pressed)
{
	/* Se pressed bisogna iterare il vector e se non presente aggiungere
	 * Se !pressed bisogna iterare il vector e se presente rimuovere
	 */

	bool to_add = true;
	vector<unsigned short>::iterator it;
	for (it = m_ordModes.begin(); it != m_ordModes.end(); it++)
	{
		if (*it == code)
		{
			if (pressed) to_add = false;
			else m_ordModes.erase(it);
			
			break;
		}
	}

	if (pressed && to_add)
		m_ordModes.push_back(code);

	/* Controlliamo che ci siano almeno 2 tasti nella sequenza */

	if (m_ordModes.size() < 2)
		return false;
	
	/* Impostiamo tutti i tasti nella sequenza m_ordModes in
	 * m_modes eccetto l'ultimo che e' il discriminatore della sequenza.
	 * In seguito generiamo il digest.
	 */
	
	for (int i = 0; i < m_maxModes; i++)
		m_modes[i].pressed = false;

	for (it = m_ordModes.begin(); it != m_ordModes.end() - 1; it++)
	{
		for (int i = 0; i < m_maxModes; i++)
		{
			if (*it == m_modes[i].code)
				m_modes[i].pressed = true;
		}
	}

	return true;
}


char App::makeDigest()
{
	char digest = '\0';
	for (int i = 0; i < m_maxModes; i++)
		digest = (m_modes[i].pressed) ? digest | (1 << i) : digest /*& ~(1 << i)*/;
	
	return digest;
}


void App::parseKey(bool pressed, unsigned short code)
{
	char digest;
	static bool last_pressed = false;
	static float accel_factor = (static_cast <float> (m_maxAccel) / 100.0);

	static Shortcut* last_action = NULL;
	
	if (m_dump)
		Logger::Log(INFO, "Grabbed! code: %d pressed: %d", code, pressed);
	else if (isMod(code))
	{
		accel_factor = (static_cast <float> (m_maxAccel) / 100.0) + ((last_pressed) ? accel_factor : 0);

		if (updateSequence(code, pressed))
		{
			digest = makeDigest();

			/* Dovremmo inoltre controllare che l'ultima azione
			 * sia valida (quindi *click) e se cosi mandare il
			 * release event
			 */

			/* Evitiamo di mandare messaggi replicati per i click */

			for (int i = 0; i < m_shortcuts[digest].size(); i++)
			{
				if (last_action == &m_shortcuts[digest][i])
					continue;

				int ret = m_shortcuts[digest][i].execAction(*m_dev, pressed ? 1 : 0, code, accel_factor);

				/* click */
				if (ret == 1)
				{
					if (last_action != NULL)
						last_action->execAction(*m_dev, 0, 0, 0);
					last_action = &m_shortcuts[digest][i];
				}

				/* lock */
				if (ret == -1)
				{
					notifyLock(Shortcut::locked);
					if (!Shortcut::locked && last_action != NULL)
					{
						last_action->execAction(*m_dev, 0, 0, 0);
						last_action = NULL;
					}
				}
			}
		}
		else if (last_action && !Shortcut::locked)
		{
			last_action->execAction(*m_dev, 0, 0, 0);
			last_action = NULL;
		}
	}

	last_pressed = pressed;
}


void App::notifyLock(bool flag)
{
	if (m_notify == 0)
		return;
	
	if (m_notify == 1)
	{
		int i = 2;
		struct input_event ev;
		
		while (i > 0)
		{
			ev.code = LED_NUML;
			ev.value = 1;
			write(m_fd, &ev, sizeof(struct input_event));
			
			usleep(200000);
			
			ev.value = 0;
			write(m_fd, &ev, sizeof(struct input_event));
			
			i--;
		}
		
		return;
	}
	
	if (m_notify == 2)
	{
		pid_t pid = fork();
		
		if (pid == 0)
		{
			execvp(m_notifyProg, m_notifyProgArgs);
			
			Logger::Log(ERROR, "Error in execvp");
			
			_exit(-1);
		}
		else if (pid < 0)
			Logger::Log(ERROR, "Failed to fork() during lock notify.");
	}
}
