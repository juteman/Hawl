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
#ifndef HAWL_TIMER_H
#  define HAWL_TIMER_H
#  include "BaseType.h"
namespace Hawl {
INT64
getUSec();

INT64
getTimerFrequency();

// 获得当前时间，毫秒
UINT32
getMSec();

UINT32
getTimeSinceStart();

/// 低精度时间
class LTimer
{
public:
  LTimer();

  /// 得到时间差
  /// @param reset 调用函数是否重置时间
  /// @return 返回时间差
  UINT32 GetElapsedTime(bool reset);

  //重置时间
  void Reset();

private:
  UINT32 m_startTime;
};

/// 高精度时间
class HTimer
{
public:
  HTimer();

  INT64   GetElapsedTime(bool reset);
  INT64   GetElapsedTimeAverage();
  FLOAT32 GetSeconds(bool reset);
  FLOAT32 GetSecondsAverage();
  void    Reset();

private:
  INT64 m_startTime;

  static const UINT32 LENGTH_OF_HISTORY = 60;
  INT64               m_history[LENGTH_OF_HISTORY];
  UINT32              m_historyIndex;
};
}

#endif // !TIMER_H
