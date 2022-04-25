QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MyEventHandler.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    MyEventHandler.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

CONFIG(debug, debug|release) {
QMAKE_CFLAGS_DEBUG += -MTd
QMAKE_CXXFLAGS_DEBUG += -MTd
LIBS += ../RTCSDK/x64/Debug/RTCSDK.lib \
    Advapi32.lib \
    Ole32.lib
}else{
QMAKE_CFLAGS_RELEASE += -MT
QMAKE_CXXFLAGS_RELEASE += -MT
LIBS += ../RTCSDK/x64/Release/RTCSDK.lib \
    Advapi32.lib \
    Ole32.lib
}


INCLUDEPATH += ../../fpnn-sdk-windows-cpp/fpnn-sdk/core \
                ../../fpnn-sdk-windows-cpp/fpnn-sdk/base \
                ../../fpnn-sdk-windows-cpp/fpnn-sdk/proto \
                ../../fpnn-sdk-windows-cpp/fpnn-sdk/proto/msgpack \
                ../../fpnn-sdk-windows-cpp\fpnn-sdk\proto\rapidjson \
                ../RTCSDK/RTCSDK \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
