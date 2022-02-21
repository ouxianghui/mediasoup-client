#include "video_renderer.h"
#include <thread>
#include <array>
#include "opengl/video_shader.h"
#include "opengl/i420_texture_cache.h"
#include "absl/types/optional.h"
#include "api/video/video_rotation.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "logger/u_logger.h"

VideoRenderer::VideoRenderer(QWidget *parent)
    : QOpenGLWidget(parent)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    this->setFormat(format);
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
    connect(this, &VideoRenderer::frameArrived, this, &VideoRenderer::onFrameArrived);
}

void VideoRenderer::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &VideoRenderer::cleanup);

    initializeOpenGLFunctions();

    //glewInit();

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);

    _i420TextureCache = std::make_shared<I420TextureCache>();
    _i420TextureCache->init();

    _videoShader = std::make_shared<VideoShader>();

    // Set up the rendering context, load shaders and other resources, etc.:
    //QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
     makeCurrent();
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

        glViewport(viewportX, viewportY, viewportW, viewportH);
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    if (_cacheFrame) {

        static int counter = 0;
        DLOG("--> frame: {}, ts: {}", ++counter, _cacheFrame->timestamp_us());
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
    emit frameArrived(frame);
}

void VideoRenderer::onFrameArrived(const webrtc::VideoFrame& frame)
{
    _cacheFrame = std::make_shared<webrtc::VideoFrame>(frame);

    QWidget::update();
}

void VideoRenderer::cleanup()
{
    makeCurrent();

    _i420TextureCache = nullptr;
    _videoShader = nullptr;

    doneCurrent();
}
