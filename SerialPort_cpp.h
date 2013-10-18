#ifndef SERIALPORT_H
#define SERIALPORT_H 1

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<termios.h>
#include<fcntl.h>
#include<limits.h>
#include<string.h>

typedef unsigned char byte;

class SerialPort {
public:
	SerialPort(void);
	SerialPort(const char *path);

	~SerialPort();
	
	int OpenPort();
	int OpenPort(const char *path);
	int ClosePort(void);
	int ReOpen(void);

	int SetAttr(int baud_value, int vtime, int vmin);
	int SetAttr(int baud_value, int vtime, int vmin, int stopbit);
	int GetFD(void);
	int GetPath(char *path);

	int WriteData(char *buffer, int num);
	int WriteData(byte *buffer, int num);
	int ReadData(char *buffer, int num);
	int ReadData(byte *buffer, int num);


private:
	int _fd;
	char *_path_string;
};

#endif
