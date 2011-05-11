/******************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)

#include "tools/common/SGppStopwatch.hpp"

namespace sg
{
namespace base
{

SGppStopwatch::SGppStopwatch()
{
#ifdef WINDOWS
	QueryPerformanceFrequency(&ticksPerSecond);
#endif
#ifndef WINDOWS

#endif
}

SGppStopwatch::~SGppStopwatch()
{
}

void SGppStopwatch::start()
{
#ifdef WINDOWS
	QueryPerformanceCounter(&begin);
#endif
#ifndef WINDOWS
	gettimeofday(&begin,(struct timezone *)0);
#endif
}

double SGppStopwatch::stop()
{
#ifdef WINDOWS
	QueryPerformanceCounter(&end);

	double ret, ticksps;

	end.QuadPart -= begin.QuadPart;
	ret = (double)(end.QuadPart);
	ticksps = (double)(ticksPerSecond.QuadPart);
	ret /= ticksps;

	return ret;
#endif
#ifndef WINDOWS
	gettimeofday(&end,(struct timezone *)0);
	double seconds, useconds;
	double ret, tmp;

	if (end.tv_usec >= begin.tv_usec)
	{
		seconds = (double)end.tv_sec - (double)begin.tv_sec;
		useconds = (double)end.tv_usec - (double)begin.tv_usec;
	}
	else
	{
		seconds = (double)end.tv_sec - (double)begin.tv_sec;
		seconds -= 1;					// Correction
		useconds = (double)end.tv_usec - (double)begin.tv_usec;
		useconds += 1000000;			// Correction
	}

	// get time in seconds
	tmp = (double)useconds;
	ret = (double)seconds;
	tmp /= 1000000;
	ret += tmp;

	return ret;
#endif
}

}
}
