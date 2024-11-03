QT += quick
QT += multimedia
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        $$PWD/src/SocketIO/sio_client.cpp \
        $$PWD/src/SocketIO/sio_socket.cpp \
        $$PWD/src/SocketIO/internal/sio_client_impl.cpp \
        $$PWD/src/SocketIO/internal/sio_packet.cpp \
        src/audio/audiooutput.cpp \
        src/audio/audioinput.cpp \
        src/main.cpp \
        src/network/client.cpp \
        src/network/webrtc.cpp


HEADERS += \
    $$PWD/src/SocketIO/sio_client.h \
    $$PWD/src/SocketIO/sio_message.h \
    $$PWD/src/SocketIO/sio_socket.h \
    $$PWD/src/SocketIO/internal/sio_client_impl.h \
    $$PWD/src/SocketIO/internal/sio_packet.h \
    src/network/webrtc.h \
    src/audio/audiooutput.h \
    src/audio/audioinput.h \
    src/network/client.h

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

PATH_TO_LIBDATACHANNEL = C:/cn-files/libdatachannel
INCLUDEPATH += $$PATH_TO_LIBDATACHANNEL/include
LIBS += -L$$PATH_TO_LIBDATACHANNEL/Windows/Mingw64 -ldatachannel.dll
LIBS += -LC:/Qt/Tools/OpenSSLv3/Win_x64/bin -lcrypto-3-x64 -lssl-3-x64
INCLUDEPATH += C:/Qt/Tools/OpenSSLv3/Win_x64/include
LIBS += -lws2_32
QMAKE_LFLAGS += -fuse-ld=lld



PATH_TO_OPUS = C:/cn-files/opus
INCLUDEPATH += $$PATH_TO_OPUS/include
LIBS += -L$$PATH_TO_OPUS/Windows/Mingw64 -lopus

LIBS += -lssp


PATH_TO_SIO = C:/cn-files/socket.io-client-cpp
INCLUDEPATH += $$PATH_TO_SIO/lib/websocketpp
INCLUDEPATH += $$PATH_TO_SIO/lib/asio/asio/include
INCLUDEPATH += $$PATH_TO_SIO/lib/rapidjson/include
DEFINES += BOOST_DATE_TIME_NO_LIB
DEFINES += BOOST_REGEX_NO_LIB
DEFINES += ASIO_STANDALONE
DEFINES += _WEBSOCKETPP_CPP11_STL_
DEFINES += _WEBSOCKETPP_CPP11_FUNCTIONAL_
# DEFINES += SIO_TLS


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG += no_keywords


