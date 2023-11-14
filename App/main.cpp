#include "mainwindow.h"
#include <QMetaType>
#include <QApplication>

#include <QOpenGLFunctions>
#include "rtc_base/thread.h"
#include "rtc_base/physical_socket_server.h"

#ifdef WIN32
#include "rtc_base/win32_window.h"
#include "rtc_base/win32_socket_init.h"
#include "rtc_base/win32_socket_server.h"
#endif

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
    rtc::Win32SocketServer ss;
    rtc::Win32Thread mainThread(&ss);
    rtc::ThreadManager::Instance()->SetCurrentThread(&mainThread);
#endif

    vi::Core::init();

    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    getThread("main")->PostTask([](){
        DLOG("mediasoupclient main: test");
    });

    auto w = std::make_shared<MainWindow>();
    w->init();
    w->show();

    int ret = a.exec();

    vi::Core::destroy();


    return ret;
    //return 0;
}
