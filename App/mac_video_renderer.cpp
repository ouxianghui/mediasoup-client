/**
 * This file is part of mediasoup_client project.
 * Author:    Jackie Ou
 * Created:   2021-11-01
 **/

#include "mac_video_renderer.h"
#include <thread>
#include <array>
#include <QTimer>
//#include "logger/u_logger.h"
#include "absl/types/optional.h"
#include "api/video/video_rotation.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
//#include "logger/u_logger.h"
#include "rtc_base/checks.h"

MacVideoRenderer::MacVideoRenderer(QWidget *parent)
    : QOpenGLWidget(parent)
    , _inited(false), _buffer(NULL), _width(0), _height(0)
{

}

MacVideoRenderer::~MacVideoRenderer()
{

}

void MacVideoRenderer::init()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(this, &MacVideoRenderer::frameArrived, this, &MacVideoRenderer::onFrameArrived);
}

void MacVideoRenderer::destroy()
{
    disconnect(this, &MacVideoRenderer::frameArrived, this, &MacVideoRenderer::onFrameArrived);

    if (!_inited) {
        return;
    }

    _inited = false;

    delete[] _buffer;
    _buffer = NULL;

    glDeleteTextures(1, &_texture);
}

void MacVideoRenderer::initializeGL() 
{
    initializeOpenGLFunctions();

    if (!_inited) {
        RTC_DCHECK(!_inited);
        _inited = true;
        glGenTextures(1, &_texture);
    }
}

void MacVideoRenderer::resizeGL(int /*width*/, int /*height*/)
{
    resizeViewport();
}

void MacVideoRenderer::paintGL()
{
    RTC_DCHECK(_inited);

    if (!_cacheFrame) {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    if (_cacheFrame->width() != _width || _cacheFrame->height() != _height) {
        resizeVideo(_cacheFrame->width(), _cacheFrame->height());
    }

    resizeViewport();

    webrtc::ConvertFromI420(*_cacheFrame, webrtc::VideoType::kBGRA, 0, _buffer);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, _buffer);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
}

void MacVideoRenderer::OnFrame(const webrtc::VideoFrame& frame)
{
    Q_EMIT frameArrived(frame);
}

void MacVideoRenderer::onFrameArrived(const webrtc::VideoFrame& frame)
{
    _cacheFrame = std::make_shared<webrtc::VideoFrame>(frame);

    QWidget::update();
}

void MacVideoRenderer::resizeViewport() {

    // TODO(pbos): Aspect ratio, letterbox the video.
    //glViewport(0, 0, width, height);

    if (_cacheFrame) {
        float imageRatio = (float)_cacheFrame->width() / (float)_cacheFrame->height();
        float canvaRatio = (float)width() / (float)height();
        float viewportX = 0, viewportY = 0, viewportW = 0, viewportH = 0;

        if (canvaRatio >= imageRatio) {
            viewportH = height();
            viewportW = viewportH * imageRatio;
            viewportX = (float)(width() - viewportW) / 2.0f;
        }
        else {
            viewportW = width();
            viewportH = viewportW / imageRatio;
            viewportY = (float)(height() - viewportH) / 2.0f;
        }

        glViewport(viewportX*window()->devicePixelRatio(), viewportY*window()->devicePixelRatio(), viewportW*window()->devicePixelRatio(), viewportH*window()->devicePixelRatio());
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glOrtho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
}

void MacVideoRenderer::resizeVideo(size_t width, size_t height) {
    RTC_DCHECK(_inited);
    _width = width;
    _height = height;

    _bufferSize = width * height * 4;  // BGRA

    delete[] _buffer;
    _buffer = new uint8_t[_bufferSize];
    RTC_DCHECK(_buffer);
    memset(_buffer, 0, _bufferSize);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, static_cast<GLvoid*>(_buffer));
}
