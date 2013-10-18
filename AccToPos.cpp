#include <qapplication.h>
#include <QVBoxLayout>
#include "plot.h"
#include "settings.h"
#include "CHR_6dm.h"

pthread_mutex_t mutex_pos;
double Pos;
double acc_callback(void);
double pos_callback(void);
void *calc_thread(void *ptr);

CHR_6dm device("/dev/ttyUSB0");
CHR_SensorData data;

int main(int argc, char *argv[])
{
	if (device.open(20) != CHR_OK) {
		fprintf(stderr, "Error Open");
		return -1;
	}

	sensor.enableChannel(Accelerometer);
	if (device.setActiveChannels(&sensor) != CHR_OK)
	{
		fprintf(stderr, "Error set channels!\n");
		return -1;
	}

	if (device.gotoMeasurementMode(&sensor, 300) != CHR_OK) {
		fprintf(stderr, "Error set measurement mode!\n");
		return -1;
	}

	pthread_t thread;
	int ret = pthread_create(&thread, NULL, calc_thread, NULL);
	if (ret < 0) {
		fprintf(stderr, "Error create thread");
		return -1;
	}

	QApplication a(argc, argv);




