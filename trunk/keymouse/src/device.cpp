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


Device::Device() : m_valid(false), m_fd(0)
{
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
	strcpy(dev.name, "Keyboard Mouse Emulator");
	write(m_fd, &dev, sizeof(dev));
	
	ioctl(m_fd, UI_SET_EVBIT, EV_REL);
	ioctl(m_fd, UI_SET_RELBIT, REL_X);
	ioctl(m_fd, UI_SET_RELBIT, REL_Y);
	ioctl(m_fd, UI_SET_EVBIT, EV_KEY);
	ioctl(m_fd, UI_SET_KEYBIT, BTN_MOUSE);
	ioctl(m_fd, UI_SET_KEYBIT, BTN_LEFT);
	ioctl(m_fd, UI_SET_KEYBIT, BTN_RIGHT);
	ioctl(m_fd, UI_SET_KEYBIT, BTN_MIDDLE);

	ioctl(m_fd, UI_DEV_CREATE, 0);
	
	return true;
}


void Device::moveTo(int rel_x, int rel_y)
{
	if (!m_valid)
		return;
	
	Logger::Log(DEBUG, "Moving cursor x: %d y: %d", rel_x, rel_y);
	
	struct input_event event;

	gettimeofday(&event.time, NULL);
	event.type = EV_REL;
	event.code = REL_X;
	event.value = rel_x;
	write(m_fd, &event, sizeof(event));

	event.type = EV_REL;
	event.code = REL_Y;
	event.value = rel_y;
	write(m_fd, &event, sizeof(event));

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
