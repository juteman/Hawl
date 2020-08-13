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
#include "Common.h"
#include <filesystem>
namespace Hawl
{

/// Use program_invocation_name to get the filename
const std::string &GetExecutableFilename()
{
    static std::string filename;
    filename = std::filesystem::path(program_invocation_name).filename();
    return filename;
}

const std::string &GetExecutableName()
{
    static std::string file;
    file = std::filesystem::path(program_invocation_name).stem();
    return file;
}

const std::string &GetExecutablePath()
{
    static std::string path;
    path = std::filesystem::path(program_invocation_name).parent_path();
    return path;
}
} // namespace Hawl