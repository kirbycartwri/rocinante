/*
	Telemetry.cpp - Base Telemetry class for Raspberry PI Version 1.
	Copyright (c) 2024 Kirby W. Cartwright. All right reserved.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include "Telemetry.h"

const int32_t Telemetry::TELEMETRY_BUFFER_SIZE 	= 1024;
const char Telemetry::DELIMITER 				= ',';
const int32_t Telemetry::NUM_TELEMETRY_DATA 	= 100;
const double_t Telemetry::INITIAL_DELAY_PERIOD	= 1.00;	
const double_t Telemetry::DEFAULT_UPDATE_PERIOD	= 0.100;

const bool Telemetry::bDebug					= true;

Telemetry::Telemetry(void) :
    bStartStop(false), outBuffer(NULL), data(NULL), nTicks(0), nItems(0)
{
    outBuffer = new char[TELEMETRY_BUFFER_SIZE];
	(void)memset(outBuffer, '\0', TELEMETRY_BUFFER_SIZE);

    data = new telemetryDatum[NUM_TELEMETRY_DATA];

	for (int32_t i=0 ; i< NUM_TELEMETRY_DATA ; i++ )
	{
		data[i].bWritten 	= true;
		data[i].bNew		= false;
	}

	(void)clock_gettime(CLOCK_REALTIME, &thisTime);
	(void)clock_gettime(CLOCK_REALTIME, &lastTime);	

}
Telemetry::~Telemetry()
{
    delete data;
    data = NULL;
    delete outBuffer;
    outBuffer = NULL;
}

void Telemetry::startTelemetry(void)
{
	nTicks		= 0;
	bStartStop 	= true;
}

void Telemetry::stopTelemetry(void)
{
	nTicks		= 0;
	bStartStop 	= false;
}

bool Telemetry::getTelemetry(void)
{
	return bStartStop;
}

void Telemetry::writeItemValueHeader(const char *name, const char *units, int32_t &telItemNumber)
{

	if ( telItemNumber < 0 || telItemNumber >= NUM_TELEMETRY_DATA )
	{
		return;
	}

	if ( NULL!=name )
		(void)strncpy(data[telItemNumber].name, name, sizeof(data[telItemNumber].name));
	
	if ( NULL!=units)
		(void)strncpy(data[telItemNumber].units, units, sizeof(data[telItemNumber].units));

	nItems++;

}

void Telemetry::writeItemValue(const double_t &dValue, int32_t &itemNumber)
{
	if ( !data[itemNumber].bNew && data[itemNumber].bWritten )
	{
		data[itemNumber].dValue = dValue;
		data[itemNumber].bNew = 1;
	}

}

void Telemetry::update(void)
{
	(void)clock_gettime(CLOCK_REALTIME, &thisTime);

	double_t deltaT = 0.0;

	time_t tv_delta_sec = ( thisTime.tv_sec -  lastTime.tv_sec  );
	long tv_delta_ns	= ( thisTime.tv_nsec - lastTime.tv_nsec );

	deltaT = tv_delta_ns;
	deltaT /= 1e9;
	deltaT += tv_delta_sec;

	if ( !bStartStop || ( ( 0 == nTicks ) && ( deltaT < INITIAL_DELAY_PERIOD ) ) ) 
	{
		// (void)printf("One\n");
		return;
	}

	else if ( 0 == nTicks )
	{
		//(void)printf("Two\n");		
		for (int32_t i=0 ; i<nItems ; i++ )
		{
			(void)sprintf(&outBuffer[strlen(outBuffer)], "%s(%s),", data[i].name, data[i].units);
		}
		lastTime = thisTime;
		writeBuffer();		
		nTicks++;
	}

	else if ( deltaT < DEFAULT_UPDATE_PERIOD )
	{
		// (void)printf("Three\n");
		return;
	}

	else
	{
		// (void)printf("Four\n");		
		for (int32_t i=0 ; ( i<nItems ) && (data[i].bNew); i++ )
		{
			(void)sprintf(&outBuffer[strlen(outBuffer)], "%lf,", data[i].dValue);
			data[i].bNew 		= 0;
			data[i].bWritten 	= 0;
		}
		writeBuffer();
		lastTime = thisTime;
		nTicks++;
		// (void)printf("Ticks: %d\n", nTicks);
	}

}

void Telemetry::writeBuffer(void)
{	
	(void)puts(outBuffer); 				// includes "... a trailing newline ..."
	(void)fflush(stdout);
	for (int32_t i=0 ; ( i<nItems ) && (!data[i].bWritten); i++ )
	{
		data[i].bWritten = 1;
	}
	(void)memset(outBuffer, '\0', TELEMETRY_BUFFER_SIZE);

}