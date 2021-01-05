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
#include "Common.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include <string>
namespace Hawl
{
namespace Logger = spdlog;

void _FailedAssert(const char* file, int line, const char* statement);

#if defined(_DEBUG)
#define IFASSERT(x) x
#define ASSERT(b) \
	if (!(b)) _FailedAssert(__FILE__, __LINE__, #b)
#else
#if _MSC_VER >= 1400
#define IFASSERT(x) x
#else
#define IFASSERT(x)
#endif
#define ASSERT(b) (void)(b)
#endif
} // namespace Hawl
