/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "threadhelper_stub.h"

bool threadhelper_stub::visited;
void* threadhelper_stub::void_pointer;

void threadhelper_stub::clear()
{
    visited = false;
    void_pointer = NULL;
}

ThreadHelper::ThreadHelper()
{
}

ThreadHelper::~ThreadHelper()
{
}

bool ThreadHelper::start()
{
   return threadhelper_stub::visited;
}

bool ThreadHelper::join()
{
    return threadhelper_stub::visited;
}

bool ThreadHelper::cancel()
{
    return threadhelper_stub::visited;
}

void* ThreadHelper::thread_function(void* ptr)
{
    return threadhelper_stub::void_pointer;
}

bool ThreadHelper::create_thread()
{
    return threadhelper_stub::visited;
}
