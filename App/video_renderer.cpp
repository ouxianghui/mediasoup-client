/**
 * This file is part of janus_client project.
 * Author:    Jackie Ou
 * Created:   2020-10-01
 **/

#include "opengl/video_shader.h"
#include "opengl/i420_texture_cache.h"
#include "video_renderer.h"
#include <thread>
#include <array>
#include <QTimer>
#include <QLabel>
#include <QFont>
#include "logger/u_logger.h"
#include "absl/types/optional.h"
#include "api/video/video_rotation.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "common_video/include/video_frame_buffer_pool.h"

VideoRenderer::VideoRenderer(QWidget *parent)
    : QOpenGLWidget(parent)
{
    _renderingTimer = new QTimer(this);
    connect(_renderingTimer, SIGNAL(timeout()), this, SLOT(onRendering()));
    _renderingTimer->start(30);
}

VideoRenderer::~VideoRenderer()
{
    cleanup();
}

void VideoRenderer::init()
{
    //setAttribute(Qt::WA_StyledBackground, true);
    //setStyleSheet("background-color:rgb(255, 0, 255)");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void VideoRenderer::destroy()
{

}

void VideoRenderer::clear()
{
    _locked = true;
}

void VideoRenderer::reset()
{
    _locked = false;
}

void VideoRenderer::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &VideoRenderer::cleanup);

    initializeOpenGLFunctions();

    glewInit();

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);

    _i420TextureCache = std::make_shared<I420TextureCache>();
    _i420TextureCache->init();

    _videoShader = std::make_shared<VideoShader>();

    // Set up the rendering context, load shaders and other resources, etc.:
    //QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void VideoRenderer::resizeGL(int w, int h)
{
    // Update projection matrix and other size related settings:
    //if (_frame) {
    //	glViewport(0, 0, _frame->width(), _frame->height());
    //}
    //else {
    //	glViewport(0, 0, 640, 480);
    //}
}

void VideoRenderer::paintGL()
{
    if (_cacheFrame) {

        float imageRatio = (float)_cacheFrame->width() / (float)_cacheFrame->height();
        float canvasRatio = (float)width() / (float)height();

        int32_t viewportX = 0;
        int32_t viewportY = 0;

        int32_t viewportW = 0;
        int32_t viewportH = 0;

        if (canvasRatio >= imageRatio) {
            viewportH = height();
            viewportW = viewportH * imageRatio;
            viewportX = (float)(width() - viewportW) / 2.0f;
        }
        else {
            viewportW = width();
            viewportH = viewportW / imageRatio;
            viewportY = (float)(height() - viewportH) / 2.0f;
        }

        glViewport(viewportX*devicePixelRatio(), viewportY*devicePixelRatio(), viewportW*devicePixelRatio(), viewportH*devicePixelRatio());
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    std::shared_ptr<webrtc::VideoFrame> frame;

    if (_frameQ.try_dequeue(frame)) {
        _cacheFrame = frame;
    }

    if (!_locked && _cacheFrame) {
        _i420TextureCache->uploadFrameToTextures(*_cacheFrame);
        _videoShader->applyShadingForFrame(_cacheFrame->width(),
            _cacheFrame->height(),
            _cacheFrame->rotation(),
            _i420TextureCache->yTexture(),
            _i420TextureCache->uTexture(),
            _i420TextureCache->vTexture());
    }
}

void VideoRenderer::OnFrame(const webrtc::VideoFrame& frame)
{
    auto videeoFrame = std::make_shared<webrtc::VideoFrame>(frame);

    if (_frameQ.size_approx() >= 300) {
        for (int i = 0; i < 100; ++i) {
            std::shared_ptr<webrtc::VideoFrame> dropFrame;
            if (_frameQ.try_dequeue(dropFrame)) {
                DLOG("drop frame .");
            }
        }
    }
    _frameQ.enqueue(videeoFrame);
    //static int counter = 0;
    //DLOG("--> frame: {}", ++counter);
}

void VideoRenderer::onRendering()
{
    QWidget::update();
}

void VideoRenderer::cleanup()
{
    makeCurrent();

    _i420TextureCache = nullptr;
    _videoShader = nullptr;

    doneCurrent();
}

void VideoRenderer::resizeEvent(QResizeEvent *e)
{
    QOpenGLWidget::resizeEvent(e);
}
