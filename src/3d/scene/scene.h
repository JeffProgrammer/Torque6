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

#ifndef _SCENE_CORE_H_
#define _SCENE_CORE_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _PHYSICS_H
#include <physics/physics.h>
#endif

#ifndef _NETCONNECTION_H_
#include "network/netConnection.h"
#endif

#ifndef _SCENE_FEATURE_H_
#include "feature.h"
#endif

namespace Scene
{
   class SceneObject;
   class SceneCamera;

   // Init/Destroy
   void init();
   void destroy();

   void clear();
   void clearGhosted();
   void load(const char* filename);
   void save(const char* filename);

   // Scene Functions
   SimGroup*      getSceneGroup();
   Box3F          getSceneBounds();

   // Scene Cameras
   SceneCamera*   getActiveCamera();
   void           pushActiveCamera(const char* name);
   void           popActiveCamera();
   void           addCamera(const char* name, SceneCamera* cam);
   SceneCamera*   getCamera(const char* name);

   // Scene Entities
   void           addObject(SceneObject* obj, const char* name = "SceneObject");
   void           deleteObject(SceneObject* obj);
   void           removeObject(SceneObject* obj);
   void           refresh();
   SceneObject*   raycast(const Point3F& start, const Point3F& end);

   // Scene Features
   void        addFeature(SceneFeature* feature);
   void        deleteFeature(SceneFeature* feature);
   void        removeFeature(SceneFeature* feature);

   // Networking
   void onCameraScopeQuery(NetConnection *cr, CameraScopeQuery *camInfo);
}

#endif