#include"SerialPort_cpp.h"

SerialPort::SerialPort(void)
{
	_path_string = (char*)calloc(sizeof(char), PATH_MAX);
}

SerialPort::SerialPort(const char *path)
{
	_path_string = (char*)calloc(sizeof(char), PATH_MAX);
	strcpy(_path_string, path);
}

SerialPort::~SerialPort()
{
	if (_path_string) {
		free(_path_string);
	}
	if (_fd) {
		close(_fd);
	}
}

int SerialPort::OpenPort()
{
	_fd = open(_path_string, O_RDWR | O_NOCTTY | O_NDELAY);
	if (_fd > 0)
		fcntl(_fd, F_SETFL, 0);
	return _fd;
}
	

int SerialPort::OpenPort(const char *path)
{
	_fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY);
	if (_fd > 0) {
		fcntl(_fd, F_SETFL, 0);
		strcpy(_path_string, path);
	}
	return _fd;
}

int SerialPort::ClosePort(void)
{
	if (_fd > 0) {
		close(_fd);
		_fd = 0;
		return 0;
	}
	else
		return -1;
}

int SerialPort::ReOpen(void)
{
	if (_fd > 0) {
		this->ClosePort();
		this->OpenPort(_path_string);
		return _fd;
	}
	return -1;
}

int SerialPort::SetAttr(int baud_value, int vtime, int vmin)
{
	struct termios options;
	tcgetattr(_fd, &options);
	int baud;
	switch(baud_value) {
		case 2400:
			baud = B2400;
			break;
		case 4800:
			baud = B4800;
			break;
		case 9600:
			baud = B9600;
			break;
		case 19200:
			baud = B19200;
			break;
		case 38400:
			baud = B38400;
			break;
		case 57600:
			baud = B57600;
			break;
		case 115200:
			baud = B115200;
			break;
		default:
			fprintf(stderr, "Unsupported Baud Rate!\n");
			return -1;
	}
	cfsetispeed(&options, baud);
	cfsetospeed(&options, baud);
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_lflag = 0;
	options.c_cc[VTIME]	= vtime;
	options.c_cc[VMIN]	= vmin;
	options.c_iflag |= IGNPAR;
	tcsetattr(_fd, TCSANOW, &options);
	return 0;
}

int SerialPort::SetAttr(int baud_value, int vtime, int vmin, int stopbit)
{
	struct termios options;
	tcgetattr(_fd, &options);
	int baud;
	switch(baud_value) {
		case 2400:
			baud = B2400;
			break;
		case 4800:
			baud = B4800;
			break;
		case 9600:
			baud = B9600;
			break;
		case 19200:
			baud = B19200;
			break;
		case 38400:
			baud = B38400;
			break;
		case 57600:
			baud = B57600;
			break;
		case 115200:
			baud = B115200;
			break;
		default:
			fprintf(stderr, "Unsupported Baud Rate!\n");
			return -1;
	}
	cfsetispeed(&options, baud);
	cfsetospeed(&options, baud);
	options.c_cflag |= (CLOCAL | CREAD);
	if (stopbit == 2)
		options.c_cflag |= CSTOPB;
	
	options.c_lflag = 0;
	options.c_cc[VTIME]	= vtime;
	options.c_cc[VMIN]	= vmin;
	options.c_iflag |= IGNPAR;
	tcsetattr(_fd, TCSANOW, &options);
	return 0;
}


int SerialPort::GetFD(void)
{
	return _fd;
}

int SerialPort::GetPath(char *path)
{
	if (path) {
		strcpy(path, _path_string);
		return 0;
	}
	else
		return -1;
}

int SerialPort::WriteData(char *buffer, int num)
{	
	int wr = write(_fd, buffer, num);
	return wr;
}

int SerialPort::WriteData(byte *buffer, int num)
{	
	int wr = write(_fd, buffer, num);
	return wr;
}
	
int SerialPort::ReadData(char *buffer, int num)
{
	int rd = read(_fd, buffer, num);
	return rd;
}

int SerialPort::ReadData(byte *buffer, int num)
{
	int rd = read(_fd, buffer, num);
	return rd;
}
