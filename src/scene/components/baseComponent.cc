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

#include "baseComponent.h"
#include "scene/object.h"

// Script bindings.
#include "baseComponent_Binding.h"

// bgfx/bx
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

namespace Scene 
{
   IMPLEMENT_CONOBJECT(BaseComponent);

   BaseComponent::BaseComponent()
      : mOwnerObject(NULL)
   {
      mTypeString = "Base";
      mBoundingBox.minExtents.set(0, 0, 0);
      mBoundingBox.maxExtents.set(0, 0, 0);
      mScale.set(1.0f, 1.0f, 1.0f);
      mPosition.set(0.0f, 0.0f, 0.0f);
      mRotation.set(0.0f, 0.0f, 0.0f);
   }

   void BaseComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("BaseComponent");
         addField( "Position", TypePoint3F, Offset(mPosition, BaseComponent), "Position within the object's local space." );
         addField( "Rotation", TypePoint3F, Offset(mRotation, BaseComponent), "Position within the object's local space." );
         addField( "Scale", TypePoint3F, Offset(mScale, BaseComponent), "Position within the object's local space." );
      endGroup("BaseComponent");
   }

   bool BaseComponent::raycast(const Point3F& start, const Point3F& end, Point3F& hitPoint)
   {
      return false;
   }

   void BaseComponent::refresh()
   {
      if ( !mOwnerObject ) return;

      // Build Transformation Matrix
      mLocalTransformMatrix.createSRTMatrix(mScale.x, mScale.y, mScale.z,
                           mRotation.x, mRotation.y, mRotation.z,
                           mPosition.x, mPosition.y, mPosition.z);

      // Combine local and world.
      mTransformMatrix = mOwnerObject->mTransformMatrix * mLocalTransformMatrix;

      // Set world position.
      mWorldPosition.set(mTransformMatrix[12], mTransformMatrix[13], mTransformMatrix[14]);
   }

   void BaseComponent::setUniformVec4(const char* name, Point4F value)
   {
      bgfx::UniformHandle handle = Graphics::Shader::getUniformVec4(name);

      if ( !mUniforms.isEmpty() )
      {
         for ( S32 n = 0; n < mUniforms.uniforms->size(); n++ )
         {
            Rendering::UniformData* uni = &mUniforms.uniforms->at(n);
            if ( uni->uniform.idx == handle.idx )
            {
               uni->setValue(value);
               refresh();
               return;
            }
         }
      }

      Rendering::UniformData* uni = mUniforms.addUniform();
      uni->uniform = handle;
      uni->count = 1;
      uni->setValue(value);
      refresh();
   }

   Box3F BaseComponent::getBoundingBox()
   {
      return mBoundingBox;
   }
}