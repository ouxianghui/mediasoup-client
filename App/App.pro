QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += GL_SILENCE_DEPRECATION
DEFINES += UNICODE
DEFINES += _UNICODE
DEFINES += WIN32
DEFINES += _ENABLE_EXTENDED_ALIGNED_STORAGE
DEFINES += WIN64
DEFINES += BUILD_STATIC
DEFINES += USE_AURA=1
DEFINES += NO_TCMALLOC
DEFINES += FULL_SAFE_BROWSING
DEFINES += SAFE_BROWSING_CSD
DEFINES += SAFE_BROWSING_DB_LOCAL
DEFINES += CHROMIUM_BUILD
DEFINES += _HAS_EXCEPTIONS=0
DEFINES += __STD_C
DEFINES += _CRT_RAND_S
DEFINES += _CRT_SECURE_NO_DEPRECATE
DEFINES += _SCL_SECURE_NO_DEPRECATE
DEFINES += _ATL_NO_OPENGL
DEFINES += CERT_CHAIN_PARA_HAS_EXTRA_FIELDS
DEFINES += PSAPI_VERSION=2
DEFINES += _SECURE_ATL
DEFINES += _USING_V110_SDK71_
DEFINES += WINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP
DEFINES += WIN32_LEAN_AND_MEAN
DEFINES += NOMINMAX
DEFINES += NTDDI_VERSION=NTDDI_WIN10_RS2
DEFINES += _WIN32_WINNT=0x0A00
DEFINES += WINVER=0x0A00
DEFINES += DYNAMIC_ANNOTATIONS_ENABLED=1
DEFINES += WTF_USE_DYNAMIC_ANNOTATIONS=1
DEFINES += WEBRTC_ENABLE_PROTOBUF=1
DEFINES += WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
DEFINES += RTC_ENABLE_VP9
DEFINES += HAVE_SCTP
DEFINES += WEBRTC_USE_H264
DEFINES += WEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=0
DEFINES += WEBRTC_WIN
DEFINES += ABSL_ALLOCATOR_NOTHROW=1
DEFINES += HAVE_WEBRTC_VIDEO
DEFINES += HAVE_WEBRTC_VOICE
DEFINES += ASIO_STANDALONE
#DEFINES += _WEBSOCKETPP_CPP11_INTERNAL_

#QMAKE_INFO_PLIST +=  $${TARGET}/Info.plist
#QMAKE_POST_LINK += sed -i -e "s/@VERSION@/$$VERSION/g" "../Debug/$${TARGET}.app/Contents/Info.plist"

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
    $$PWD/../deps/glew/include \
    $$PWD/../deps/concurrentqueue \
    $$PWD/../deps/cpr/include
#/usr/local/Cellar/glew/2.2.0_1/include

LIBS += $$PWD/../deps/glew/lib/Release/x64/glew32.lib
LIBS += winmm.lib Advapi32.lib comdlg32.lib dbghelp.lib dnsapi.lib gdi32.lib msimg32.lib odbc32.lib odbccp32.lib oleaut32.lib shell32.lib shlwapi.lib user32.lib usp10.lib uuid.lib version.lib wininet.lib winmm.lib winspool.lib ws2_32.lib delayimp.lib kernel32.lib ole32.lib crypt32.lib iphlpapi.lib secur32.lib dmoguids.lib wmcodecdspuuid.lib amstrmid.lib msdmo.lib strmiids.lib opengl32.lib glu32.lib

#LIBS += -L/usr/local/Cellar/glew/2.2.0_1/lib/ -lGLEW
#LIBS += -framework AudioToolbox -framework CoreAudio -framework AVFoundation -framework CoreMedia -framework CoreVideo

CONFIG(debug, debug | release) {
    DESTDIR = $$PWD/../Debug
    LIBS += $$PWD/../deps/webrtc/lib/windows_debug_x64/webrtc.lib
    LIBS += $$PWD/../deps/cpr/debug/lib/cpr.lib
    LIBS += $$PWD/../deps/cpr/debug/lib/libcurl-d.lib
    LIBS += $$PWD/../deps/cpr/debug/lib/zlibd.lib
    LIBS += $$PWD/../Debug/RoomClient.lib
    QMAKE_CXXFLAGS_DEBUG = /MTd /Zi
} else {
    DESTDIR = $$PWD/../Release
    LIBS += $$PWD/../deps/webrtc/lib/windows_release_x64/webrtc.lib
    LIBS += $$PWD/../deps/cpr/debug/lib/cpr.lib
    LIBS += $$PWD/../deps/cpr/debug/lib/libcurl.lib
    LIBS += $$PWD/../deps/cpr/debug/lib/zlib.lib
    LIBS += $$PWD/../Release/RoomClient.lib
    QMAKE_CXXFLAGS_RELEASE = /MT
}

SOURCES += \
    gallery_view.cpp \
    main.cpp \
    mainwindow.cpp \
    participant_item_view.cpp \
    participant_list_view.cpp \
    table.cpp \
    video_renderer.cpp

HEADERS += \
    gallery_view.h \
    mainwindow.h \
    participant_item_view.h \
    participant_list_view.h \
    table.h \
    video_renderer.h

FORMS += \
    gallery_view.ui \
    mainwindow.ui \
    participant_item_view.ui \
    participant_list_view.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
