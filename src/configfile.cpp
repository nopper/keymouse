/*
      Copyright (C) 2008 by Francesco Piccinno
      stack.box@gmail.com
      BSD License
*/

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "configfile.h"

ConfigFile::ConfigFile(const char *file) : m_values(), m_file(file)
{
	ifstream in(file);
	
	if (in.is_open())
	{
		string line;
		
		while (getline(in, line))
		{
			string::size_type i = line.find_first_not_of(" \t\n\v");
			
			if (i != string::npos && line[i] == '#')
				continue;
			
			vector<string> tokens;
			tokenize(line, tokens, "=", 2);
			
			if (tokens.size() == 2)
				setOption(tokens[0].c_str(), tokens[1].c_str());
		}
	}
}


ConfigFile::~ConfigFile()
{
	m_values.clear();
}


const string& ConfigFile::getOption(const char *id)
{
	return m_values[id].getValue();
}


const string& ConfigFile::getFileName()
{
	return m_file;
}


int ConfigFile::getOptionAsInt(const char *id, int def)
{
	const string s = getOption(id);
	
	for (int i = 0; i < s.length(); i++)
		if (!::isdigit(s.c_str()[i]))
			return def;
	
	return ::atoi(s.c_str());
}


void ConfigFile::getOptionAndSplit(const char *id, const char *delim, vector<string>& tokens)
{
	tokenize(getOption(id), tokens, delim, 0);
}

void ConfigFile::setOption(const char *id, const char *val)
{
	if (id == NULL || val == NULL || id == "" || val == "")
		return;
	
	Option tmp_opt(id, val);
	
	m_values[id] = tmp_opt;
}


void ConfigFile::tokenize(const string& str, vector<string>& tokens, const string& delimiters, unsigned int len)
{
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos     = str.find_first_of(delimiters, lastPos);
	
	while (string::npos != pos || string::npos != lastPos)
	{
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
		
		if (len == 0)
			continue;
		
		if (tokens.size () + 1 == len)
		{
			if (string::npos != lastPos)
				tokens.push_back(str.substr(lastPos));
			break;
		}
	}
}
