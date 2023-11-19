QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


#DEFINES += _WEBSOCKETPP_CPP11_INTERNAL_

QMAKE_MACOSX_DEPLOYMENT_TARGET=10.15

INCLUDEPATH += \
    $$PWD/../deps/webrtc/include \
    $$PWD/../deps/webrtc/include/third_party \
    $$PWD/../deps/webrtc/include/third_party/abseil-cpp \
    $$PWD/../deps/webrtc/include/third_party/boringssl/src/include \
    $$PWD/../deps/webrtc/include/sdk/objc \
    $$PWD/../deps/webrtc/include/sdk/objc/base \
    $$PWD/../deps/libsdptransform/include \
    $$PWD/../deps/rapidjson/include \
    $$PWD/../deps/spdlog/include \
    $$PWD/../deps/asio/asio/include \
    $$PWD/../deps/websocketpp \
    $$PWD/../deps/libmediasoupclient/include \
    $$PWD/../deps

win: {
    INCLUDEPATH += $$PWD/../deps/glew/include
    INCLUDEPATH += $$PWD/../deps/cpr/include
}

unix: {
    INCLUDEPATH += /usr/local/Cellar/glew/2.2.0_1/include
    INCLUDEPATH += /usr/local/Cellar/cpr/1.10.5/include
}

#INCLUDEPATH += "$$PWD/../deps/webrtc/Frameworks/WebRTC.xcframework/WebRTC.framework/Headers"

#LIBS += $$PWD/../deps/webrtc/lib/windows_debug_x64/webrtc.lib
#LIBS += $$PWD/../deps/glew/lib/Release/x64/glew32.lib

#LIBS += -framework AppKit

CONFIG(debug, debug | release) {
    DESTDIR = $$PWD/../Debug
} else {
    DESTDIR = $$PWD/../Release
}

SOURCES += \
    ../deps/libmediasoupclient/src/Consumer.cpp \
    ../deps/libmediasoupclient/src/DataConsumer.cpp \
    ../deps/libmediasoupclient/src/DataProducer.cpp \
    ../deps/libmediasoupclient/src/Device.cpp \
    ../deps/libmediasoupclient/src/Handler.cpp \
    ../deps/libmediasoupclient/src/Logger.cpp \
    ../deps/libmediasoupclient/src/PeerConnection.cpp \
    ../deps/libmediasoupclient/src/Producer.cpp \
    ../deps/libmediasoupclient/src/Transport.cpp \
    ../deps/libmediasoupclient/src/mediasoupclient.cpp \
    ../deps/libmediasoupclient/src/ortc.cpp \
    ../deps/libmediasoupclient/src/scalabilityMode.cpp \
    ../deps/libmediasoupclient/src/sdp/MediaSection.cpp \
    ../deps/libmediasoupclient/src/sdp/RemoteSdp.cpp \
    ../deps/libmediasoupclient/src/sdp/Utils.cpp \
    ../deps/libsdptransform/src/grammar.cpp \
    ../deps/libsdptransform/src/parser.cpp \
    ../deps/libsdptransform/src/writer.cpp \
    logger/rtc_log_sink.cpp \
    logger/spd_logger.cpp \
    network/network_http_client.cpp \
    network/network_request_consumer.cpp \
    network/network_request_executor.cpp \
    network/network_request_manager.cpp \
    network/network_request_plugin.cpp \
    network/network_request_task.cpp \
    network/network_status_detector.cpp \
    opengl/i420_texture_cache.cpp \
    opengl/video_shader.cpp \
    service/base_video_capturer.cc \
    service/broadcaster.cpp \
    service/core.cpp \
    service/engine.cpp \
    service/component_factory.cpp \
    service/media_controller.cpp \
    service/mediasoup_api.cpp \
    service/participant.cpp \
    service/participant_controller.cpp \
    service/room_client.cpp \
    service/rtc_context.cpp \
    service/signaling_client.cpp \
    service/service_factory.cpp \
    service/signaling_models.cpp \
    utils/bad_any_cast.cc \
    utils/notification_center.cpp \
    utils/notification_keys.cpp \
    utils/sdp_utils.cpp \
    utils/string_utils.cpp \
    utils/task_scheduler.cpp \
    utils/thread_provider.cpp \
    websocket/tls_websocket_endpoint.cpp \
    websocket/websocket_endpoint.cpp

win: {
    SOURCES += service/windows_capture.cpp
}

unix: {
    SOURCES += service/mac_capturer.mm
}

HEADERS += \
    ../deps/libmediasoupclient/include/Consumer.hpp \
    ../deps/libmediasoupclient/include/DataConsumer.hpp \
    ../deps/libmediasoupclient/include/DataProducer.hpp \
    ../deps/libmediasoupclient/include/Device.hpp \
    ../deps/libmediasoupclient/include/Handler.hpp \
    ../deps/libmediasoupclient/include/Logger.hpp \
    ../deps/libmediasoupclient/include/MediaSoupClientErrors.hpp \
    ../deps/libmediasoupclient/include/PeerConnection.hpp \
    ../deps/libmediasoupclient/include/Producer.hpp \
    ../deps/libmediasoupclient/include/Transport.hpp \
    ../deps/libmediasoupclient/include/Utils.hpp \
    ../deps/libmediasoupclient/include/mediasoupclient.hpp \
    ../deps/libmediasoupclient/include/ortc.hpp \
    ../deps/libmediasoupclient/include/scalabilityMode.hpp \
    ../deps/libmediasoupclient/include/sdp/MediaSection.hpp \
    ../deps/libmediasoupclient/include/sdp/RemoteSdp.hpp \
    ../deps/libmediasoupclient/include/sdp/Utils.hpp \
    ../deps/libmediasoupclient/include/version.hpp \
    ../deps/libsdptransform/include/json.hpp \
    ../deps/libsdptransform/include/sdptransform.hpp \
    json/jsonable.hpp \
    json/serialization.hpp \
    json/string_algo.hpp \
    json/stringable.hpp \
    logger/rtc_log_sink.h \
    logger/spd_logger.h \
    network/i_network_request_manager.h \
    network/network.hpp \
    network/network_http_client.h \
    network/network_request_builder.h \
    network/network_request_consumer.h \
    network/network_request_executor.h \
    network/network_request_manager.h \
    network/network_request_plugin.h \
    network/network_request_task.h \
    network/network_status_detector.h \
    opengl/gl_defines.h \
    opengl/i420_texture_cache.h \
    opengl/video_shader.h \
    service/base_video_capturer.h \
    service/broadcaster.hpp \
    service/core.h \
    service/engine.h \
    service/component_factory.h \
    service/i_media_controller.h \
    service/i_media_event_handler.h \
    service/i_mediasoup_api.h \
    service/i_participant.h \
    service/i_participant_controller.h \
    service/i_participant_event_handler.h \
    service/i_room_client.h \
    service/i_room_client_event_handler.h \
    service/i_signaling_client.h \
    service/i_signaling_event_handler.h \
    service/media_controller.h \
    service/mediasoup_api.h \
    service/options.h \
    service/participant.h \
    service/participant_controller.h \
    service/room_client.h \
    service/rtc_context.hpp \
    service/signaling_client.h \
    service/signaling_models.h \
    service/i_service.hpp \
    service/service_factory.hpp \
    utils/container.hpp \
    utils/i_notification.h \
    utils/i_observer.hpp \
    utils/interface_proxy.hpp \
    utils/notification_center.hpp \
    utils/notification_keys.hpp \
    utils/notifications.hpp \
    utils/object_factory.hpp \
    utils/observable.h \
    utils/observer.hpp \
    utils/sdp_utils.h \
    utils/singleton.h \
    utils/string_utils.h \
    utils/task_scheduler.h \
    utils/thread_provider.h \
    utils/universal_observable.hpp \
    websocket/connection_metadata.h \
    websocket/i_connection_observer.h \
    websocket/i_transport.h \
    websocket/i_transport_observer.h \
    websocket/tls_websocket_endpoint.h \
    websocket/websocket_endpoint.h \
    websocket/websocket_request.h \
    websocket/websocket_transport.h


win: {
    HEADERS += service/windows_capture.h
}

unix: {
    HEADERS += service/mac_capturer.h
}

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

mac {
    DEFINES += WEBRTC_MAC
    DEFINES += WEBRTC_POSIX
    DEFINES += ABSL_ALLOCATOR_NOTHROW=1
    DEFINES += ASIO_STANDALONE

    LIBS += -L/usr/local/Cellar/glew/2.2.0_1/lib/ -lGLEW
    LIBS += -framework AudioToolbox -framework CoreAudio -framework AVFoundation -framework CoreMedia -framework CoreVideo

    DISTFILES += \
        opengl/RTCNV12TextureCache.m
}

win32 {
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

    CONFIG(debug, debug | release) {
#        QMAKE_CXXFLAGS_DEBUG = /MTd /Zi
    } else {
#        QMAKE_CXXFLAGS_RELEASE = /MT
    }
}

linux {

}

