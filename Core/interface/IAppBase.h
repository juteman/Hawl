/*
 * Copyright (c) 2020 juteman
 *
 * This file is part of ReForge
 * (see https://github.com/juteman/Hawl).
 *
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
#ifndef HAWL_IAPPBASE_H
#  define HAWL_IAPPBASE_H
#  include "BaseType.h"
namespace Hawl {
class IApp
{
public:
  virtual INT  Init()                    = 0;
  virtual void Exit()                    = 0;
  virtual bool Load()                    = 0;
  virtual void Unload()                  = 0;
  virtual void Update(FLOAT32 deltaTime) = 0;
  virtual void Draw()                    = 0;
  virtual ~IApp(){};
};
}
#endif
