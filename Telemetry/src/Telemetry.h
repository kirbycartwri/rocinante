/*
	Telemetry.h - Base Telemetry class for Raspberry PI Version 1.
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
#ifndef _TELEMETRY_H
#define _TELEMETRY_H

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>

typedef enum telItemNumber
{

    NUMBER_OF_TELEMETRY_ITEMS = 100
} E_TELEMETRY_ITEM_NUMBER;

typedef struct sTelItem
{
    char name[100];
    char units[100];
    int32_t itemNumber;
    double_t dValue;
    sig_atomic_t bNew, bWritten;
    sTelItem(void)
    {
        (void)memset(name, '\0', sizeof(name));
        (void)memset(units, '\0', sizeof(units));
        itemNumber  = 0;
        dValue      = 0.0;
        bNew        = 0;
        bWritten    = 0;
    }

} telemetryDatum;

class Telemetry
{
public:
    Telemetry(void);
    virtual ~Telemetry();
    void startTelemetry(void);
    void stopTelemetry(void);
    bool getTelemetry(void);    
    static const int32_t TELEMETRY_BUFFER_SIZE;
    static const int32_t NUM_TELEMETRY_DATA;
    static const double_t DEFAULT_UPDATE_PERIOD;
    static const double_t INITIAL_DELAY_PERIOD;
    static const char DELIMITER;

    void writeItemValue(const double_t &dValue, int32_t &itemNumber);
    void writeItemValueHeader(const char *name, const char *units, int32_t &telItemNumber);
    void readItemValue(double_t &dValue, int32_t &itemNumber);

    virtual void update(void);

protected:
    bool bStartStop;
    char *outBuffer;
    telemetryDatum *data;
    double_t updatePeriod;
    sig_atomic_t bHeaderWritten;
    struct timespec thisTime, lastTime;
    int32_t nTicks;
    int32_t nItems;

    static const bool bDebug;

    virtual void writeBuffer(void);

private:

};



#endif  // _TELEMETRY_H