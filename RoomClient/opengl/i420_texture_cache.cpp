/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "i420_texture_cache.h"

#if TARGET_OS_IPHONE
#include <OpenGLES/ES3/gl.h>
#else
#include <OpenGL/gl3.h>
#endif

#include "api/video/video_frame_buffer.h"
#include "api/video/i420_buffer.h"

I420TextureCache::I420TextureCache()
{

}

I420TextureCache::~I420TextureCache()
{
    glDeleteTextures(kNumTextures, _textures);
}

GLuint I420TextureCache::yTexture() {
    return _textures[_currentTextureSet * kNumTexturesPerSet];
}

GLuint I420TextureCache::uTexture() {
    return _textures[_currentTextureSet * kNumTexturesPerSet + 1];
}

GLuint I420TextureCache::vTexture() {
    return _textures[_currentTextureSet * kNumTexturesPerSet + 2];
}

void I420TextureCache::init() {
    //  if (self = [super init]) {
    //#if TARGET_OS_IPHONE
    //    _hasUnpackRowLength = (context.API == kEAGLRenderingAPIOpenGLES3);
    //#else
    _hasUnpackRowLength = true;
    //#endif
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    setupTextures();
    //  }
    //  return self;
}

void I420TextureCache::setupTextures() {
    glGenTextures(kNumTextures, _textures);
    // Set parameters for each of the textures we created.
    for (GLsizei i = 0; i < kNumTextures; i++) {
        glBindTexture(GL_TEXTURE_2D, _textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

void I420TextureCache::uploadPlane(const uint8_t *plane, GLuint texture, size_t width, size_t height, int32_t stride) {
    glBindTexture(GL_TEXTURE_2D, texture);

    const uint8_t *uploadPlane = plane;
    if ((size_t)stride != width) {
        if (_hasUnpackRowLength) {
            // GLES3 allows us to specify stride.
            glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         RTC_PIXEL_FORMAT,
                         static_cast<GLsizei>(width),
                         static_cast<GLsizei>(height),
                         0,
                         RTC_PIXEL_FORMAT,
                         GL_UNSIGNED_BYTE,
                         uploadPlane);
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            return;
        } else {
            // Make an unpadded copy and upload that instead. Quick profiling showed
            // that this is faster than uploading row by row using glTexSubImage2D.
            uint8_t *unpaddedPlane = _planeBuffer.data();
            for (size_t y = 0; y < height; ++y) {
                memcpy(unpaddedPlane + y * width, plane + y * stride, width);
            }
            uploadPlane = unpaddedPlane;
        }
    }
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 RTC_PIXEL_FORMAT,
                 static_cast<GLsizei>(width),
                 static_cast<GLsizei>(height),
                 0,
                 RTC_PIXEL_FORMAT,
                 GL_UNSIGNED_BYTE,
                 uploadPlane);
}

void I420TextureCache::uploadFrameToTextures(const webrtc::VideoFrame& frame) {
    _currentTextureSet = (_currentTextureSet + 1) % kNumTextureSets;

    rtc::scoped_refptr<webrtc::VideoFrameBuffer> vfb = frame.video_frame_buffer();
    if (!vfb) {
        return;
    }

    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer = vfb->ToI420();

    const int chromaWidth = buffer->ChromaWidth();
    //const int chromaHeight = buffer->ChromaHeight();
    if (buffer->StrideY() != frame.width() ||
            buffer->StrideU() != chromaWidth ||
            buffer->StrideV() != chromaWidth) {
        _planeBuffer.resize(buffer->width() * buffer->height());
    }

    uploadPlane(buffer->DataY(), yTexture(), buffer->width(), buffer->height(),  buffer->StrideY());

    uploadPlane(buffer->DataU(), uTexture(), buffer->ChromaWidth(), buffer->ChromaHeight(), buffer->StrideU());

    uploadPlane(buffer->DataV(), vTexture(), buffer->ChromaWidth(), buffer->ChromaHeight(), buffer->StrideV());
}

