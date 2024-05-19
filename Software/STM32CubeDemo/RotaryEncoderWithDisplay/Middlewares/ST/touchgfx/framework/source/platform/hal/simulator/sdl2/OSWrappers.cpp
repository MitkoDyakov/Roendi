/******************************************************************************
* Copyright (c) 2018(-2024) STMicroelectronics.
* All rights reserved.
*
* This file is part of the TouchGFX 4.23.2 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
*******************************************************************************/

#include <assert.h>
#include <touchgfx/hal/OSWrappers.hpp>
#include <SDL2/SDL.h>

namespace touchgfx
{
static SDL_mutex* s_FrameBufferLock = NULL;
static bool framebufferLocked = false;

void OSWrappers::initialize()
{
    atexit(deinitialize);
    // Setup framebuffer lock
    s_FrameBufferLock = SDL_CreateMutex();
    framebufferLocked = false;
}

void OSWrappers::deinitialize()
{
    if (s_FrameBufferLock)
    {
        SDL_DestroyMutex(s_FrameBufferLock);
        s_FrameBufferLock = NULL;
    }
}

void OSWrappers::takeFrameBufferSemaphore()
{
    assert(!framebufferLocked && "Framebuffers already locked");
    framebufferLocked = true;
    SDL_LockMutex(s_FrameBufferLock);
}

void OSWrappers::giveFrameBufferSemaphore()
{
    assert(framebufferLocked && "Framebuffers not previously locked");
    framebufferLocked = false;
    SDL_UnlockMutex(s_FrameBufferLock);
}

void OSWrappers::waitForVSync()
{
}

void OSWrappers::tryTakeFrameBufferSemaphore()
{
}

void OSWrappers::giveFrameBufferSemaphoreFromISR()
{
}

void OSWrappers::taskYield()
{
}
} // namespace touchgfx
