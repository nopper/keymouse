bin_PROGRAMS = keymouse
keymouse_SOURCES = app.cpp configfile.cpp device.cpp keymouse.cpp logger.cpp \
	shortcut.cpp

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
keymouse_LDFLAGS = $(all_libraries) 
noinst_HEADERS = app.h configfile.h device.h logger.h shortcut.h
keymousedir = /etc
keymouse_DATA = keymouse.cfg.example
