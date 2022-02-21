QT       += core gui opengl openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += WEBRTC_POSIX WEBRTC_MAC ASIO_STANDALONE GL_SILENCE_DEPRECATION

#QMAKE_INFO_PLIST +=  $${TARGET}/Info.plist
#QMAKE_POST_LINK += sed -i -e "s/@VERSION@/$$VERSION/g" "../Debug/$${TARGET}.app/Contents/Info.plist";

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../RoomClient \
    $$PWD/../RoomClient/client/include \
    $$PWD/../deps/webrtc/include \
    $$PWD/../deps/webrtc/include/third_party \
    $$PWD/../deps/webrtc/include/third_party/abseil-cpp \
    $$PWD/../deps/webrtc/include/third_party/boringssl/src/include \
    $$PWD/../deps/libsdptransform/include \
    $$PWD/../deps/spdlog/include \
    $$PWD/../deps/rapidjson/include \
    $$PWD/../deps/asio/asio/include \
    $$PWD/../deps/websocketpp \
    $$PWD/../deps/libmediasoupclient/include \
    /usr/local/Cellar/glew/2.2.0_1/include

LIBS += -L$$PWD/../deps/webrtc/lib/ -lwebrtc
LIBS += -L/usr/local/Cellar/glew/2.2.0_1/lib/ -lGLEW
LIBS += -framework AudioToolbox -framework CoreAudio -framework AVFoundation -framework CoreMedia -framework CoreVideo

CONFIG(debug, debug | release) {
    DESTDIR = $$PWD/../Debug
    LIBS += -L$$PWD/../Debug/ -lRoomClient
}
eles {
    DESTDIR = $$PWD/../Release
    LIBS += -L$$PWD/../Release/ -lRoomClient
}

SOURCES += \
    app_delegate.cpp \
    gallery_view.cpp \
    gl_renderer.cc \
    mac_video_renderer.cpp \
    main.cpp \
    mainwindow.cpp \
    media_event_adapter.cpp \
    participant_event_adapter.cpp \
    room_event_adapter.cpp \
    video_renderer.cpp

HEADERS += \
    app_delegate.h \
    gallery_view.h \
    gl_renderer.h \
    mac_video_renderer.h \
    mainwindow.h \
    media_event_adapter.h \
    participant_event_adapter.h \
    room_event_adapter.h \
    video_renderer.h

FORMS += \
    gallery_view.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
