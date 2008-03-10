/*
      Copyright (C) 2008 by Francesco Piccinno
      stack.box@gmail.com
      BSD License
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <signal.h>
#include "app.h"

using namespace std;

App *m_app;

void sig_handler(int signum)
{
	switch (signum)
	{
		case SIGINT:
				Logger::Log(INFO, "Exiting ..");
				m_app->stop(true);
			break;
		case SIGHUP:
				m_app->stop(false);
				
				App *clone = new App(*m_app);
				delete m_app;
				
				m_app = clone;
				m_app->run();
			break;
	}
}

int main(int argc, char *argv[])
{
	Logger::setType(INFO);
	
	Logger::Log(INFO, "Starting key-mouse by stack");
	Logger::Log(INFO, "BSD license - (C) 2008 by Francesco Piccinno");
	Logger::Log(INFO, "For bugs: <stack.box@gmail.com>");
	Logger::Log(INFO, "Happy mousing :)");
	
	if (argc != 3)
	{
		Logger::Log(ERROR, "Usage: %s <config-file> <logint>", argv[0]);
		Logger::Log(ERROR, " <config-file> is a path to keymouse.cfg (/etc/keymouse.cfg)");
		Logger::Log(ERROR, " <logint>      is an integer 1:errors 2:warnings 3:information 4:debug (3 for default)");
		return -1;
	}
	
	int type = ::atoi(argv[2]) - 1;
	
	if (type > 0 && type <= DEBUG)
		Logger::setType(static_cast <LogType> (type));
	
	signal(SIGHUP, sig_handler);
	signal(SIGINT, sig_handler);

	m_app = new App(argv[1]);
	m_app->run();

	delete m_app;
	
	return 0;
}
