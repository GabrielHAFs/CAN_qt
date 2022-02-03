TEMPLATE = app

TARGET = Qt_isobus

HEADERS = Qt_isobus.h Backend.h

SOURCES = Qt_isobus.cpp Backend.cpp

DESTDIR = $$(QMAKE_DESTIDIR)

QT+= core quick

RESOURCES += qml.qrc
