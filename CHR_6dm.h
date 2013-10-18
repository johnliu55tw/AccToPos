#ifndef CHR_6DM_H
#define CHR_6DM_H 1
#include <limits.h>
#include <pthread.h>

#include "SerialPort_cpp.h"
#include "CHR_SensorData.h"
#include "CHR_PacketType.h"


using namespace CHR;

enum RValue{
	CHR_OK,
	CHR_Timeout,
	CHR_Error,
	CHR_ErrorCommand
};

enum UpdateMode{
	BLOCKING,
	NONBLOCKING
};

enum Costants {
	BUF_MAX = 255
};

/* 
   Data structure stores the configurations and shared data.
*/
struct SharedData {
	pthread_mutex_t mutex;
	char *path;
	byte *data;
	int baudrate;
	int timeout;
	int dataNumber;
	bool updated;
	bool measurement_mode;
};

class CHR_6dm {
public:
	CHR_6dm(const char *);
	~CHR_6dm();

	/* 
	   When invoking the open method for the first time, this driver
	   will send a SELF_TEST command to the device to ensure the
	   device is correctly connected. After receiving the response
	   data, it will also set the device into SILENT_MODE. Thus
	   users must invoke the gotoMeasurementMode method in order to 
	   make the device sending sensor packet automatically.
	   
	   This method needs a Timeout argument, representing the 
	   timeout in reading the data.
	*/
	RValue open(int Timeout);

	/*
	   This method will resets all AHRS settings to factory default
	   value.
	*/
	RValue resetToFactory();

	/*
	   This method simply set the device into SLIENT_MODE, not that
	   there really exist a config mode in the device. However, if
	   user wants to change any settings of the device, they have 
	   to make sure the device is in a so called "config mode". 
	   I think this is a better way to prevent accidentally option
	   changing.
	*/
	RValue gotoConfigMode();

	/*
	   Set active channels in the device. the class CHR::SensorData
	   is used to preserve the settings, thus it's passed by 
	   pointer.
	*/
	RValue setActiveChannels(SensorData *);

	/*
	   This is how you can get the data in SLIENT_MODE.
	*/
	RValue getData(SensorData *);

	/*
	   Set the device into BROADCAST_MODE, which transmits the data
	   automatically. The second argument is the samping rate.
	*/
	RValue gotoMeasurementMode(SensorData *, int);

	/*
	   This method will block until the data received by the driver
	   from the device is updated.
	*/
	RValue isUpdated(UpdateMode);

private:
	struct SharedData _shared;
	SerialPort serial;
	SensorData *_data_packet;
	pthread_t _comm_thread;

	RValue _sending_command(byte *buffer);
	static void *_communicator(void *ptr);
	static void _cleanup_function(void *ptr);
};
	
#endif
