QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += WEBRTC_POSIX WEBRTC_MAC ASIO_STANDALONE GL_SILENCE_DEPRECATION

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
    $$PWD/../deps/libmediasoupclient/include

#INCLUDEPATH += "$$PWD/../deps/webrtc/Frameworks/WebRTC.xcframework/WebRTC.framework/Headers"

LIBS += -L$$PWD/../deps/webrtc/lib/ -lwebrtc
LIBS += -framework AppKit

CONFIG(debug, debug | release) {
    DESTDIR = $$PWD/../Debug
}
eles {
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
    logger/u_logger.cpp \
    opengl/i420_texture_cache.cpp \
    opengl/video_shader.cpp \
    service/base_video_capturer.cc \
    service/component_factory.cpp \
    service/mac_capturer.mm \
    service/media_controller.cpp \
    service/mediasoup_api.cpp \
    service/participant.cpp \
    service/participant_controller.cpp \
    service/room_client.cpp \
    service/signaling_client.cpp \
    service/service_factory.cpp \
    service/windows_capture.cpp \
    utils/bad_any_cast.cc \
    utils/notification_center.cpp \
    utils/notification_keys.cpp \
    utils/sdp_utils.cpp \
    utils/string_utils.cpp \
    utils/task_scheduler.cpp \
    utils/thread_provider.cpp \
    websocket/connection_metadata.cpp \
    websocket/websocket_endpoint.cpp \
    websocket/websocket_transport.cpp

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
    logger/u_logger.h \
    opengl/gl_defines.h \
    opengl/i420_texture_cache.h \
    opengl/video_shader.h \
    service/base_video_capturer.h \
    service/component_factory.h \
    service/i_component_factory.h \
    service/i_media_controller.h \
    service/i_media_controller_observer.h \
    service/i_mediasoup_api.h \
    service/i_participant.h \
    service/i_participant_controller.h \
    service/i_participant_controller_observer.h \
    service/i_room_client.h \
    service/i_room_client_observer.h \
    service/i_signaling_client.h \
    service/i_signaling_observer.h \
    service/mac_capturer.h \
    service/media_controller.h \
    service/mediasoup_api.h \
    service/options.h \
    service/participant.h \
    service/participant_controller.h \
    service/room_client.h \
    service/signaling_client.h \
    service/signaling_models.h \
    service/i_service.hpp \
    service/i_service_factory.hpp \
    service/service_factory.hpp \
    service/windows_capture.h \
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
    websocket/websocket_endpoint.h \
    websocket/websocket_request.h \
    websocket/websocket_transport.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    opengl/RTCNV12TextureCache.m

