#pragma once

#include <memory>
#include "opengl/gl_defines.h"
#include "opengl/video_shader.h"
#include "api/video/video_sink_interface.h"
#include "api/video/video_frame.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class GLVideoShader;
class I420TextureCache;

class VideoRenderer :
        public QOpenGLWidget,
        public QOpenGLFunctions,
        public rtc::VideoSinkInterface<webrtc::VideoFrame>,
        public std::enable_shared_from_this<VideoRenderer>
{
    Q_OBJECT

public:
    VideoRenderer(QWidget *parent);

    ~VideoRenderer();

    void init();

protected:
    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

    void OnFrame(const webrtc::VideoFrame& frame) override;

signals:
    void frameArrived(const webrtc::VideoFrame& frame);

private slots:

    void onFrameArrived(const webrtc::VideoFrame& frame);

    void cleanup();

private:
    std::shared_ptr<VideoShader> _videoShader;

    std::shared_ptr<I420TextureCache> _i420TextureCache;

    std::shared_ptr<webrtc::VideoFrame> _cacheFrame;
};
