/*
BNO055.h - Header file for the Bosch BNO055 Intelligent 9-axis absolute orientation sensor.
*
* Todo: update licensing
*
*/

#ifndef BNO055_H
#define BNO055_H

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>

#define IIO_PATH_PREFACE	"/sys/bus/iio/devices/iio:device1"

class BNO055
{
    public:
		BNO055();
		~BNO055();
		
		const char *getName(void);
		void getOffsets(const uint32_t uN = NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_MINUTE);		
									// It's implicit that we are zeroing the sensor.

		void readLinearAccelerations(double_t &x, double_t &y, double_t &z);
		void readAccelerations(double_t &x, double_t &y, double_t &z);
		void readGyroscope(double_t &x, double_t &y, double_t &z);
		// readGyroscope returns radians per second.

		void readGravities(double_t &x, double_t &y, double_t &z);
		void readCompass(double_t &x, double_t &y, double_t &z);
		void readQuaternions(double_t &w, double_t &x, double_t &y, double_t &z);
		void readOrientation(double_t &pitch, double_t &roll, double_t &yaw);

		inline bool calibrated(void)
		{
			return bCalibrated;
		}

		static const uint32_t BNO055_SAMPLE_DELAY_US;
		static const uint32_t NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_SECOND;
		static const uint32_t NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_MINUTE;
		static const uint32_t NUMBER_OF_OFFSET_SAMPLES_FOR_FIVE_MINUTES;

		static const double_t UPDATE_RATE;

	protected:

		bool bCalibrated;

    private:
    	FILE *pHandle;					// Used to access the "/sys/bus/iio/device0" BNO055 abstraction.
		
		char achDeviceName[100];
		
		static const bool bDebug;

		static const int32_t NUMBER_OF_SCALE_FACTORS;

		static const int32_t NUMBER_OF_ANGLES;

		static const int32_t NUMBER_OF_AXES;

		static const int32_t NUMBER_OF_QUATERNIONS;

		// Note: these match the names provided by the IIO driver. They are writable by root.
		double_t in_accel_scale,
			in_magn_scale,
			in_anglvel_scale,
			in_gravity_scale,
			in_rot_scale;

		// Note: these match the names provided by the IIO driver. They are readable.
		int32_t in_accel_linear_x_raw, in_accel_z_raw, in_magn_x_raw,
			in_accel_linear_y_raw, in_anglvel_x_raw, in_magn_y_raw,
			in_accel_linear_z_raw, in_anglvel_y_raw, in_magn_z_raw,
			in_anglvel_z_raw, in_rot_pitch_raw,
			in_gravity_x_raw, in_rot_quaternion_raw,
			in_accel_x_raw, in_gravity_y_raw, in_rot_roll_raw,
			in_accel_y_raw, in_gravity_z_raw, in_rot_yaw_raw;

		// Note: these match the names provided by the IIO driver. They are not writable. 
		//	The user has to set these class elements.
		//	Created made-up offsets for linear, gravity, quaternions, and orientation.
		int32_t in_accel_x_offset, in_anglvel_x_offset, in_magn_x_offset,
			in_accel_y_offset, in_anglvel_y_offset, in_magn_y_offset,
			in_accel_z_offset, in_anglvel_z_offset, in_magn_z_offset,
			in_linear_accel_x_offset, in_linear_accel_y_offset, in_linear_accel_z_offset,
			in_gravity_x_offset, in_gravity_y_offset, in_gravity_z_offset,
			in_quaternion_w_offset, in_quaternion_x_offset, in_quaternion_y_offset, in_quaternion_z_offset,
			in_orientation_roll_offset, in_orientation_pitch_offset, in_orientation_yaw_offset;

		void getScaleFactors(void);

		void readRawLinearAccelerations(int32_t &x, int32_t &y, int32_t &z);
		void readRawAccelerations(int32_t &x, int32_t &y, int32_t &z);
		void readRawGyroscopeValues(int32_t &x, int32_t &y, int32_t &z);
		void readRawGravityValues(int32_t &x, int32_t &y, int32_t &z);
		void readRawCompassAngles(int32_t &x, int32_t &y, int32_t &z);
		void readRawQuaternions(int32_t &w, int32_t &x, int32_t &y, int32_t &z);
		void readRawOrientation(int32_t &pitch, int32_t &roll, int32_t &yaw);

		void writeRawLinearAccelerationOffsets(int32_t &x, int32_t &y, int32_t &z);
		void writeRawAccelerationOffsets(int32_t &x, int32_t &y, int32_t &z);
		void writeRawGyroscopeOffsets(int32_t &x, int32_t &y, int32_t &z);
		void writeRawGravityOffsets(int32_t &x, int32_t &y, int32_t &z);
		void writeRawCompassOffsets(int32_t &x, int32_t &y, int32_t &z);
		void writeRawQuaternionOffsets(int32_t &w, int32_t &x, int32_t &y, int32_t &z);
		void writeRawOrientationOffsets(int32_t &pitch, int32_t &roll, int32_t &yaw);
		
};

#endif
