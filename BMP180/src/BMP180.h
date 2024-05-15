/*
BMP180.h - Header file for the BMP180/BMP180 Barometric Pressure & Temperature Sensor Arduino Library.

Version: 1.0.2
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
* Todo: update licensing.
*/

#ifndef BMP180_h
#define BMP180_h

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>

#define IIO_PATH_PREFACE	"/sys/bus/iio/devices/iio:device0"

typedef enum
{
    BMP180_ULTRA_LOW_POWER  	= 1,	// Oversampling 1 Conversion time 4.5 ms.
    BMP180_STANDARD         	= 2,	// Oversampling 2 Conversion time 7.5 ms
    BMP180_HIGH_RES         	= 4,	// Oversampling 4 Conversion time 13.5 ms
    BMP180_ULTRA_HIGH_RES   	= 8,	// Oversampling 8 Conversion time 25.5 ms. Standard on power-up with Linux driver.

    NUM_BMP_180_OSS_SETTINGS	= 4
    
} E_BMP180_OSS;

class BMP180
{
    public:
		BMP180(E_BMP180_OSS e = BMP180_ULTRA_HIGH_RES);
		~BMP180();
		
		const char *getName(void);

		void setOversampling(E_BMP180_OSS oss = BMP180_ULTRA_HIGH_RES);
		E_BMP180_OSS getOversampling(void);

		uint32_t readRawTemperature(void);
		double_t readTemperature(void);
		double_t readPressure(void);
		double_t readAltitude(void);

		double_t getPressure(void);
		double_t getAltitude(double_t p0 = SEALEVEL_PRESSURE_MILLIBARS);
		double_t getSeaLevel(double_t pressure, double_t altitude);

		void getOffsets(const uint32_t uN = NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_MINUTE, const double_t dPressure = DEFAULT_PRESSURE, const double_t dTemperature = DEFAULT_TEMPERATURE, const double_t dAltitude = DEFAULT_ALTITUDE );		
									
		inline bool calibrated(void)
		{
			return bCalibrated;
		}									

		static const double_t MILLIBARS_TO_PASCALS;
		
		static const double_t SEALEVEL_PRESSURE_MILLIBARS;

		static const double_t SEALEVEL_PRESSURE_PASCALS;

		static const uint32_t BMP180_SAMPLE_DELAY_US;
		static const uint32_t NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_SECOND;
		static const uint32_t NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_MINUTE;
		static const uint32_t NUMBER_OF_OFFSET_SAMPLES_FOR_FIVE_MINUTES;

		static const double_t DEFAULT_TEMPERATURE, DEFAULT_ALTITUDE, DEFAULT_PRESSURE;

	protected:
		E_BMP180_OSS eOSS;

		bool bCalibrated;

		double_t dBaselinePressure;

		double_t dBaselineAltitude;

    private:
    	FILE *pHandle;					// Used to access the "/sys/bus/iio/device0" bmp180 abstraction.
		
		char achDeviceName[100];
		
		static const bool bDebug;
		
		uint32_t in_temp_input;
		
		uint32_t in_pressure_oversampling_ratio;
		
		uint32_t in_temp_oversampling_ratio;
										// Never changes for BMP180. Does for BMP280.
		double_t in_pressure_input;
		
		static const double_t RAW_TEMPERATURE_CONVERSION;

		double_t raw_temp_offset;
		double_t raw_pressure_offset, raw_altitude_offset, set_altitude_value, set_pressure_value, set_temperature_value;
		
};

#endif
