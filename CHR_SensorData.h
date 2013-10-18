#ifndef SENSORDATA_H
#define SENSORDATA_H 1

#include<string.h>

typedef unsigned short mask_16;

namespace CHR {
/* Enumerators for enable/disable channels */
enum Channels{az = 1, ay, ax, gz, gy, gx, mz, my, mx,
	      roll_rate, pitch_rate, yaw_rate, roll, pitch, yaw};

/* Enumerator for enable/disable a sensor set */
enum ChannelGroups{Accelerometer, RateGyro, Magnetometer,
	           EulerRate, EulerAngle};

class DataField {
public:
	DataField();
	~DataField();
	short *data;
	double *cal_data;
	double yaw();
	double pitch();
	double roll();
	double yaw_rate();
	double pitch_rate();
	double roll_rate();
	double mag_z();
	double mag_y();
	double mag_x();
	double gyro_z();
	double gyro_y();
	double gyro_x();
	double accel_z();
	double accel_y();
	double accel_x();
};


/* Packet class */
class SensorData {
public:
	SensorData();

	/* Functions for enable/disable channels */
	mask_16 enableChannel(Channels);
	mask_16 disableChannel(Channels);
	mask_16 enableChannel(ChannelGroups);
	mask_16 disableChannel(ChannelGroups);
	
	/* Functions for testing the data */
	bool containedChannel(Channels);
	bool containedChannel(ChannelGroups);

	/* Get the channel mask */
	mask_16 getChannelMask();

	/* Get first nibble of the channel mask */
	unsigned char getFirstChannelMask();

	/* Get second nibble of the channel mask */
	unsigned char getSecondChannelMask();
	
	/* Get number of enabled channel */
	int getNumberOfChannel();

	/* Transform the data in the packet into corresponding field */
	bool resolvePacket(unsigned char *packet);
	
	CHR::DataField datafield;
private:
	/* Channel setting */
	mask_16 _channel_mask;
};

};
#endif
