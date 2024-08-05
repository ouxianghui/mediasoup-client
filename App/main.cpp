#pragma warning(disable:4996)
#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QMetaType>
#include <QOpenGLFunctions>
#include "rtc_base/thread.h"
#include "rtc_base/physical_socket_server.h"

#ifdef WIN32
#include "rtc_base/win32_window.h"
#include "rtc_base/win32_socket_init.h"
//#include "rtc_base/win32_socket_server.h"
#endif // WIN32

#include "mediasoupclient.hpp"
#include "logger/spd_logger.h"
#include "service/core.h"
#include "service/engine.h"

static void registerMetaTypes()
{
    qRegisterMetaType<vi::RoomState>("vi::RoomState");
    qRegisterMetaType<rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>>("rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>");
    qRegisterMetaType<std::shared_ptr<vi::IParticipant>>("std::shared_ptr<vi::IParticipant>");
    qRegisterMetaType<webrtc::VideoFrame*>("const webrtc::VideoFrame*");
}
int main(int argc, char *argv[])
{
    registerMetaTypes();
    
#ifdef WIN32
    rtc::WinsockInitializer winsockInit;
    rtc::PhysicalSocketServer ss;
    rtc::AutoSocketServerThread main_thread(&ss);
    rtc::ThreadManager::Instance()->SetCurrentThread(&main_thread);
#endif
    vi::Core::init();
    QApplication a(argc, argv);
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    getThread("main")->PostTask([]() {
        DLOG("mediasoupclient main::test");
    });
    auto w = std::make_shared<MainWindow>();
    w->init();
    w->show();
    int ret = a.exec();
    vi::Core::destroy();
    return ret;
}
