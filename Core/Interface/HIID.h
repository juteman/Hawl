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

/****************************************************************
 * 每个接口都是不可变的，一个新的接口不能和旧的接口拥有同样的一个标识
 * 这里采用了Hawl Interface ID (HIID) 的方法去保证每个接口的独立性
 *****************************************************************/
#pragma once
#ifndef HAWL_HIID_H
#  define HAWL_HIID_H
#  include "BaseType.h"
#  include <cstring>
namespace Hawl {

// 定义 HIID
typedef struct HIID
{
  UINT32 Data1;
  UINT16 Data2;
  UINT16 Data3;
  UINT8  Data4[8];

  // HIID 相等需每个字段相等
  bool operator==(const HIID& rhs)
  {
    return (Data1 = rhs.Data1) && (Data2 == rhs.Data2) &&
           (Data3 == rhs.Data3) &&
           (memcmp(Data4, rhs.Data4, sizeof(Data4)) == 0);
  }

} HIID;

static const HIID HIIDUnknown = { 0, 0, 0, { 0 } };

}
#endif
