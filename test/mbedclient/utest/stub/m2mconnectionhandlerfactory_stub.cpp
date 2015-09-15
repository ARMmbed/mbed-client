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

#include "m2mconfig.h"
#include "m2mconnectionhandlerfactory.h"
#include "m2mconnectionsecurity.h"
#include "m2mconnectionhandler.h"

#include "m2mconnectionsecurity.h"
#include "m2mconnectionhandler.h"

M2MConnectionHandler* M2MConnectionHandlerFactory::createConnectionHandler(M2MConnectionObserver &observer,
                                                                           M2MInterface::BindingMode mode,
                                                                           M2MInterface::NetworkStack stack){
    //TODO: return expected object
    M2MConnectionHandler* ret = NULL;
    M2MConnectionSecurity* sec = NULL;

    sec = new M2MConnectionSecurity();

    ret = new M2MConnectionHandler(observer, sec, mode, stack);

    return ret;
}
