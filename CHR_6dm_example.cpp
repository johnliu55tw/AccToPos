#include"CHR_6dm.h"

using namespace CHR;

int main(void)
{
	CHR_6dm device("/dev/ttyUSB0");
	if (device.open(20) != CHR_OK) {
		fprintf(stderr, "Error Open");
		return -1;
	}
	printf("Open complete\n");

	SensorData sensor;
	sensor.enableChannel(az);
	sensor.enableChannel(ax);

	printf("Now set active channel\n");	
	if (device.setActiveChannels(&sensor) != CHR_OK)
	{
		fprintf(stderr, "Error Open");
		return -1;
	}

	printf("Now goto measurement mode\n");
	if (device.gotoMeasurementMode(&sensor, 300) != CHR_OK) {
		fprintf(stderr, "Error goto measurement mode\n");
		return -1;
	}

	for (int i=0; ; i++) {
		device.getData(&sensor);
		/*
		printf("Yaw:%f Pitch:%f Roll:%f\n", sensor.datafield.yaw(),
						    sensor.datafield.pitch(),
						    sensor.datafield.roll());
		*/
		printf("Acc_x:%3.4f Acc_y:%3.4f Acc_z:%3.4f\n", sensor.datafield.accel_x(),
						       sensor.datafield.accel_y(),
						       sensor.datafield.accel_z());
		printf("Mag_x:%3.4f Mag_y:%3.4f Mag_z:%3.4f\n", sensor.datafield.mag_x(),
						       sensor.datafield.mag_y(),
						       sensor.datafield.mag_z());
		printf("Gyro_x:%3.4f Gyro_y:%3.4f Gyro_z:%3.4f\n", sensor.datafield.gyro_x(),
						       sensor.datafield.gyro_y(),
						       sensor.datafield.gyro_z());
		usleep(100000);
	}

	printf("Go to config mode again!\n");
	if (device.gotoConfigMode() != CHR_OK) {
		fprintf(stderr, "Error goto config mode\n");
		return -1;
	}

	return 0;
}


		
