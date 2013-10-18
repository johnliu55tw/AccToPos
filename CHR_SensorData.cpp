#include"CHR_SensorData.h"

CHR::DataField::DataField()
{
	data = new short[15];
	cal_data = new double[5];
	cal_data[0] = 0.0109863;
	cal_data[1] = 0.0137329;
	cal_data[2] = 0.061035;
	cal_data[3] = 0.01812;
	cal_data[4] = 0.15387;
}

CHR::DataField::~DataField()
{
	delete data;
	delete cal_data;
}

double CHR::DataField::yaw() {return ((double)data[0] * cal_data[0]);}

double CHR::DataField::pitch() {return ((double)data[1] * cal_data[0]);}

double CHR::DataField::roll() {return ((double)data[2] * cal_data[0]);}

double CHR::DataField::yaw_rate() {return ((double)data[3] * cal_data[1]);}

double CHR::DataField::pitch_rate() {return ((double)data[4] * cal_data[1]);}

double CHR::DataField::roll_rate() {return ((double)data[5] * cal_data[1]);}

double CHR::DataField::mag_x() {return ((double)data[6] * cal_data[2]);}

double CHR::DataField::mag_y() {return ((double)data[7] * cal_data[2]);}

double CHR::DataField::mag_z() {return ((double)data[8] * cal_data[2]);}

double CHR::DataField::gyro_x() {return ((double)data[9] * cal_data[3]);}

double CHR::DataField::gyro_y() {return ((double)data[10] * cal_data[3]);}

double CHR::DataField::gyro_z() {return ((double)data[11] * cal_data[3]);}

double CHR::DataField::accel_x() {return ((double)data[12] * cal_data[4]);}

double CHR::DataField::accel_y() {return ((double)data[13] * cal_data[4]);}

double CHR::DataField::accel_z() {return ((double)data[14] * cal_data[4]);}

CHR::SensorData::SensorData()
{
	/* Default Channel : Euler Angle only */
	_channel_mask = 0xE000;
}

mask_16 CHR::SensorData::enableChannel(Channels channel)
{
	_channel_mask |= (1 << channel);
	return _channel_mask;
}

mask_16 CHR::SensorData::disableChannel(Channels channel)
{
	_channel_mask &= ~(1 << channel);
	return _channel_mask;
}

mask_16 CHR::SensorData::enableChannel(ChannelGroups group)
{
	_channel_mask |= (7 << (1 + group * 3));
	return _channel_mask;
}
mask_16 CHR::SensorData::disableChannel(ChannelGroups group)
{
	_channel_mask &= ~(7 << (1 + group * 3));
	return _channel_mask;
}

bool CHR::SensorData::containedChannel(Channels channel)
{
	return (_channel_mask & (1 << channel));
}

bool CHR::SensorData::containedChannel(ChannelGroups group)
{
	mask_16 mask = 7 << (1 + group * 3);
	return ((_channel_mask & mask) == mask);
}

mask_16 CHR::SensorData::getChannelMask()
{
	return _channel_mask;
}

unsigned char CHR::SensorData::getFirstChannelMask()
{
	return _channel_mask >> 8;
}

unsigned char CHR::SensorData::getSecondChannelMask()
{
	return _channel_mask & 0xFF;
}

int CHR::SensorData::getNumberOfChannel()
{
	int count = 0;
	for (int i=0; i<16; i++) {
		if (_channel_mask & (1 << i))
			count++;
	}
	return count;
}

bool CHR::SensorData::resolvePacket(unsigned char *packet)
{
	mask_16 packet_mask = (packet[5] << 8) + packet[6];
	if (_channel_mask != packet_mask)
		return false;

	mask_16 bit_mask = 0x8000;
	int packet_cnt = 7;
	for (int i=0; i<16; i++) {
		if (packet_mask & (bit_mask >> i)) {
			datafield.data[i] = packet[packet_cnt] << 8;
			packet_cnt++;
			datafield.data[i] += packet[packet_cnt];
			packet_cnt++;
		} else {
			datafield.data[i] = 0;
		}
	}
	return true;
}
