################################################################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
################################################################

CONFIG += qwt
LIBS += -lqwt -lpthread

TARGET       = AccToPos.cpp

HEADERS = \
    settings.h \
    plot.h \
    CHR_SensorData.h \
    CHR_PacketType.h \
    SerialPort_cpp.h \
    CHR_6dm.h

SOURCES = \
    plot.cpp \
    CHR_SensorData.cpp \
    SerialPort_cpp.cpp \
    CHR_6dm.cpp \
    AccToPos.cpp 

