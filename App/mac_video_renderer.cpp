/**
 * This file is part of janus_client project.
 * Author:    Jackie Ou
 * Created:   2020-10-01
 **/

#include "mac_video_renderer.h"
#include <thread>
#include <array>
#include <QTimer>
#include "logger/u_logger.h"
#include "absl/types/optional.h"
#include "api/video/video_rotation.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "logger/u_logger.h"

MacVideoRenderer::MacVideoRenderer(QWidget *parent)
    : QOpenGLWidget(parent)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(2, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    this->setFormat(format);
}

MacVideoRenderer::~MacVideoRenderer()
{
    cleanup();
}

void MacVideoRenderer::init()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(this, &MacVideoRenderer::frameArrived, this, &MacVideoRenderer::onFrameArrived);
}

void MacVideoRenderer::initializeGL() 
{
    initializeOpenGLFunctions();

    if (!_inited) {
        GlRenderer::init();
        _inited = true;
    }
}

void MacVideoRenderer::resizeGL(int width, int height)
{
    //if (_cacheFrame) {
    //    if (_videoWidth != _cacheFrame->width() || _videoHeight != _cacheFrame->height() || _width != width|| _height != height) {
    //        _videoWidth = _cacheFrame->width();
    //        _videoHeight = _cacheFrame->height();
    //        _width = width;
    //        _height = height;
    //        GlRenderer::resizeViewport(_videoWidth, _videoHeight, _width, _height);
    //    }
    //}

    GlRenderer::resizeViewport(width, height);
}

void MacVideoRenderer::paintGL()
{
    if (_cacheFrame) {
        vi::GlRenderer::OnFrame(*_cacheFrame);
    }
}

void MacVideoRenderer::OnFrame(const webrtc::VideoFrame& frame)
{
    emit frameArrived(frame);
}

void MacVideoRenderer::onFrameArrived(const webrtc::VideoFrame& frame)
{
    _cacheFrame = std::make_shared<webrtc::VideoFrame>(frame);

    QWidget::update();
}

void MacVideoRenderer::cleanup()
{
    GlRenderer::destroy();
}
