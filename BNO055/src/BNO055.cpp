/*
* BNO055.cpp - Class file for the Bosch BNO055 Intelligent 9-axis absolute orientation sensor.
*
* 
* Todo: update licensing.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "BNO055.h"

const bool BNO055::bDebug = false;

const double_t BNO055::UPDATE_RATE = 100.0;

const char *BNO055::getName(void)
{
	// Note: the same path as for the BMP180.
	const char *pName =
	{
		IIO_PATH_PREFACE "/name"
	};

	if ( bDebug )
		(void)printf("\nTrying to open \"%s.\"\n", pName);

	pHandle = fopen(pName,"r");
	
	if ( NULL == pHandle )
	{	
		(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pName, strerror(errno));	
		return (const char *)achDeviceName;		
	}

	else if ( bDebug )
		(void)printf("Successfully opened \"%s.\"\n", pName);	
	else
		;
	
	(void)memset(achDeviceName, '\0', sizeof(achDeviceName));
	if ( 1 != fscanf(pHandle, "%s", achDeviceName) )
	{
		(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pName, strerror(errno));
	}
	else if ( bDebug )
	{
		(void)printf("The name reported from \"%s\" is \"%s.\"\n", pName, achDeviceName);
	}
	else
		;

	if ( NULL!=pHandle )
		(void)fclose(pHandle);
	pHandle = NULL;
 	
	return (const char *)achDeviceName;
};

const int32_t BNO055::NUMBER_OF_SCALE_FACTORS = 5;


void BNO055::getScaleFactors(void)
{
	const char *pScales[NUMBER_OF_SCALE_FACTORS] =
	{
		{ IIO_PATH_PREFACE "/in_accel_scale"  	},
		{ IIO_PATH_PREFACE "/in_magn_scale"   	},
		{ IIO_PATH_PREFACE "/in_anglvel_scale"	},
		{ IIO_PATH_PREFACE "/in_gravity_scale"	},
		{ IIO_PATH_PREFACE "/in_rot_scale"    	}
	};

	double_t *pScaleFactors[NUMBER_OF_SCALE_FACTORS] =
	{
		{ &this->in_accel_scale   },
		{ &this->in_magn_scale    },
		{ &this->in_anglvel_scale },
		{ &this->in_gravity_scale },
		{ &this->in_rot_scale 	  }
	};

	for ( int32_t i = 0 ; i < NUMBER_OF_SCALE_FACTORS ; i++ )
	{
		if ( bDebug )
			(void)printf("\nTrying to open \"%s.\"\n", pScales[i]);

		pHandle = fopen(pScales[i],"r");
		
		if ( NULL == pHandle )
		{	
			(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pScales[i], strerror(errno));	
			break;		
		}

		else if ( bDebug )
			(void)printf("Successfully opened \"%s.\"\n", pScales[i]);	
		else
			;
		
		*pScaleFactors[i] = 0.0;
		
		if ( 1 != fscanf(pHandle, "%lf", pScaleFactors[i]) )
		{
			(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pScales[i], strerror(errno));
		}
		else if ( bDebug )
		{
			(void)printf("The scale factor reported from \"%s\" is \"%lf.\"\n", pScales[i], *pScaleFactors[i]);
		}
		else
			;

		if ( NULL!=pHandle )
			(void)fclose(pHandle);
		pHandle = NULL;	
	}
	return;
}

BNO055::BNO055() :
	bCalibrated(false), pHandle(NULL),
	in_accel_scale(0.0), in_magn_scale(0.0), in_anglvel_scale(0.0), 
	in_gravity_scale(0.0), in_rot_scale(0.0), 
	
	in_accel_z_raw(0), in_magn_x_raw(0),
	in_accel_linear_y_raw(0), in_anglvel_x_raw(0), in_magn_y_raw(0),
	in_accel_linear_z_raw(0), in_anglvel_y_raw(0), in_magn_z_raw(0),
	in_anglvel_z_raw(0), in_rot_pitch_raw(0),
	in_gravity_x_raw(0), in_rot_quaternion_raw(0),
	in_accel_x_raw(0), in_gravity_y_raw(0), in_rot_roll_raw(0),
	in_accel_y_raw(0), in_gravity_z_raw(0), in_rot_yaw_raw(0),

	in_accel_x_offset(0), in_anglvel_x_offset(0), in_magn_x_offset(0),
	in_accel_y_offset(0), in_anglvel_y_offset(0), in_magn_y_offset(0),
	in_accel_z_offset(0), in_anglvel_z_offset(0), in_magn_z_offset(0),

	in_linear_accel_x_offset(0), in_linear_accel_y_offset(0), in_linear_accel_z_offset(0),
	in_gravity_x_offset(0), in_gravity_y_offset(0), in_gravity_z_offset(0),
	in_quaternion_w_offset(0), in_quaternion_x_offset(0), in_quaternion_y_offset(0), in_quaternion_z_offset(0),
	in_orientation_roll_offset(0), in_orientation_pitch_offset(0), in_orientation_yaw_offset(0)

{
	// 03/11/2024: I don't know why the sensor stops producing output and this fixes it.
    (void)system("sudo modprobe -r bno055_i2c");
    (void)system("sudo modprobe bno055_i2c");


	(void)memset(achDeviceName, '\0', sizeof(achDeviceName));

	(void)getName();
	getScaleFactors();
	getOffsets(NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_SECOND);
}

BNO055::~BNO055() 
{	
	if ( NULL!=pHandle )
		(void)fclose(pHandle);
	pHandle = NULL;
	bCalibrated = false;
}

const int32_t BNO055::NUMBER_OF_ANGLES		=3;

const int32_t BNO055::NUMBER_OF_AXES		= 3;

const int32_t BNO055::NUMBER_OF_QUATERNIONS	= 4;

void BNO055::readGyroscope(double_t &x, double_t &y, double_t &z)
{
	int32_t ix=0, iy=0, iz=0;

	double_t * prXYZ[NUMBER_OF_ANGLES] =
	{
		&x, &y, &z
	};

	double_t *pXYZ[NUMBER_OF_ANGLES] =
	{
		(double_t *)&ix, (double_t *)&iy, (double_t *)&iz
	};

	int32_t *prXYZOffsets[NUMBER_OF_ANGLES] =
	{
		&in_anglvel_x_offset, &in_anglvel_y_offset, &in_anglvel_z_offset
	};

	readRawGyroscopeValues(ix, iy, iz);

	for ( int32_t i = 0 ; i < NUMBER_OF_ANGLES ; i++ )
	{
		*prXYZ[i] = in_anglvel_scale * (double_t)( *pXYZ[i] - *prXYZOffsets[i] );
	}	

	if ( bDebug )
	{
		(void)printf("The Gyroscope values (x,y,z) are %lf, %lf, %lf radians/second.\n", x, y, z);
	}	
}

// 100 Hz.
const uint32_t BNO055::BNO055_SAMPLE_DELAY_US						= 10000;

const uint32_t BNO055::NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_SECOND 		= ( 100 );

const uint32_t BNO055::NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_MINUTE 		= ( 60 * NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_SECOND );

const uint32_t BNO055::NUMBER_OF_OFFSET_SAMPLES_FOR_FIVE_MINUTES	= ( 5 * NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_MINUTE);

// Todo: review gain and offset calculations for references.


#define SIGNED_ROUND(x) if(x<0)x-=(uN/2);else x+=(uN/2);

void BNO055::getOffsets(const uint32_t uN /*= NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_MINUTE*/)
{
	bCalibrated = false;

	int64_t x_sum = 0, y_sum = 0, z_sum = 0,
		xx_sum = 0, yy_sum = 0, zz_sum = 0,
		xxx_sum = 0, yyy_sum = 0, zzz_sum = 0,
		xxxx_sum = 0, yyyy_sum = 0, zzzz_sum = 0,
		xxxxx_sum = 0, yyyyy_sum = 0, zzzzz_sum = 0,
		qw_sum = 0, qx_sum = 0, qy_sum = 0, qz_sum = 0,
		pitch_sum = 0, roll_sum = 0, yaw_sum = 0;

	int32_t ix = 0, iy = 0, iz = 0,
		iqw = 0, iqx = 0, iqy = 0, iqz =0,
		ip = 0, ir =0;

	for ( uint32_t i = 0 ; i < uN ; i++ )
	{
		int32_t x = 0, y = 0, z = 0, 
			xx = 0, yy = 0, zz = 0, 
			xxx = 0, yyy = 0, zzz = 0,
			xxxx = 0, yyyy = 0, zzzz = 0,
			xxxxx = 0, yyyyy = 0, zzzzz = 0,
			qw = 0, qx = 0, qy = 0, qz = 0,
			pitch = 0, roll = 0, yaw = 0;
		readRawGyroscopeValues(x, y, z);
		readRawAccelerations(xx, yy, zz);
		readRawLinearAccelerations(xxx, yyy, zzz);
		readRawGravityValues(xxxx, yyyy, zzzz);
		readRawCompassAngles(xxxxx, yyyyy, zzzzz);
		readRawQuaternions(qw, qx, qy, qz);
		readRawOrientation(pitch, roll, yaw);
		pitch_sum += pitch, roll_sum += roll, yaw_sum += yaw;
		qw_sum += qw, qx_sum += qx, qy_sum += qy, qz_sum += qz;
		xxxxx_sum += xxxxx, yyyyy_sum += yyyyy, zzzzz_sum += zzzzz;
		xxxx_sum += xxxx, yyyy_sum += yyyy, zzzz_sum += zzzz;
		xxx_sum += xxx, yyy_sum += yyy, zzz_sum += zzz;
		xx_sum += xx, yy_sum += yy, zz_sum += zz;
		x_sum += x, y_sum += y, z_sum += z;
		(void)usleep(BNO055_SAMPLE_DELAY_US);
	}

	SIGNED_ROUND(x_sum)
	x_sum /= (int64_t)uN;

	SIGNED_ROUND(y_sum)
	y_sum /= (int64_t)uN;

	SIGNED_ROUND(z_sum)
	z_sum /= (int64_t)uN;

	ix = x_sum, iy = y_sum, iz = z_sum;

	writeRawGyroscopeOffsets(ix, iy, iz);

	SIGNED_ROUND(xx_sum)
	xx_sum /= (int64_t)uN;

	SIGNED_ROUND(yy_sum)
	yy_sum /= (int64_t)uN;

	SIGNED_ROUND(zz_sum)
	zz_sum /= (int64_t)uN;

	ix = xx_sum, iy = yy_sum, iz = zz_sum;

	writeRawAccelerationOffsets(ix, iy, iz);

	SIGNED_ROUND(xxx_sum)
	xxx_sum /= (int64_t)uN;

	SIGNED_ROUND(yyy_sum)
	yyy_sum /= (int64_t)uN;

	SIGNED_ROUND(zzz_sum)
	zzz_sum /= (int64_t)uN;

	ix = xxx_sum, iy = yyy_sum, iz = zzz_sum;

	writeRawLinearAccelerationOffsets(ix, iy, iz);

	SIGNED_ROUND(xxxx_sum)
	xxxx_sum /= (int64_t)uN;

	SIGNED_ROUND(yyyy_sum)
	yyyy_sum /= (int64_t)uN;

	SIGNED_ROUND(zzzz_sum)
	zzzz_sum /= (int64_t)uN;

	ix = xxxx_sum, iy = yyyy_sum, iz = zzzz_sum;

	writeRawGravityOffsets(ix, iy, iz);

	SIGNED_ROUND(xxxxx_sum)
	xxxxx_sum /= (int64_t)uN;

	SIGNED_ROUND(yyyyy_sum)
	yyyyy_sum /= (int64_t)uN;

	SIGNED_ROUND(zzzzz_sum)
	zzzzz_sum /= (int64_t)uN;

	ix = xxxxx_sum, iy = yyyyy_sum, iz = zzzzz_sum;

	writeRawCompassOffsets(ix, iy, iz);

	SIGNED_ROUND(qw_sum)
	qw_sum /= (int64_t)uN;

	SIGNED_ROUND(qx_sum)
	qx_sum /= (int64_t)uN;

	SIGNED_ROUND(qy_sum)
	qy_sum /= (int64_t)uN;

	SIGNED_ROUND(qz_sum)
	qz_sum /= (int64_t)uN;

	iqw = qw_sum, iqx = qx_sum, iqy = qy_sum, iqz = qz_sum; 

	writeRawQuaternionOffsets(iqw, iqx, iqy, iqz);

	SIGNED_ROUND(pitch_sum)
	pitch_sum /= (int64_t)uN;

	SIGNED_ROUND(roll_sum)
	roll_sum /= (int64_t)uN;

	SIGNED_ROUND(yaw_sum)
	yaw_sum /= (int64_t)uN;

	ip = pitch_sum, ir = roll_sum, iy = yaw_sum;

	writeRawOrientationOffsets(ip, ir, iy);


	if ( bDebug )
	{
		(void)printf("\nThe gyroscope offsets are %d (x) %d (y) %d (z)\n", (int32_t)x_sum, (int32_t)y_sum, (int32_t)z_sum);
		(void)printf("The acceleration offsets are %d (x) %d (y) %d (z)\n", (int32_t)xx_sum, (int32_t)yy_sum, (int32_t)zz_sum);	
		(void)printf("The linear acceleration offsets are %d (x) %d (y) %d (z)\n", (int32_t)xxx_sum, (int32_t)yyy_sum, (int32_t)zzz_sum);		
		(void)printf("The gravity offsets are %d (x) %d (y) %d (z)\n", (int32_t)xxxx_sum, (int32_t)yyyy_sum, (int32_t)zzzz_sum);		
		(void)printf("The magnetometer offsets are %d (x) %d (y) %d (z)\n", (int32_t)xxxxx_sum, (int32_t)yyyyy_sum, (int32_t)zzzzz_sum);			
		(void)printf("The quaternion offsets are %d (w) %d (x) %d (y) %d (z)\n", (int32_t)qw_sum, (int32_t)qx_sum, (int32_t)qy_sum, (int32_t)qz_sum);			
		(void)printf("The orientation (rotation) offsets are %d (pitch) %d (roll) %d (yaw)\n\n", (int32_t)pitch_sum, (int32_t)roll_sum, (int32_t)yaw_sum);
	}

	bCalibrated = true;

}

void BNO055::writeRawGyroscopeOffsets(int32_t &x, int32_t &y, int32_t &z)
{
	in_anglvel_x_offset = x;
	in_anglvel_y_offset = y;
	in_anglvel_z_offset = z;
}

void BNO055::readRawGyroscopeValues(int32_t &x, int32_t &y, int32_t &z)
{
	x = y = z = 0;

	const char *pAngles[NUMBER_OF_ANGLES] =
	{
		{ IIO_PATH_PREFACE "/in_anglvel_x_raw"   },
		{ IIO_PATH_PREFACE "/in_anglvel_y_raw"   },
		{ IIO_PATH_PREFACE "/in_anglvel_z_raw"	}
	};

	int32_t *pXYZ[NUMBER_OF_ANGLES] =
	{
		(int32_t *)&x, (int32_t *)&y, (int32_t *)&z
	};

	for ( int32_t i = 0 ; i < NUMBER_OF_ANGLES ; i++ )
	{
		if ( bDebug )
			(void)printf("\nTrying to open \"%s.\"\n", pAngles[i]);

		pHandle = fopen(pAngles[i],"r");
		
		if ( NULL == pHandle )
		{	
			(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pAngles[i], strerror(errno));	
			break;		
		}

		else if ( bDebug )
			(void)printf("Successfully opened \"%s.\"\n", pAngles[i]);	
		else
			;
		
		*pXYZ[i] = 0;
		
		if ( 1 != fscanf(pHandle, "%i", pXYZ[i]) )
		{
			(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pAngles[i], strerror(errno));
		}
		else if ( bDebug )
		{
			(void)printf("The raw Gyroscope value from \"%s\" is \"%i.\"\n", pAngles[i], *pXYZ[i]);
		}
		else
			;

		if ( NULL!=pHandle )
			(void)fclose(pHandle);
		pHandle = NULL;	
	}

	return;
}

// That is, acceleration due to forces excluding gravity.
void BNO055::readRawLinearAccelerations(int32_t &x, int32_t &y, int32_t &z)
{
	const char *pAccels[NUMBER_OF_AXES] =
	{
		{ IIO_PATH_PREFACE "/in_accel_linear_x_raw"	},
		{ IIO_PATH_PREFACE "/in_accel_linear_y_raw" },
		{ IIO_PATH_PREFACE "/in_accel_linear_z_raw"	}
	};

	int32_t *pXYZ[NUMBER_OF_AXES] =
	{
		(int32_t *)&x, (int32_t *)&y, (int32_t *)&z
	};

	for ( int32_t i = 0 ; i < NUMBER_OF_AXES ; i++ )
	{
		if ( bDebug )
			(void)printf("\nTrying to open \"%s.\"\n", pAccels[i]);

		pHandle = fopen(pAccels[i],"r");
		
		if ( NULL == pHandle )
		{	
			(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pAccels[i], strerror(errno));	
			break;		
		}

		else if ( bDebug )
			(void)printf("Successfully opened \"%s.\"\n", pAccels[i]);	
		else
			;
		
		*pXYZ[i] = 0;
		
		if ( 1 != fscanf(pHandle, "%i", pXYZ[i]) )
		{
			(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pAccels[i], strerror(errno));
		}
		else if ( bDebug )
		{
			(void)printf("The raw linear acceleration value from \"%s\" is \"%i.\"\n", pAccels[i], *pXYZ[i]);
		}
		else
			;

		if ( NULL!=pHandle )
			(void)fclose(pHandle);
		pHandle = NULL;	
	}

	return;
}

void BNO055::readLinearAccelerations(double_t &x, double_t &y, double_t &z)
{
	int32_t ix=0, iy=0, iz=0;

	double_t *prXYZ[NUMBER_OF_AXES] =
	{
		(double_t *)&x, (double_t *)&y, (double_t *)&z
	};

	int32_t *pXYZ[NUMBER_OF_AXES] =
	{
		(int32_t *)&ix, (int32_t *)&iy, (int32_t *)&iz
	};

	int32_t *pXYZOffsets[NUMBER_OF_AXES] =
	{
		&in_linear_accel_x_offset, &in_linear_accel_y_offset, &in_linear_accel_z_offset
	};

	readRawLinearAccelerations(ix, iy, iz);

	for ( int32_t i = 0 ; i < NUMBER_OF_AXES ; i++ )
	{
		*prXYZ[i] = in_accel_scale * (double_t)( *pXYZ[i] - *pXYZOffsets[i] );
	}	

	if ( bDebug )
	{
		(void)printf("The acceleration values (x,y,z) are %lf, %lf, %lf.\n", x, y, z);
	}	
}

void BNO055::writeRawLinearAccelerationOffsets(int32_t &x, int32_t &y, int32_t &z)
{
	in_linear_accel_x_offset = x;
	in_linear_accel_y_offset = y;
	in_linear_accel_z_offset = z;
}

void BNO055::readRawAccelerations(int32_t &x, int32_t &y, int32_t &z)
{
	const char *pAccels[NUMBER_OF_AXES] =
	{
		{ IIO_PATH_PREFACE "/in_accel_x_raw"	},
		{ IIO_PATH_PREFACE "/in_accel_y_raw"   	},
		{ IIO_PATH_PREFACE "/in_accel_z_raw"	}
	};

	int32_t *pXYZ[NUMBER_OF_AXES] =
	{
		(int32_t *)&x, (int32_t *)&y, (int32_t *)&z
	};

	for ( int32_t i = 0 ; i < NUMBER_OF_AXES ; i++ )
	{
		if ( bDebug )
			(void)printf("\nTrying to open \"%s.\"\n", pAccels[i]);

		pHandle = fopen(pAccels[i],"r");
		
		if ( NULL == pHandle )
		{	
			(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pAccels[i], strerror(errno));	
			break;		
		}

		else if ( bDebug )
			(void)printf("Successfully opened \"%s.\"\n", pAccels[i]);	
		else
			;
		
		*pXYZ[i] = 0;
		
		if ( 1 != fscanf(pHandle, "%i", pXYZ[i]) )
		{
			(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pAccels[i], strerror(errno));
		}
		else if ( bDebug )
		{
			(void)printf("The raw acceleration value from \"%s\" is \"%i.\"\n", pAccels[i], *pXYZ[i]);
		}
		else
			;

		if ( NULL!=pHandle )
			(void)fclose(pHandle);
		pHandle = NULL;	
	}

	return;
}

void BNO055::readAccelerations(double_t &x, double_t &y, double_t &z)
{
	int32_t ix=0, iy=0, iz=0;

	double_t *prXYZ[NUMBER_OF_AXES] =
	{
		(double_t *)&x, (double_t *)&y, (double_t *)&z
	};

	int32_t *pXYZ[NUMBER_OF_AXES] =
	{
		(int32_t *)&ix, (int32_t *)&iy, (int32_t *)&iz
	};

	int32_t *pXYZOffsets[NUMBER_OF_AXES] =
	{
		&in_accel_x_offset, &in_accel_y_offset, &in_accel_z_offset
	};

	readRawAccelerations(ix, iy, iz);

	for ( int32_t i = 0 ; i < NUMBER_OF_AXES ; i++ )
	{
		*prXYZ[i] = in_accel_scale * (double_t)( *pXYZ[i] - *pXYZOffsets[i] );
	}	

	if ( bDebug )
	{
		(void)printf("The acceleration values (x,y,z) are %lf, %lf, %lf.\n", x, y, z);
	}	
}

void BNO055::writeRawAccelerationOffsets(int32_t &x, int32_t &y, int32_t &z)
{
	in_accel_x_offset = x;
	in_accel_y_offset = y;
	in_accel_z_offset = z;
}

void BNO055::readRawGravityValues(int32_t &x, int32_t &y, int32_t &z)
{
	const char *pGees[NUMBER_OF_AXES] =
	{
		{ IIO_PATH_PREFACE "/in_gravity_x_raw"	},
		{ IIO_PATH_PREFACE "/in_gravity_y_raw"  },
		{ IIO_PATH_PREFACE "/in_gravity_z_raw"	}
	};

	int32_t *pXYZ[NUMBER_OF_AXES] =
	{
		(int32_t *)&x, (int32_t *)&y, (int32_t *)&z
	};

	for ( int32_t i = 0 ; i < NUMBER_OF_AXES ; i++ )
	{
		if ( bDebug )
			(void)printf("\nTrying to open \"%s.\"\n", pGees[i]);

		pHandle = fopen(pGees[i],"r");
		
		if ( NULL == pHandle )
		{	
			(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pGees[i], strerror(errno));	
			break;		
		}

		else if ( bDebug )
			(void)printf("Successfully opened \"%s.\"\n", pGees[i]);	
		else
			;
		
		*pXYZ[i] = 0;
		
		if ( 1 != fscanf(pHandle, "%i", pXYZ[i]) )
		{
			(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pGees[i], strerror(errno));
		}
		else if ( bDebug )
		{
			(void)printf("The raw gravity value from \"%s\" is \"%i.\"\n", pGees[i], *pXYZ[i]);
		}
		else
			;

		if ( NULL!=pHandle )
			(void)fclose(pHandle);
		pHandle = NULL;	
	}

	return;
}

void BNO055::writeRawGravityOffsets(int32_t &x, int32_t &y, int32_t &z)
{
	in_gravity_x_offset = x, in_gravity_y_offset = y, in_gravity_z_offset = z;
}

void BNO055::readGravities(double_t &x, double_t &y, double_t &z)
{
	int32_t ix=0, iy=0, iz=0;

	double_t *prXYZ[NUMBER_OF_AXES] =
	{
		(double_t *)&x, (double_t *)&y, (double_t *)&z
	};

	int32_t *pXYZ[NUMBER_OF_AXES] =
	{
		(int32_t *)&ix, (int32_t *)&iy, (int32_t *)&iz
	};

	int32_t *pXYZOffsets[NUMBER_OF_AXES] =
	{
		&in_gravity_x_offset, &in_gravity_y_offset, &in_gravity_z_offset
	};

	readRawGravityValues(ix, iy, iz);

	for ( int32_t i = 0 ; i < NUMBER_OF_AXES ; i++ )
	{
		*prXYZ[i] = in_gravity_scale * (double_t)( *pXYZ[i] - *pXYZOffsets[i] );
	}	

	if ( bDebug )
	{
		(void)printf("The gravity values (x,y,z) are %lf, %lf, %lf.\n", x, y, z);
	}	
}

void BNO055::readRawCompassAngles(int32_t &x, int32_t &y, int32_t &z)
{
	const char *pCompass[NUMBER_OF_AXES] =
	{
		{ IIO_PATH_PREFACE "/in_magn_x_raw" },
		{ IIO_PATH_PREFACE "/in_magn_y_raw" },
		{ IIO_PATH_PREFACE "/in_magn_z_raw"	}
	};

	int32_t *pXYZ[NUMBER_OF_AXES] =
	{
		(int32_t *)&x, (int32_t *)&y, (int32_t *)&z
	};

	for ( int32_t i = 0 ; i < NUMBER_OF_AXES ; i++ )
	{
		if ( bDebug )
			(void)printf("\nTrying to open \"%s.\"\n", pCompass[i]);

		pHandle = fopen(pCompass[i],"r");
		
		if ( NULL == pHandle )
		{	
			(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pCompass[i], strerror(errno));	
			break;		
		}

		else if ( bDebug )
			(void)printf("Successfully opened \"%s.\"\n", pCompass[i]);	
		else
			;
		
		*pXYZ[i] = 0;
		
		if ( 1 != fscanf(pHandle, "%i", pXYZ[i]) )
		{
			(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pCompass[i], strerror(errno));
		}
		else if ( bDebug )
		{
			(void)printf("The raw Magnetometer value from \"%s\" is \"%i.\"\n", pCompass[i], *pXYZ[i]);
		}
		else
			;

		if ( NULL!=pHandle )
			(void)fclose(pHandle);
		pHandle = NULL;	
	}

	return;
}

void BNO055::readCompass(double_t &x, double_t &y, double_t &z)
{
	int32_t ix=0, iy=0, iz=0;

	double_t *prXYZ[NUMBER_OF_AXES] =
	{
		(double_t *)&x, (double_t *)&y, (double_t *)&z
	};

	int32_t *pXYZ[NUMBER_OF_AXES] =
	{
		(int32_t *)&ix, (int32_t *)&iy, (int32_t *)&iz
	};

	int32_t *pXYZOffsets[NUMBER_OF_AXES] =
	{
		&in_magn_x_offset, &in_magn_y_offset, &in_magn_z_offset
	};

	readRawCompassAngles(ix, iy, iz);

	for ( int32_t i = 0 ; i < NUMBER_OF_AXES ; i++ )
	{
		*prXYZ[i] = in_magn_scale * (double_t)( *pXYZ[i] - *pXYZOffsets[i] );
	}	

	if ( bDebug )
	{
		(void)printf("The Magnetometer values (x,y,z) are %lf, %lf, %lf.\n", x, y, z);
	}	
}

void BNO055::writeRawCompassOffsets(int32_t &x, int32_t &y, int32_t &z)
{
	in_magn_x_offset = x, in_magn_y_offset = y, in_magn_z_offset = z;
}

void BNO055::readRawQuaternions(int32_t &w, int32_t &x, int32_t &y, int32_t &z)
{
	const char *pQuaternions = IIO_PATH_PREFACE "/in_rot_quaternion_raw";

	w = x = y =z = 0;

	if ( bDebug )
		(void)printf("\nTrying to open \"%s.\"\n", pQuaternions);

	pHandle = fopen(pQuaternions,"r");
	
	if ( NULL == pHandle )
	{	
		(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pQuaternions, strerror(errno));
		w = x = y =z = 0;
		return;
	}

	else if ( bDebug )
		(void)printf("Successfully opened \"%s.\"\n", pQuaternions);	
	else
		;
	
	
	if ( NUMBER_OF_QUATERNIONS != fscanf(pHandle, "%i%i%i%i", &w, &x, &y, &z) )
	{
		(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pQuaternions, strerror(errno));
	}
	else if ( bDebug )
	{
		(void)printf("The raw Quaternion values (w, x, y, z) from \"%s\" are \"%i %i %i %i.\"\n", pQuaternions, w, x, y, z);
	}
	else
		;

	if ( NULL!=pHandle )
		(void)fclose(pHandle);
	pHandle = NULL;	

	return;
}

void BNO055::writeRawQuaternionOffsets(int32_t &w, int32_t &x, int32_t &y, int32_t &z)
{
	in_quaternion_w_offset = w, in_quaternion_x_offset = x, in_quaternion_y_offset = y, in_quaternion_z_offset = z;
}

void BNO055::readQuaternions(double_t &w, double_t &x, double_t &y, double_t &z)
{
	int32_t iw = 0, ix=0, iy=0, iz=0;

	double_t *prWXYZ[NUMBER_OF_QUATERNIONS] =
	{
		(double_t *)&w, (double_t *)&x, (double_t *)&y, (double_t *)&z
	};

	int32_t *pWXYZ[NUMBER_OF_QUATERNIONS] =
	{
		(int32_t *)&iw, (int32_t *)&ix, (int32_t *)&iy, (int32_t *)&iz
	};

	int32_t *pWXYZOffsets[NUMBER_OF_QUATERNIONS] =
	{
		&in_quaternion_w_offset, &in_quaternion_x_offset, &in_quaternion_y_offset, &in_quaternion_z_offset
	};

	readRawQuaternions(iw, ix, iy, iz);

	for ( int32_t i = 0 ; i < NUMBER_OF_QUATERNIONS ; i++ )
	{
		*prWXYZ[i] = in_rot_scale * (double_t)( *pWXYZ[i] - *pWXYZOffsets[i] );
	}	

	if ( bDebug )
	{
		(void)printf("The Quaternion values (w,x,y,z) are %lf %lf, %lf, %lf.\n", w, x, y, z);
	}	
}

void BNO055::readRawOrientation(int32_t &pitch, int32_t &roll, int32_t &yaw)
{
	const char *pOrientation[NUMBER_OF_AXES] =
	{
		{ IIO_PATH_PREFACE "/in_rot_pitch_raw"  },
		{ IIO_PATH_PREFACE "/in_rot_roll_raw"   },
		{ IIO_PATH_PREFACE "/in_rot_yaw_raw"	}
	};

	int32_t *pXYZ[NUMBER_OF_AXES] =
	{
		(int32_t *)&pitch, (int32_t *)&roll, (int32_t *)&yaw
	};

	for ( int32_t i = 0 ; i < NUMBER_OF_AXES ; i++ )
	{
		if ( bDebug )
			(void)printf("\nTrying to open \"%s.\"\n", pOrientation[i]);

		pHandle = fopen(pOrientation[i],"r");
		
		if ( NULL == pHandle )
		{	
			(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pOrientation[i], strerror(errno));
			if ( NULL!=pHandle )
				(void)fclose(pHandle);
			pHandle = NULL;					
			break;		
		}

		else if ( bDebug )
			(void)printf("Successfully opened \"%s.\"\n", pOrientation[i]);	
		else
			;
		
		*pXYZ[i] = 0;
		
		if ( 1 != fscanf(pHandle, "%i", pXYZ[i]) )
		{
			(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pOrientation[i], strerror(errno));
		}
		else if ( bDebug )
		{
			(void)printf("The raw rotation (orientation) value from \"%s\" is \"%i.\"\n", pOrientation[i], *pXYZ[i]);
		}
		else
			;

		if ( NULL!=pHandle )
			(void)fclose(pHandle);
		pHandle = NULL;	
	}

	return;
}

void BNO055::writeRawOrientationOffsets(int32_t &pitch, int32_t &roll, int32_t &yaw)
{
 	in_orientation_roll_offset = roll, 
		in_orientation_pitch_offset = pitch, 
		in_orientation_yaw_offset = yaw;
}

void BNO055::readOrientation(double_t &pitch, double_t &roll, double_t &yaw)
{
	int32_t ix=0, iy=0, iz=0;

	double_t *prXYZ[NUMBER_OF_AXES] =
	{
		(double_t *)&pitch, (double_t *)&roll, (double_t *)&yaw
	};

	int32_t *pXYZ[NUMBER_OF_AXES] =
	{
		(int32_t *)&ix, (int32_t *)&iy, (int32_t *)&iz
	};

	int32_t *prXYZOffsets[NUMBER_OF_AXES] =
	{
		&in_orientation_pitch_offset, &in_orientation_roll_offset, &in_orientation_yaw_offset
	};

	readRawOrientation(ix, iy, iz);

	for ( int32_t i = 0 ; i < NUMBER_OF_AXES ; i++ )
	{
		*prXYZ[i] = in_rot_scale * (double_t)( *pXYZ[i] - *prXYZOffsets[i] );
	}	

	if ( bDebug )
	{
		(void)printf("The orientation (rotation) values (pitch, roll, yaw) are %lf, %lf, %lf.\n", pitch, roll, yaw);
	}	
}
