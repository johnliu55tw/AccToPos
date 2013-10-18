#ifndef CHR_PACKETTYPE_H
#define CHR_PACKETTYPE_H 1

namespace CHR {

enum TX_Packet{
	COMMAND_COMPLETE	= 0xB0,
	COMMAND_FAILED,
	BAD_CHECKSUM,
	BAD_DATA_LENGTH,
	UNRECOGNIZED_PACKET,
	BUFFER_OVERFLOW,
	STATUS_REPORT,
	SENSOR_DATA,
	GYRO_BIAS_REPORT,
	GYRO_SCALE_REPORT,
	START_CAL_REPORT,
	ACCEL_REF_VECTOR_REPORT,
	ACTIVE_CHANNEL_REPORT,
	ACCEL_COVARIANCE_REPORT,
	MAG_COVARIANCE_REPORT,
	PROCESS_COVARIANCE_REPORT,
	STATE_COVARIANCE_REPORT,
	EKF_CONFIG_REPORT,
	GYRO_ALIGNMENT_REPORT,
	ACCEL_ALIGNMENT_REPORT,
	MAG_REF_VECTOR_REPORT,
	MAG_CAL_REPORT,
	MAG_BIAS_REPORT,
	BROADCAST_MODE_REPORT
};

enum RX_Packet{
	SET_ACTIVE_CHANNELS	= 0x80,
	SET_SILENT_MODE,
	SET_BROADCAST_MODE,
	SET_GYRO_BIAS,
	SET_ACCEL_BIAS,
	SET_ACCEL_REF_VECTOR,
	AUTO_SET_ACCEL_REF,
	ZERO_RATE_GYROS,
	SELF_TEST,
	SET_START_CAL,
	SET_PROCESS_COVARIANCE,
	SET_MAG_COVARIANCE,
	SET_ACCEL_COVARIANCE,
	SET_EKF_CONFIG,
	SET_GYRO_ALIGNMENT,
	SET_ACCEL_ALIGNMENT,
	SET_MAG_REF_VECTOR,
	AUTO_SET_MAG_REF,
	SET_MAG_CAL,
	SET_MAG_BIAS,
	SET_GYRO_SCALE,
	EKF_RESET,
	RESET_TO_FACTORY,
	WRITE_TO_FLASH		= 0xA0,
	GET_DATA		= 0x01,
	GET_ACTIVE_CHANNELS,
	GET_BROADCAST_MODE,
	GET_ACCEL_BIAS,
	GET_ACCEL_REF_VECTOR,
	GET_GYRO_BIAS,
	GET_GYRO_SCALE,
	GET_START_CAL,
	GET_EKF_CONFIG,
	GET_ACCEL_COVARIANCE,
	GET_MAG_COVARIANCE,
	GET_PROCESS_COVARIANCE,
	GET_STATE_COVARIANCE,
	GET_GYRO_ALIGNMENT,
	GET_ACCEL_ALIGNMENT,
	GET_MAG_REF_VECTOR,
	GET_MAG_CAL,
	GET_MAG_BIAS
};

inline void addHeader(unsigned char *packet)
{
	packet[0] = 's';
	packet[1] = 'n';
	packet[2] = 'p';
}

inline unsigned short addCheckSum(unsigned char *packet)
{
	int N = packet[4];
	unsigned short sum = 0;
	for (int i=0; i<(N+5); i++)
		sum += packet[i];
	packet[N+5] = sum >> 8;
	packet[N+6] = sum & 0xFF;
	return sum;
}

inline bool verifyCheckSum(unsigned char *packet)
{
	int N = packet[4];
	unsigned short sum = 0;
	for (int i=0; i<(N+5); i++)
		sum += packet[i];
	
	unsigned short sum_inpacket = (packet[N+5] << 8) + packet[N+6];
	
	if (sum_inpacket != sum) {
		fprintf(stderr, "Error CheckSum!\n");
		fprintf(stderr, "Calculated Checksum : %X\n", sum);
		fprintf(stderr, "Checksum in Packet  : %X\n", sum_inpacket);
	}
	return (sum_inpacket == sum);
}

inline bool verifyPacket(unsigned char *packet)
{
	if (packet[0] != 's' || packet[1] != 'n' || packet[2] != 'p')
		return false;
	return verifyCheckSum(packet);
}

inline bool verifyPacket(unsigned char *packet, CHR::TX_Packet type)
{
	if (verifyPacket(packet))
		return (packet[3] == type);
	return false;
}

};
#endif
