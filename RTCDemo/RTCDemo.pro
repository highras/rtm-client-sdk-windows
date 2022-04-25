QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MyEventHandler.cpp \
    QDX12Renderer.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    MyEventHandler.h \
    QDX12Renderer.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    RTCDemo_zh_CN.ts
CONFIG += embed_translations

QMAKE_CFLAGS_DEBUG += -MTd
QMAKE_CXXFLAGS_DEBUG += -MTd
QMAKE_CFLAGS_RELEASE += -MT
QMAKE_CXXFLAGS_RELEASE += -MT



CONFIG(debug, debug|release) {
LIBS += -L..\RTCSDK\x64\Debug\ -lRTCSDK\
        Advapi32.lib
}else{
LIBS += -L..\RTCSDK\x64\Release\ -lRTCSDK\
        Advapi32.lib
}


INCLUDEPATH += ..\..\fpnn-sdk-windows-cpp\fpnn-sdk\core \
                ..\..\fpnn-sdk-windows-cpp\fpnn-sdk\base \
                ..\..\fpnn-sdk-windows-cpp\fpnn-sdk\proto \
                ..\..\fpnn-sdk-windows-cpp\fpnn-sdk\proto\msgpack \
                ..\RTCSDK\DX12Renderer \
                ..\RTCSDK\LivedataAudioStatic \
                ..\RTCSDK\RTCSDK \
                ..\openh264\codec\api\svc \
                ..\libyuv\include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
