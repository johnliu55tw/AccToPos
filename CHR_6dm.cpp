#include "CHR_6dm.h"

CHR_6dm::CHR_6dm(const char *path)
{
	/* Initializing SharedData strcuture */
	_shared.mutex = PTHREAD_MUTEX_INITIALIZER;
	_shared.data = new byte[BUF_MAX];
	_shared.path = new char[PATH_MAX];
	strcpy(_shared.path, path);
	_shared.baudrate = 115200;
	_shared.dataNumber = 0;
}

CHR_6dm::~CHR_6dm()
{
	delete _shared.data;
	delete _shared.path;
	pthread_cancel(_comm_thread);
}

RValue CHR_6dm::open(int Timeout)
{
	_shared.timeout = Timeout;
	_shared.measurement_mode = false;

	int ret;
	int count = 0;
	while ((ret = serial.OpenPort(_shared.path)) < 0) {
		perror("Open serial port");
		fprintf(stderr, "wait for 3 seconds...\n");
		sleep(3);
		count++;
		if (count >= 10)
			return CHR_Error;
	}

	serial.SetAttr(_shared.baudrate, Timeout, 0);
	return gotoConfigMode();
}

/*
   This method will resets all AHRS settings to factory default
   value.
*/
//RValue CHR_6dm::resetToFactory();

/*
   This method simply set the device into SLIENT_MODE, not that
   there really exist a config mode in the device. However, if
   user wants to change any settings of the device, they have 
   to make sure the device is in a so called "config mode". 
   I think this is a better way to prevent accidentally option
   changing.
*/
RValue CHR_6dm::gotoConfigMode()
{
	if (!_shared.measurement_mode) {
		addHeader(_shared.data);
		_shared.data[3] = SET_SILENT_MODE;
		_shared.data[4] = 0;
		addCheckSum(_shared.data);
		return _sending_command(_shared.data);
	} else {
		pthread_cancel(_comm_thread);
		return open(_shared.timeout);
	}
}

/*
   Set active channels in the device. the class SensorData
   is used to preserve the settings, thus it's passed by 
   pointer.
*/
RValue CHR_6dm::setActiveChannels(SensorData *sensor)
{
	addHeader(_shared.data);
	_shared.data[3] = SET_ACTIVE_CHANNELS;
	_shared.data[4] = 2;
	_shared.data[5] = sensor->getFirstChannelMask();
	_shared.data[6] = sensor->getSecondChannelMask();
	addCheckSum(_shared.data);

	RValue ret_val = _sending_command(_shared.data);
	if (ret_val == CHR_OK) {
		_shared.dataNumber = (sensor->getNumberOfChannel() * 2) + 9;
		printf("Number of data : %d\n", _shared.dataNumber);
	}
	return ret_val;
}

/*
   This is how you can get the data in SLIENT_MODE.
*/
RValue CHR_6dm::getData(SensorData *sensor)
{
	byte local_buffer[BUF_MAX];
	RValue ret_val;
	if (!_shared.measurement_mode) {
		addHeader(_shared.data);
		_shared.data[3] = GET_DATA;
		_shared.data[4] = 0;
		addCheckSum(_shared.data);
		ret_val = _sending_command(_shared.data);
		if (ret_val != CHR_OK)
			return ret_val;
	}

	pthread_mutex_lock(&_shared.mutex);
	memcpy(local_buffer, _shared.data, _shared.dataNumber);
	pthread_mutex_unlock(&_shared.mutex);

	sensor->resolvePacket(local_buffer);
	printf("mask = %X%X\n", local_buffer[5], local_buffer[6]);

	return CHR_OK;
}

/*
   Set the device into BROADCAST_MODE, which transmits the data
   automatically. The second argument is the samping rate.
*/
RValue CHR_6dm::gotoMeasurementMode(SensorData *sensor, int freq)
{
	addHeader(_shared.data);
	_shared.data[3] = SET_BROADCAST_MODE;
	_shared.data[4] = 1;
	RValue ret_val;

	if (freq < 20 || freq > 300) {
		return CHR_ErrorCommand;
	}
	else {
		_shared.data[5] = ((freq - 20) * 255) / 280;
		printf("X = %X\n", _shared.data[5]);
	}
	addCheckSum(_shared.data);
	
	ret_val = _sending_command(_shared.data);
	if (ret_val != CHR_OK)
		return ret_val;

	serial.ClosePort();

	if (pthread_create(&_comm_thread,NULL,CHR_6dm::_communicator,(void*)&_shared) < 0) {
		perror("Thread Creation");
		return CHR_Error;
	}

	_shared.measurement_mode = true;
	return CHR_OK;
}

/*
   This method will block until the data received by the driver
   from the device is updated.
*/
//RValue CHR_6dm::isUpdated(UpdateMode mode)

void *CHR_6dm::_communicator(void *ptr)
{
	struct SharedData *s = (struct SharedData *)ptr;
	class SerialPort serial(s->path);
	serial.OpenPort();
	serial.SetAttr(s->baudrate, 1, s->dataNumber);
	int rd, wr;

	byte local_buffer[BUF_MAX];

	pthread_cleanup_push(CHR_6dm::_cleanup_function, ptr);

	int error_cnt = 0;
	while (1) {
		rd = serial.ReadData(local_buffer, s->dataNumber);
		if (verifyPacket(local_buffer)) {
			/* DEBUG
			printf("Data Received:");
			for (int i=0; i<rd; i++) {
				printf("%X ", local_buffer[i]);
			}
			printf("\n");
			*/
			pthread_mutex_lock(&s->mutex);
			s->updated = true;
			memcpy(s->data, local_buffer, s->dataNumber);
			pthread_mutex_unlock(&s->mutex);
		} else {
			error_cnt++;
			if (error_cnt >= 10) {
				rd = serial.ReadData(local_buffer, BUF_MAX);
				error_cnt = 0;
			}
		}

	}
	pthread_cleanup_pop(0);

}

void CHR_6dm::_cleanup_function(void *ptr)
{
	struct SharedData *s = (struct SharedData *)ptr;
	pthread_mutex_unlock(&s->mutex);
}
	
RValue CHR_6dm::_sending_command(byte *buffer)
{
	int count = 0;
	int wr, rd;
	byte *local_buffer = new byte[BUF_MAX]; 
	RValue ret_val;

	while(1) {
		count++;
		if (count >= 100) {
			ret_val = CHR_ErrorCommand;
			break;
		}

		wr = serial.WriteData(buffer, buffer[4] + 7);

		if (wr <= 0) {
			perror("Write Data");
			ret_val =  CHR_Error;
			break;
		}
		rd = serial.ReadData(local_buffer, BUF_MAX);
		
		if (rd == 0) {
			ret_val = CHR_Timeout;
			break;
		}
		else if (rd < 0) {
			ret_val = CHR_Error;
			break;
		}
		
		/* DEBUG */
		printf("Command Type:%X, Received data:\n", buffer[3]);
		for (int i=0; i<rd; i++) {
			printf("%X ", local_buffer[i]);
		}
		printf("\n\n");

		if (verifyPacket(local_buffer, COMMAND_COMPLETE)) {
			ret_val = CHR_OK;
			break;
		}

		/* Special case, prevent unnecessary loop */
		if (buffer[3] == SET_BROADCAST_MODE) {
			if (verifyPacket(local_buffer, SENSOR_DATA)) {
				ret_val = CHR_OK;
				break;
			}
		}
	}
	delete local_buffer;
	return ret_val;
}
