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
#include <string>

namespace Hawl
{
class Logger
{
    HAWL_DISABLE_COPY(Logger)
  public:
    enum class Level
    {
        INFO = 0,
        WARNING = 1,
        ERROR = 2,
        FATAL = 3,
        DISABLED = -1
    };

    /**
     * \brief Shutdown the logger and close the logger file
     */
    static void Shutdown();

    /**
     * \brief Set the log file with path and name
     * \param filename the file with path name
     * \return true if set the path
     */
    static bool SetLogFile(const std::string &filename);

    /**
     * \brief Get the file name with Log
     * \return return the path and name with log file
     */
    static const std::string &GetLogFile();

  private:
    /**
     * \brief log the message to the log
     * \param level logging level
     * \param msg logging message to the file
     */
    static void Log(Level level, const std::string &msg);

    /**
     * add friend log function
     */
    friend void LogInfo(const std::string &msg);
    friend void LogWarn(const std::string &msg);
    friend void LogError(const std::string &msg);
    friend void LogFatal(const std::string &msg);
};

/**
 * \brief log message with information level
 * \param msg message to log
 */
inline void LogInfo(const std::string &msg)
{
    Logger::Log(Logger::Level::INFO, msg);
}

/**
 * \brief log message with warning level
 * \param msg message to log
 */
inline void LogWarn(const std::string &msg)
{
    Logger::Log(Logger::Level::WARNING, msg);
}

/**
 * \brief log message with error level
 * \param msg message to log
 */
inline void LogError(const std::string &msg)
{
    Logger::Log(Logger::Level::ERROR, msg);
}

/**
 * \brief log message with fatal level
 * \param msg message to log
 */
inline void LogFatal(const std::string &msg)
{
    Logger::Log(Logger::Level::FATAL, msg);
}

} // namespace Hawl
