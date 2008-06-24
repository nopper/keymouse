/*
      Copyright (C) 2008 by Francesco Piccinno
      stack.box@gmail.com
      BSD License
*/

#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "device.h"


Device::Device(bool touchscreen, int touchscreen_max_x, int touchscreen_max_y) : m_valid(false), m_fd(0), m_abs_x(0), m_abs_y(0)
{
	m_absolute = touchscreen;
	m_max_x=touchscreen_max_x;
	m_max_y=touchscreen_max_y;
}


Device::~Device()
{
	if (m_valid)
	{
		ioctl(m_fd, UI_DEV_DESTROY);
		close(m_fd);

		Logger::Log(INFO, "Device destroyed.");
	}
	m_valid = false;
}


bool Device::allocate(const char *dev_str)
{
	struct uinput_user_dev dev;
	
	m_fd = open(dev_str, O_WRONLY | O_NDELAY);
	
	if (m_fd <= 0)
		return false;
	
	m_valid = true;

	memset(&dev, 0, sizeof(dev));
	if(m_absolute) 
	{
		strcpy(dev.name, "Keyboard Touchpad Emulator");
		dev.absmax[ABS_X]=m_max_x;
		dev.absmax[ABS_Y]=m_max_y;
		dev.absmax[ABS_PRESSURE]=400;
		write(m_fd, &dev, sizeof(dev));
		ioctl(m_fd, UI_SET_EVBIT, EV_ABS);
		ioctl(m_fd, UI_SET_ABSBIT, ABS_X);
		ioctl(m_fd, UI_SET_ABSBIT, ABS_Y);
		// a bit overhead that we report both BTN_TOUCH and ABS_PRESSURE  for detecting 
		// that user is touching the pad. But it is better for interoperability.
		ioctl(m_fd, UI_SET_ABSBIT, ABS_PRESSURE);
		ioctl(m_fd, UI_SET_KEYBIT, BTN_TOUCH);
	} 
	else 
	{
		strcpy(dev.name, "Keyboard Mouse Emulator");
		write(m_fd, &dev, sizeof(dev));
		ioctl(m_fd, UI_SET_EVBIT, EV_REL);
		ioctl(m_fd, UI_SET_RELBIT, REL_X);
		ioctl(m_fd, UI_SET_RELBIT, REL_Y);
	}
	
	ioctl(m_fd, UI_SET_EVBIT, EV_KEY);
	ioctl(m_fd, UI_SET_KEYBIT, BTN_MOUSE);
	ioctl(m_fd, UI_SET_KEYBIT, BTN_LEFT);
	ioctl(m_fd, UI_SET_KEYBIT, BTN_RIGHT);
	ioctl(m_fd, UI_SET_KEYBIT, BTN_MIDDLE);
	ioctl(m_fd, UI_DEV_CREATE, 0);
	
	return true;
}


void Device::moveTo(int rel_x, int rel_y, int pressed)
{
	if (!m_valid)
		return;
	
	if(m_absolute) 	//! touchscreen (absolute coordinates)
	{ 	
		Logger::Log(DEBUG, "Moving cursor absolute x: %d y: %d press: %d", rel_x, rel_y, pressed);
	} 
	else 		//! mouse (relative moves)
	{		
		Logger::Log(DEBUG, "Moving cursor relative x: %d y: %d", rel_x, rel_y);
	}
	struct input_event event;

	gettimeofday(&event.time, NULL);

	if(m_absolute) 	//! touchscreen
	{ 	
		event.type = EV_ABS;
		event.code = ABS_X;
		event.value = rel_x;
		write(m_fd, &event, sizeof(event));

		event.type = EV_ABS;
		event.code = ABS_Y;
		event.value = rel_y;
		write(m_fd, &event, sizeof(event));

		event.type = EV_ABS;
		event.code = ABS_PRESSURE;
		if(pressed) event.value = 100; else event.value = 0;
		write(m_fd, &event, sizeof(event));

		event.type = EV_KEY;
		event.code = BTN_TOUCH;
		if(pressed) event.value = 1; else event.value = 0;
		write(m_fd, &event, sizeof(event));
	} 
	else 		//! mouse
	{ 
		event.type = EV_REL;
		event.code = REL_X;
		event.value = rel_x;
		write(m_fd, &event, sizeof(event));

		event.type = EV_REL;
		event.code = REL_Y;
		event.value = rel_y;
		write(m_fd, &event, sizeof(event));
	}
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(m_fd, &event, sizeof(event));
}


bool Device::sendClick(int btn, int pressed)
{
	if (!m_valid)
		return false;
	
	if (btn == BTN_RIGHT)
		Logger::Log(DEBUG, "sendClick(BTN_RIGHT) %s",
			((pressed) ? " PRESSED" : " RELEASED"));
	else if (btn == BTN_LEFT)
		Logger::Log(DEBUG, "sendClick(BTN_LEFT) %s",
			((pressed) ? " PRESSED" : " RELEASED"));
	else if (btn == BTN_MIDDLE)
		Logger::Log(DEBUG, "sendClick(BTN_MIDDLE) %s",
			((pressed) ? " PRESSED" : " RELEASED"));
	
	struct input_event event;
	
	gettimeofday(&event.time, NULL);
	event.type = EV_KEY;
	event.code = btn;
	event.value = pressed;
	write(m_fd, &event, sizeof(event));
	
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(m_fd, &event, sizeof(event));

	return true;
}
