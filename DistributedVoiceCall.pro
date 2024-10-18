QT += quick

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        $$PWD/SocketIO/sio_client.cpp \
        $$PWD/SocketIO/sio_socket.cpp \
        $$PWD/SocketIO/internal/sio_client_impl.cpp \
        $$PWD/SocketIO/internal/sio_packet.cpp \
        main.cpp \
        webrtc.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

PATH_TO_LIBDATACHANNEL = E:/CN-1/libdatachannel
INCLUDEPATH += $$PATH_TO_LIBDATACHANNEL/include
LIBS += -L$$PATH_TO_LIBDATACHANNEL/Windows/Mingw64 -ldatachannel.dll
LIBS += -LC:/Qt/Tools/OpenSSLv3/Win_x64/bin -lcrypto-3-x64 -lssl-3-x64
INCLUDEPATH += C:/Qt/Tools/OpenSSLv3/Win_x64/include
LIBS += -lws2_32
LIBS += -lssp



PATH_TO_OPUS = E:/CN-1/opus
INCLUDEPATH += $$PATH_TO_OPUS/include
LIBS += -L$$PATH_TO_OPUS/Windows/Mingw64 -lopus


PATH_TO_SIO = E:/CN-1/socketio-client-cpp
INCLUDEPATH += $$PATH_TO_SIO/lib/websocketpp
INCLUDEPATH += $$PATH_TO_SIO/lib/asio/asio/include
INCLUDEPATH += $$PATH_TO_SIO/lib/rapidjson/include
DEFINES += BOOST_DATE_TIME_NO_LIB
DEFINES += BOOST_REGEX_NO_LIB
DEFINES += ASIO_STANDALONE
DEFINES += _WEBSOCKETPP_CPP11_STL_
DEFINES += _WEBSOCKETPP_CPP11_FUNCTIONAL_
DEFINES += SIO_TLS


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    $$PWD/SocketIO/sio_client.h \
    $$PWD/SocketIO/sio_message.h \
    $$PWD/SocketIO/sio_socket.h \
    $$PWD/SocketIO/internal/sio_client_impl.h \
    $$PWD/SocketIO/internal/sio_packet.h \
    webrtc.h

