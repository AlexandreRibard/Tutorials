#ifndef _PARSER_HPP_
#define _PARSER_HPP_



////////////////////////
// STL
////////////////////////
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

namespace OpenGLEngine
{

/**
* \file parser.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @defgroup UTILITIES
*
* This module offers a number of utility classes or wrapper
*
*/

/** @addtogroup UTILITIES */
/*@{*/


/*!
*  \brief File Parser: \n
*		Utility function to efficiently parse a mesh file and recover geometric data \n
*
*	Utility functions to read the file chunk by chunk \n
*	
*	\note Current version works and is faster that naive C++ stream implementation, but remains unstable
*/
namespace parser
{
	/*!
	*  \brief readWord:
	*		Starts from a given buffer index. Parses the buffer until a word is found.
	*		Returns said word as a string as well as the resulting index of end in the input buffer
	*		Ex:
	*				 0...i......i_m........
	*		buffer = [... my_word an_other_word ....]
	*		index = first char of the word's index in the buffer
	*
	*		We start from the starting index and parse the buffer char by char [i] ---> [i_m] until reaching the word's end
	*		=> the word ends by default by a whitespace (' '), but can end at eny given char
	*
	*		Note: if the itteration makes up overflow the buffer, we read another chunk of text from the stream
	*
	* \param std::vector<char> * buffer : char buffer : chunk of input file
	* \param size_t startIndex : starting index of word
	* \param std::ifstream * readStream : input stream
	* \param char separator : char denoting end of word
	* \return std::pair<std::string, size_t> : said word as a std::string and index + 1 of last char.
	*/
	std::pair< std::string, size_t> readWord(std::vector<char> * buffer, size_t startIndex, std::ifstream * readStream, char separator = ' ');

	/*!
	*  \brief readAndSplitWord:
	*		Same as readWord, but splits the word into multiple sub-words  acording to a given separator in one pass
	*					 ... i......... .i_m ...
	*		=> buffer = [... aaa//bbb///cc ...]
	*		we want to extract the word and split it acording to a given separator
	*			aaa,bbb,cc
	*
	*		Parses the word but skips separator and fills up a vector of string of corresponding sub words
	*
	*		Note: if the itteration makes up overflow the buffer, we read another chunk of text from the stream
	*
	* \param std::vector<char> * buffer : char buffer : chunk of input file
	* \param size_t startIndex : starting index of word
	* \param std::ifstream * readStream : input stream
	* \param char separator : char denoting spliting char
	* \param std::vector<std::string> * words corresponding split words
	* \return size_t : index + 1 of last char.
	*/
	size_t readAndSplitWord(std::vector<char> * buffer, size_t startIndex, std::ifstream * readStream, char separator, std::vector<std::string> * words);




	/*!
	*  \brief readLine:
	*		Starts from a given buffer index. Parses the buffer until an end of line if found.
	*		Returns said line as a string as well as the resulting index of end in the input buffer
	*		Ex:
	*				 0... i..........i_m ...
	*		buffer = [... blalblabla\n ....]
	*		index = first char of the line's index in the buffer
	*
	*		We start from the starting index and parse the buffer char by char [i] ---> [i_m] until reaching an end of line symbol '\n'
	*
	*		Note: if the itteration makes up overflow the buffer, we read another chunk of text from the stream
	*
	* \param std::vector<char> * buffer : char buffer : chunk of input file
	* \param size_t startIndex : starting index of the line
	* \param std::ifstream * readStream : input stream
	* \return std::pair<std::string, size_t> : said line as a std::string and index + 1 of last char.
	*/
	std::pair< std::string, size_t> readLine(std::vector<char> * buffer, size_t startIndex, std::ifstream * readStream);

	/*!
	*  \brief readAndSplitLine:
	*		Same as readAndSplitWord, but splits an entier line into multiple sub-words  acording to a given separator in one pass
	*
	*		Parses a given line but skips separator and fills up a vector of string of corresponding sub words
	*
	*		Note: if the itteration makes up overflow the buffer, we read another chunk of text from the stream
	*
	* \param std::vector<char> * buffer : char buffer : chunk of input file
	* \param size_t startIndex : starting index of line
	* \param std::ifstream * readStream : input stream
	* \param char separator : char denoting spliting char
	* \param std::vector<std::string> * words corresponding split words
	* \return size_t : index + 1 of last char.
	*/
	size_t readAndSplitLine(std::vector<char> * buffer, size_t startIndex, std::ifstream * readStream, char separator, std::vector<std::string> * words);


	/*!
	*  \brief skipComment:
	*		Reads the file, (while using the buffer) and skip comments
	*
	*		If a line starts by a # or a given comment identifier, we skip the line.
	*		We loop the process if the next line is once again a comment
	*
	*		Note: if the itteration makes up overflow the buffer, we read another chunk of text from the stream
	*
	* \param std::vector<char> * buffer : char buffer : chunk of input file
	* \param size_t startIndex : starting index of line
	* \param std::ifstream * readStream : input stream
	* \param char comment : char denoting comment indentification char
	* \return size_t : index + 1 of last char.
	*/
	size_t skipComment(std::vector<char> * buffer, size_t startIndex, std::ifstream * readStream, char comment);
}

/*@}*/

}

#endif // _PARSER_HPP_