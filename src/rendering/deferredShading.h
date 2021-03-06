//-----------------------------------------------------------------------------
// Copyright (c) 2015 Andrew Mac
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------


#ifndef _DEFERRED_SHADING_H_
#define _DEFERRED_SHADING_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx/bgfx.h>
#endif

namespace Rendering 
{
   class RenderCamera;

   class DeferredShading
   {
      protected:
         bool                       mInitialized;
         RenderCamera*              mCamera;

         bgfx::TextureHandle        mGBufferTextures[4];
         bgfx::FrameBufferHandle    mDecalBuffer;
         bgfx::FrameBufferHandle    mLightBuffer;
         bgfx::FrameBufferHandle    mAmbientBuffer;
         bgfx::FrameBufferHandle    mFinalBuffer;
         Graphics::Shader*          mCombineShader; 
         
         Graphics::ViewTableEntry*  mDeferredGeometryView;
         Graphics::ViewTableEntry*  mDeferredDecalView;
         Graphics::ViewTableEntry*  mDeferredLightView;
         Graphics::ViewTableEntry*  mDeferredAmbientView;
         Graphics::ViewTableEntry*  mDeferredFinalView;

         void init();
         void destroy();

      public:
         bgfx::FrameBufferHandle    mGBuffer;
         Graphics::Shader*          mDefaultShader;

         DeferredShading(RenderCamera* camera);
         ~DeferredShading();

         virtual void preRender();
         virtual void render();
         virtual void postRender();
         virtual void resize();
   };
}

#endif