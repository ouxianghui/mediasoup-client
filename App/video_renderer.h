
#pragma once

#include <memory>
#include "api/video/video_sink_interface.h"
#include "api/video/video_frame.h"
#include <QTimer>
#include <mutex>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class VideoShader;
class I420TextureCache;

class VideoRenderer
    : public QOpenGLWidget
    , public QOpenGLFunctions
    , public rtc::VideoSinkInterface<webrtc::VideoFrame>
    , public std::enable_shared_from_this<VideoRenderer>
{
    Q_OBJECT

public:
    VideoRenderer(QWidget *parent);

    ~VideoRenderer();

    void init();

    void destroy();

    void clear();

    void reset();

protected:
    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

    void OnFrame(const webrtc::VideoFrame& frame) override;

    void resizeEvent(QResizeEvent *e) override;

Q_SIGNALS:
    void draw(std::shared_ptr<webrtc::VideoFrame> frame);

private Q_SLOTS:
    void cleanup();

    void onDraw(std::shared_ptr<webrtc::VideoFrame> frame);

private:
    std::shared_ptr<VideoShader> _videoShader;

    std::shared_ptr<I420TextureCache> _i420TextureCache;

    std::shared_ptr<webrtc::VideoFrame> _cacheFrame;

    bool _locked = false;

    //FILE* _fp = nullptr;
};
