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

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <map>
#include <string>
#include <vector>

using namespace std;

/*! \brief Represents a single option in config file
 * \author Francesco Piccinno <stack.box@gmail.com>
 */
class Option
{
public:
	/*! \brief Default constructor
	 * \param name option's name
	 * \param value option's value
	 */
	Option(const char *name, const char *value) : m_name(name), m_value(value) {};

	/*! \brief Empty constructor for map */
	Option() : m_name(), m_value() {};

	/*! \brief Get the option's name */
	const string& getName() { return m_name; };
	
	/*! \brief Get the option's value */
	const string& getValue() { return m_value; };
private:
	string m_name, m_value;
};


/*! \brief Represents an object that can read options from file
 * \author Francesco Piccinno <stack.box@gmail.com>
 * \pre # this is a comment in config file
 * \pre option=value
 */
class ConfigFile
{
public:
	/*! \brief Default constructor
	 * \param file path to config file
	 */
	ConfigFile(const char *file);

	/*! \brief Default destructor */
	~ConfigFile();
	
	/*! \brief Returns the path to config file */
	const string& getFileName();

	/*! \brief Returns the value of 'id' option
	 * \param id the option's name you would know the value
	 */
	const string& getOption(const char *id);

	/*! \brief Returns the value of 'id' option as int (::atoi called)
	 * \param id the option's name you would know the value
	 * \param def default value for option
	 */
	int getOptionAsInt(const char *id, int def);

	/*! \brief Split the value of 'id' option
	 * if option test=1,2,3,4 then getOptionAndSplit(test, ",", vect)
	 * will return [1, 2, 3, 4] in vect
	 * \param id the option's name you would know the value
	 * \param delim the delimiter for value
	 * \tokens a std::vector in which results will saved
	 */
	void getOptionAndSplit(const char *id, const char *delim, vector<string>& tokens);

private:
	
	/*! \brief Add the option (name=id, value=val) in the hash map
	 * \param id the option's name
	 * \param val the option's value
	 */
	void setOption(const char *id, const char *val);

	/*! \brief Tokenize the string
	 * \param str the string to be tokenized
	 * \param tokens the vector in which tokens will placed
	 * \param delimiters delimiters for tokens
	 * \param len max number of token that can be placed in vector
	 */
	void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ", unsigned int len = 0);
	
	map <string, Option> m_values;	//! hash map for values
	string m_file;			//! path to config file
};

#endif
