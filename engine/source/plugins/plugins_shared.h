﻿//-----------------------------------------------------------------------------
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

#ifndef _PLUGINS_SHARED_H
#define _PLUGINS_SHARED_H

#ifndef _CONSOLE_H_
#include <console/console.h>
#endif

#ifndef _RENDERINGCOMMON_H_
#include <3d/rendering/common.h>
#endif

#ifndef _TEXTURE_OBJECT_H_
#include "graphics/TextureObject.h"
#endif

#ifndef _SCENECAMERA_H_
#include "3d/scene/camera.h"
#endif

// Defines a pointer to a function in the loaded plugin.
#define PLUGIN_FUNC_PTR(name, ...) \
   typedef void (*name##Func)(__VA_ARGS__); \
   name##Func _##name;

#if defined _WIN32 || defined __CYGWIN__
  #ifdef TORQUE_PLUGIN
    #ifdef __GNUC__
      #define PLUGIN_FUNC __attribute__ ((dllexport))
    #else
      #define PLUGIN_FUNC __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define PLUGIN_FUNC __attribute__ ((dllimport))
    #else
      #define PLUGIN_FUNC __declspec(dllimport)
    #endif
  #endif
#else
  #if __GNUC__ >= 4
    #define PLUGIN_FUNC __attribute__ ((visibility ("default")))
  #else
    #define PLUGIN_FUNC
  #endif
#endif

namespace Plugins
{
   struct ConsoleWrapper
   {
      void (*printf)(const char *_format, ...);
      void (*warnf)(const char *_format, ...);
      void (*errorf)(const char *_format, ...);

      void (*addCommand)(const char *nsName, const char *name, VoidCallback cb, const char *usage, S32 minArgs, S32 maxArgs);

      const char* (*getData)(S32 type, void *dptr, S32 index, EnumTable *tbl, BitSet32 flag); // Defaults: *tbl = NULL, flag = 0
      Namespace* (*lookupNamespace)(const char *ns);
      bool (*classLinkNamespaces)(Namespace *parent, Namespace *child);
      void (*registerClassRep)(AbstractClassRep* in_pRep);

      S32 TypeF32;
      S32 TypeS8;
      S32 TypeS32;
      S32 TypeS32Vector;
      S32 TypeBool;
      S32 TypeBoolVector;
      S32 TypeF32Vector;
      S32 TypeString;
      S32 TypeStringTableEntryVector;
      S32 TypeCaseString;
      S32 TypeFilename;
      S32 TypeEnum;
      S32 TypeFlag;
      S32 TypeSimObjectPtr;
      S32 TypeSimObjectName;
      S32 TypeSimObjectId;
      S32 TypePoint3F;
   };

   struct SysGUIWrapper
   {
      S32 (*beginScrollArea)(const char* title, U32 x, U32 y, U32 width, U32 height);
      S32 (*endScrollArea)();
      S32 (*label)(const char* label);
      S32 (*list)(const char* script, void (*callback)()); // Defaults: script = "", callback = NULL
      S32 (*checkBox)(const char* label, bool value);
      S32 (*slider)(const char* label, S32 value, S32 min, S32 max);
      S32 (*textInput)(const char* label, const char* text);
      S32 (*button)(const char* label, const char* script, void (*callback)()); // Defaults: script = "", callback = NULL
      S32 (*separator)();

      void (*addListValue)(S32 id, const char* val, const char* script, void (*callback)()); // Defaults: script = "", callback = NULL
      const char* (*getListValue)(S32 id, S32 index);
      S32 (*getListSelected)(S32 id);
      void (*clearList)(S32 id);

      void  (*setElementHidden)(S32 id, bool val);
      char* (*getLabelValue)(S32 id);
      void  (*setLabelValue)(S32 id, const char* val);
      char* (*getTextValue)(S32 id);
      void  (*setTextValue)(S32 id, const char* val);
      S32   (*getIntValue)(S32 id);
      void  (*setIntValue)(S32 id, S32 val);
      bool  (*getBoolValue)(S32 id);
      void  (*setBoolValue)(S32 id, bool val);

      void  (*alignLeft)(S32 id);
      void  (*alignRight)(S32 id);
      void  (*alignTop)(S32 id);
      void  (*alignBottom)(S32 id);

      void (*clearScrollArea)(S32 id);
      void (*seek)(S32 id);
      void (*clearSeek)();
   };

   struct SceneWrapper
   {
      Scene::SceneCamera* (*getActiveCamera)();
      void (*pushActiveCamera)(const char *);
      void (*popActiveCamera)();
      Scene::SceneCamera* (*getCamera)(const char *);
      SimGroup* (*getEntityGroup)();
   };

   struct RenderingWrapper
   {
      bool*    canvasSizeChanged;
      U32*     canvasHeight; 
      U32*     canvasWidth;
      F32*     viewMatrix;
      F32*     projectionMatrix;

      Rendering::RenderData* (*createRenderData)();
   };

   struct GraphicsWrapper
   {
      bgfx::VertexDecl* PosUVColorVertex;

      bgfx::IndexBufferHandle* cubeIB;
      bgfx::VertexBufferHandle* cubeVB;

      TextureObject* (*loadTexture)(const char* pTextureKey, TextureHandle::TextureHandleType type, bool clampToEdge, bool checkOnly, bool force16Bit );
      bgfx::UniformHandle (*getTextureUniform)(U32 slot);
      bgfx::UniformHandle (*getUniformVec3)(const char* name, U32 count);
      bgfx::UniformHandle (*getUniformVec4)(const char* name, U32 count);
      bgfx::UniformHandle (*getUniform4x4Matrix)(const char* name, U32 count);
      Graphics::Shader* (*getShader)(const char* vertex_shader_path, const char* fragment_shader_path);
      Graphics::ShaderAsset* (*getShaderAsset)(const char* id);

      void (*fullScreenQuad)(float _textureWidth, float _textureHeight);
   };

   struct BGFXWrapper
   {
      void (*dbgTextClear)(uint8_t _attr, bool _small); // Defaults: _attr = 0, _small = false
	   void (*dbgTextPrintf)(uint16_t _x, uint16_t _y, uint8_t _attr, const char* _format, ...);

      void (*setViewClear)(uint8_t _id, uint16_t _flags, uint32_t _rgba, float _depth, uint8_t _stencil); // Defaults: _depth = 1.0, _stencil = 0
      void (*setViewRect)(uint8_t _id, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height);
      void (*setViewTransform)(uint8_t _id, const void* _view, const void* _projL, uint8_t _flags, const void* _projR); // Defaults: _flags = BGFX_VIEW_STEREO, _projR = NULL

      void (*setProgram)(bgfx::ProgramHandle _handle);
      void (*setTexture)(uint8_t _stage, bgfx::UniformHandle _sampler, bgfx::TextureHandle _handle, uint32_t _flags); // Defaults: _flags = UINT32_MAX
      void (*setState)(uint64_t _state, uint32_t _rgba); // Defaults: _rgba = 0
      void (*setUniform)(bgfx::UniformHandle _handle, const void* _value, uint16_t _num); // Defaults: _num = 1

      uint32_t (*submit)(uint8_t _id, int32_t _depth); // Defaults: _depth = 0

      const bgfx::Memory* (*makeRef)(const void* _data, uint32_t _size);

   	bgfx::IndexBufferHandle (*createIndexBuffer)(const bgfx::Memory* _mem, uint8_t _flags); // Defaults: _flags = BGFX_BUFFER_NONE
	   void (*destroyIndexBuffer)(bgfx::IndexBufferHandle _handle);

	   bgfx::VertexBufferHandle (*createVertexBuffer)(const bgfx::Memory* _mem, const bgfx::VertexDecl& _decl, uint8_t _flags); // Defaults: _flags = BGFX_BUFFER_NONE
	   void (*destroyVertexBuffer)(bgfx::VertexBufferHandle _handle);
   };

   struct PluginLink
   {
      ConsoleWrapper Con;
      SysGUIWrapper SysGUI;
      SceneWrapper Scene;
      RenderingWrapper Rendering;
      GraphicsWrapper Graphics;
      BGFXWrapper bgfx;
      
      _StringTable* StringTableLink;
      ResManager* ResourceManager;
   };

   extern DLL_PUBLIC Plugins::PluginLink Link;
   extern DLL_PUBLIC Vector<AbstractClassRep*> _pluginConsoleClasses;
}

#define IMPLEMENT_PLUGIN_CONOBJECT(className)                                                                       \
    AbstractClassRep* className::getClassRep() const { return &className::dynClassRep; }                            \
    AbstractClassRep* className::getStaticClassRep() { return &dynClassRep; }                                       \
    AbstractClassRep* className::getParentStaticClassRep() { return Parent::getStaticClassRep(); }                  \
    AbstractClassRep* className::getContainerChildStaticClassRep() { return NULL; }                                 \
    AbstractClassRep::WriteCustomTamlSchema className::getStaticWriteCustomTamlSchema() { return NULL; }            \
    className::className##Rep className::dynClassRep(#className, 0, -1, 0, className::getParentStaticClassRep())

#define DECLARE_PLUGIN_CONOBJECT(className)                                                                                         \
    class className##Rep;                                                                                                           \
    static className##Rep dynClassRep;                                                                                              \
    static AbstractClassRep* getParentStaticClassRep();                                                                             \
    static AbstractClassRep* getContainerChildStaticClassRep();                                                                     \
    static AbstractClassRep* getStaticClassRep();                                                                                   \
    static AbstractClassRep::WriteCustomTamlSchema getStaticWriteCustomTamlSchema();                                                \
    virtual AbstractClassRep* getClassRep() const;                                                                                  \
    class className##Rep : public AbstractClassRep {                                                                                \
      public:                                                                                                                       \
         className##Rep(const char *name, S32 netClassGroupMask, S32 netClassType, S32 netEventDir, AbstractClassRep *parent) {     \
            mClassName = name;                                                                                                      \
            for (U32 i = 0; i < NetClassGroupsCount; i++) mClassId[i] = -1;                                                         \
            mClassType = netClassType;                                                                                              \
            mClassGroupMask = netClassGroupMask;                                                                                    \
            mNetEventDir = netEventDir;                                                                                             \
            parentClass = parent;                                                                                                   \
            Plugins::_pluginConsoleClasses.push_back(this);                                                                         \
      }                                                                                                                             \
      void registerClass() {                                                                                                        \
         Plugins::Link.Con.registerClassRep( this );                                                                                \
         mNamespace = Plugins::Link.Con.lookupNamespace( Plugins::Link.StringTableLink->insert( getClassName() ) );                 \
         mNamespace->mClassRep = this;                                                                                              \
         sg_tempFieldList.setSize(0);                                                                                               \
         init();                                                                                                                    \
         if ( sg_tempFieldList.size() != 0 )                                                                                        \
            mFieldList = sg_tempFieldList;                                                                                          \
         sg_tempFieldList.clear();                                                                                                  \
      }                                                                                                                             \
      virtual AbstractClassRep* getContainerChildClass(const bool recurse) {                                                        \
         AbstractClassRep* pChildren = className::getContainerChildStaticClassRep();                                                \
         if (!recurse || pChildren != NULL) return pChildren;                                                                       \
         AbstractClassRep* pParent = className::getParentStaticClassRep();                                                          \
         if (pParent == NULL) return NULL;                                                                                          \
         return pParent->getContainerChildClass(recurse);                                                                           \
      }                                                                                                                             \
      virtual WriteCustomTamlSchema getCustomTamlSchema(void) { return className::getStaticWriteCustomTamlSchema(); }               \
      void init() const {                                                                                                           \
         AbstractClassRep *parent = className::getParentStaticClassRep();                                                           \
         AbstractClassRep *child = className::getStaticClassRep();                                                                  \
         if (parent && child) Plugins::Link.Con.classLinkNamespaces(parent->getNameSpace(), child->getNameSpace());                 \
         className::initPersistFields();                                                                                            \
         className::consoleInit();                                                                                                  \
      }                                                                                                                             \
      ConsoleObject* create() const { return new className; }                                                                       \
   };                                                                                           

#endif