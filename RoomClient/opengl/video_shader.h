/*
 *  Copyright 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#pragma once
#include "gl_defines.h"
#include "api/video/video_rotation.h"
#include "absl/types/optional.h"

class VideoShader {
public:
    VideoShader();

    ~VideoShader();

    /** Callback for I420 frames. Each plane is given as a texture. */
    void applyShadingForFrame(int width, int height, webrtc::VideoRotation rotation, GLuint yPlane, GLuint uPlane, GLuint vPlane);

    /** Callback for NV12 frames. Each plane is given as a texture. */
    void applyShadingForFrame(int width, int height, webrtc::VideoRotation rotation, GLuint yPlane, GLuint uvPlane);

protected:
    GLuint createShader(GLenum type, const GLchar* source);
    GLuint createProgram(GLuint vertexShader, GLuint fragmentShader);
    GLuint createProgramFromFragmentSource(const char fragmentShaderSource[]);
    bool createVertexBuffer(GLuint* vertexBuffer, GLuint* vertexArray);
    void setVertexData(webrtc::VideoRotation rotation);

    bool createAndSetupI420Program();
    bool createAndSetupNV12Program();
    bool prepareVertexBufferWithRotation(webrtc::VideoRotation rotation);

private:
    GLuint _vertexBuffer = 0;
    GLuint _vertexArray = 0;

    // Store current rotation and only upload new vertex data when rotation changes.
    absl::optional<webrtc::VideoRotation> _currentRotation;

    GLuint _i420Program = 0;
    GLuint _nv12Program = 0;
};
