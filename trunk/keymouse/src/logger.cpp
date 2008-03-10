/*
      Copyright (C) 2008 by Francesco Piccinno
      stack.box@gmail.com
      BSD License
*/

#include "logger.h"
#include <iostream>

using namespace std;

LogType Logger::m_type = INFO;


void Logger::setType(LogType type)
{
	m_type = type;
}


void Logger::Log(LogType type, const char *fmt, ...)
{
	if (type > m_type)
		return;
	
	char buf[1024];
	va_list args;
	
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	
	switch (type)
	{
		case INFO: cout << "(II) "; break;
		case WARN: cout << "(WW) "; break;
		case DEBUG: cout << "(DD) "; break;
		case ERROR: cout << "(EE) "; break;
	}
	
	cout << buf << endl;
}
