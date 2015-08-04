/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
