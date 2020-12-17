/*
 * Copyright (c) 2020 juteman
 *
 * This file is part of ReForge
 * (see https://github.com/juteman/Hawl).
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#pragma once
#include "Renderer.h"

/**
 * \brief Create Renderer Handle
 * \return return a Renderer class pointer
 */
HAWLRENDERERAPI Hawl::Renderer *RendererCreate()
{
    return new Hawl::Renderer;
}

/**
 * \brief See Hawl::Renderer::init
 * \param renderer Renderer class pointer
 */
HAWLRENDERERAPI void RendererInit(Hawl::Renderer *renderer, const char *renderName)
{
    renderer->Init(renderName, false);
}

/**
 * \brief Delete Handle
 * \param renderer pointer to be free
 */
HAWLRENDERERAPI void RendererDelete(Hawl::Renderer *renderer)
{
    if (renderer != nullptr)
    {
        delete renderer;
        renderer = nullptr;
    }
}
