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

#include "platform/platform.h"
#include "console/console.h"
#include "sim/simBase.h"
#include "physicsThread.h"
#include "math/mMath.h"

#include <bx/timer.h>

namespace Physics 
{

   PhysicsObject PhysicsThread::smPhysicsObjects[2048];
   U32 PhysicsThread::smPhysicsObjectCount = 0;
   U32 PhysicsThread::smPhysicsSteps = 0;
   bool PhysicsThread::smPhysicsObjectUpdated = false;
   
   void* PhysicsThread::smPhysicsExecuteMutex = Mutex::createMutex();
   void* PhysicsThread::smPhysicsFinishedMutex = Mutex::createMutex();


   PhysicsThread::PhysicsThread()
   {

   }

   void PhysicsThread::run(void *arg)
   {
      F32 dt = (1.0f/32.0f);

      while ( !shouldStop )
      {
         // This won't be available when we're not suppoesd to run.
         if( Mutex::lockMutex(PhysicsThread::smPhysicsExecuteMutex) )
         {
            for ( U32 x = 0; x < smPhysicsSteps; x++ )
            {
               for (U32 a = 0; a < smPhysicsObjectCount; a++)
               {
                  if ( smPhysicsObjects[a].deleted ) continue;

                  if ( !smPhysicsObjects[a].velocity.isZero() )
                  {
                     smPhysicsObjects[a].position += smPhysicsObjects[a].velocity * dt;
                     smPhysicsObjects[a].worldBoundingBox.minExtents += smPhysicsObjects[a].velocity * dt;
                     smPhysicsObjects[a].worldBoundingBox.maxExtents += smPhysicsObjects[a].velocity * dt;
                     smPhysicsObjects[a].updated = true;
                  }

                  for(U32 b = 0; b < smPhysicsObjectCount; b++)
                  {
                     if ( a == b || smPhysicsObjects[b].deleted ) continue;

                     if ( smPhysicsObjects[a].worldBoundingBox.isOverlapped(smPhysicsObjects[b].worldBoundingBox) )
                     {
                        Box3F overlap = smPhysicsObjects[a].worldBoundingBox.getOverlap(smPhysicsObjects[b].worldBoundingBox);
                        Con::printf("Overlap: min: %f %f %f max: %f %f %f", overlap.minExtents.x,
                                                                            overlap.minExtents.y, 
                                                                            overlap.minExtents.z,
                                                                            overlap.maxExtents.x,
                                                                            overlap.maxExtents.y,
                                                                            overlap.maxExtents.z);

                        // Bounding Boxes Collide.
                        Sim::postEvent(Sim::getRootGroup(), new PhysicsEvent(smPhysicsObjects[a], smPhysicsObjects[b]), -1);

                        if ( !smPhysicsObjects[a].velocity.isZero() )
                        {
                           smPhysicsObjects[a].position -= smPhysicsObjects[a].velocity * dt;
                           smPhysicsObjects[a].worldBoundingBox.minExtents -= smPhysicsObjects[a].velocity * dt;
                           smPhysicsObjects[a].worldBoundingBox.maxExtents -= smPhysicsObjects[a].velocity * dt;
                           smPhysicsObjects[a].velocity.set(0, 0, 0);
                        }
                     }
                  }
               }
            }

            // Release executionion mutex.
            Mutex::unlockMutex(PhysicsThread::smPhysicsExecuteMutex);

            // This will block execution until the main thread 
            // obtains the smPhysicsExecuteMutex lock.
            if( Mutex::lockMutex(PhysicsThread::smPhysicsFinishedMutex) )
               Mutex::unlockMutex(PhysicsThread::smPhysicsFinishedMutex);
         }
      }
   }

   void PhysicsEvent::process(SimObject *object)
   {
      if ( !mObjA.onCollideDelegate.empty() )
         mObjA.onCollideDelegate(mObjB);

      if ( !mObjB.onCollideDelegate.empty() )
         mObjB.onCollideDelegate(mObjA);
   }

   PhysicsEngine::PhysicsEngine()
   {
      mAccumulatorTime = 0.0f;
      mPreviousTime = (F64)( bx::getHPCounter()/F64(bx::getHPFrequency()) );;

      // We lock this to cause the physics thread to block until we release it.
      Mutex::lockMutex(PhysicsThread::smPhysicsFinishedMutex);

      setProcessTicks(true);
      mPhysicsThread = new PhysicsThread();
      mPhysicsThread->start();
   }

   PhysicsEngine::~PhysicsEngine()
   {
      Mutex::unlockMutex(PhysicsThread::smPhysicsExecuteMutex);
      Mutex::unlockMutex(PhysicsThread::smPhysicsFinishedMutex);
      SAFE_DELETE(mPhysicsThread);
   }

   void PhysicsEngine::processPhysics()
   {  
      // This will block until the physics thread is finished execution.
      if( Mutex::lockMutex(PhysicsThread::smPhysicsExecuteMutex) )
      {
         // Unlocking this will stop the physics thread from blocking at the
         // end of it's tick.
         Mutex::unlockMutex(PhysicsThread::smPhysicsFinishedMutex);

         // We're sycned, it's a great time for updates.

         PhysicsThread::smPhysicsSteps = mFloorD(mAccumulatorTime / (1.0f/32.0f));
         mAccumulatorTime -= PhysicsThread::smPhysicsSteps * (1.0f/32.0f);
         //Con::printf("Performing %d physics steps.", PhysicsThread::smPhysicsSteps);

         PhysicsThread::smPhysicsObjectCount = mComponents.size();
         for ( U32 n = 0; n < mComponents.size(); ++n)
         {
            PhysicsObject* obj = &PhysicsThread::smPhysicsObjects[n];
            Scene::PhysicsComponent* comp = mComponents[n];

            // This means the position was changed outside the simulation.
            if ( obj->deleted || comp->mDirty )
            {
               obj->deleted = false;
               obj->onCollideDelegate.bind(comp, &Scene::PhysicsComponent::onCollide);
               obj->entity = comp->mOwnerEntity;
               obj->component = comp;
               obj->position = comp->mOwnerEntity->mPosition;
               obj->worldBoundingBox = comp->mOwnerEntity->mBoundingBox;
               obj->velocity = comp->mInputVelocity;
               obj->typeStr = "none";
               comp->mDirty = false;
               comp->mCurrPosition = obj->position;
            } else {
               comp->mPrevPosition = comp->mCurrPosition;
               comp->mNextPosition = obj->position;
               comp->mCurrVelocity = obj->velocity;
            }
         }

         // We're going to unlock this so the physics thread can 
         // execute while we get on with the rest of the frames.
         Mutex::unlockMutex(PhysicsThread::smPhysicsExecuteMutex);

         // And lock this so the physics thread will lock again at 
         // the end of execution of a time step.
         Mutex::lockMutex(PhysicsThread::smPhysicsFinishedMutex);
      }
   }

   void PhysicsEngine::interpolateTick( F32 delta )
   {  
      F64 time = (F64)( bx::getHPCounter()/F64(bx::getHPFrequency()) );
      F64 dt = (time - mPreviousTime);
      mPreviousTime = time;
      mAccumulatorTime += dt;

      if ( mAccumulatorTime >= (1.0f/32.0f) )
         processPhysics();

      // Trigger interpolation from all physics components with dt = mAccumulatorTime
      for ( U32 n = 0; n < mComponents.size(); ++n)
      {
         mComponents[n]->interpolate(mAccumulatorTime / (1.0f/32.0f));
      }
   }

   void PhysicsEngine::processTick()
   {  
      //
   }

   void PhysicsEngine::advanceTime( F32 timeDelta )
   {  
      //
   }

   void PhysicsEngine::addComponent(Scene::PhysicsComponent* comp)
   {
      mComponents.push_back(comp);
   }

}