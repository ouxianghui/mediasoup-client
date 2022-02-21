/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "gl_defines.h"
#include "api/video/video_frame.h"

// Two sets of 3 textures are used here, one for each of the Y, U and V planes. Having two sets
// alleviates CPU blockage in the event that the GPU is asked to render to a texture that is already
// in use.
static const GLsizei kNumTextureSets = 2;
static const GLsizei kNumTexturesPerSet = 3;
static const GLsizei kNumTextures = kNumTexturesPerSet * kNumTextureSets;

class I420TextureCache {
public:
    I420TextureCache();

    ~I420TextureCache();

    void init();

    void uploadFrameToTextures(const webrtc::VideoFrame& frame);

    GLuint yTexture();

    GLuint uTexture();

    GLuint vTexture();

protected:
    void setupTextures();

    void uploadPlane(const uint8_t *plane, GLuint texture, size_t width, size_t height, int32_t stride);

private:
    bool _hasUnpackRowLength = false;

    GLint _currentTextureSet = 0;

    // Handles for OpenGL constructs.
    GLuint _textures[kNumTextures];

    // Used to create a non-padded plane for GPU upload when we receive padded frames.
    std::vector<uint8_t> _planeBuffer;
};
