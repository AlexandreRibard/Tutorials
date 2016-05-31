#ifndef _STOPWATCH_HPP_
#define _STOPWATCH_HPP_



////////////////////////
// STL
////////////////////////
#include <iostream> // cout
#include <chrono> // C++11 timer

namespace OpenGLEngine
{


/**
* \file stopWatch.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @addtogroup UTILITIES */
/*@{*/


/*!
*  \brief Timer Wrapper: \n
*		Implements a timer using C++ <chrono> \n
*/
/*!
*	How to use: \n
*		\code{.cpp}
*				stopWatch timer(); // declare a timer and starts ticking
*				timer.start(); // resets timer and starts ticking
*					...
*					...
*				timer.end(); // stops timer
*				double time_span = timer.time(); // returns end - start time span
*				std::cout << &t << std::endl; // displays end - start
*		\endcode
*	
*	If one wants increment the timer as to mesure a given amout of laps\n
*		\code{.cpp}
*				timer.lap(); // => time_span += current_time - start
*		\endcode
*
*/
class stopWatch
{
public:
	///////////////////////////////////////////
	//	CONSTUCTOR & DESTRUCTOR
	///////////////////////////////////////////
	/*!
	*  \brief Default Constructor: \n
	*		starts tick at current time
	*
	* \return intializes the timer and set internal start time at current call time
	* \note Might differ from one or two clock ticks due to class creation overhead
	*/
	stopWatch() {
		startTime = std::chrono::high_resolution_clock::now();
	};

	///////////////////////////////////////////
	//	UTILITY
	///////////////////////////////////////////
	/*!
	*  \brief Resets timer start time
	* \return sets start time at current call time
	*/
	void start() {
		startTime = std::chrono::high_resolution_clock::now();
	}
	/*!
	*  \brief Compute elapsed time span form now to start time
	* \return return a double representing elapsed time from current time to start time (finish - start)
	*/
	double end() {
		finishTime = std::chrono::high_resolution_clock::now();
		time_span = std::chrono::duration_cast< std::chrono::duration<double> >(finishTime - startTime);
		return time_span.count();
	}
	/*!
	*  \brief Compute elapsed time span form now to start time and increments current elapsed time
	* \return return a double representing incremented time span
	*/
	double lap() {
		finishTime = std::chrono::high_resolution_clock::now();
		time_span += std::chrono::duration_cast< std::chrono::duration<double> >(finishTime - startTime);
		return time_span.count();
	}
	/*!
	*  \brief Casts into double current elapse time span (end - start)
	* \return return a double representing current time span
	*/
	double time() {
		return time_span.count();
	}
private:
	////////////////////
	//  StopWatch Data
	////////////////////
	//! Start time
	/*! high resolution clock saving start time
	*/
	std::chrono::high_resolution_clock::time_point startTime;
	//! Finished time
	/*! high resolution clock saving finished time
	*/
	std::chrono::high_resolution_clock::time_point finishTime;
	//! Duration span
	/*! Time duration from start to finish
	*/
	std::chrono::duration<double> time_span;
};


/*!
*  \brief << opperator overload
* \return displays current timer duration span
*/
std::ostream& operator<< (std::ostream &os, stopWatch * const timer)
{
	os << timer->time();
	return os;
}

/*@}*/


}

#endif // _STOPWATCH_HPP_