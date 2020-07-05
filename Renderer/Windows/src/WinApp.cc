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

#include "WinApp.h"
#include <locale>

namespace Hawl {

WindowsApp::WindowsApp()
{
  m_instance = GetModuleHandle(NULL);
}

void
WindowsApp::SetTitle(std::wstring title)
{
  m_title = title;
}

bool
WindowsApp::Init()
{
  InitWindowClass();
  bool ret = CreateMainWindow();
  // 如果返回0，则创建窗体失败
  return ret;
}

void
WindowsApp::Run()
{
  Init();
  Load();
  Unload();
  /// TODO 这里采用循环，并传入 delta time
  Draw();
}

void
WindowsApp::InitWindowClass()
{
  m_WndClass.cbSize        = sizeof(WNDCLASSEX);
  m_WndClass.style         = CS_HREDRAW | CS_VREDRAW;
  m_WndClass.lpfnWndProc   = WndProc;
  m_WndClass.cbClsExtra    = 0;
  m_WndClass.cbWndExtra    = 0;
  m_WndClass.hInstance     = m_instance;
  m_WndClass.hIcon         = LoadIcon(0, IDI_APPLICATION);
  m_WndClass.hCursor       = LoadCursor(0, IDC_ARROW);
  m_WndClass.lpszClassName = L"WindowAppClass";
}

bool
WindowsApp::CreateMainWindow()
{
  if (!RegisterClassEx(&m_WndClass)) {
    MessageBox(0, L"注册结构失败.", 0, 0);
    return false;
  }

  RECT windowRect = {
    0, 0, static_cast<LONG>(GetWidth()), static_cast<LONG>(GetHeight())
  };

  AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
  m_hWnd = CreateWindow(m_WndClass.lpszClassName,
                        WindowsApp::GetTitle(),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        windowRect.right - windowRect.left,
                        windowRect.bottom - windowRect.top,
                        nullptr,
                        nullptr,
                        m_instance,
                        nullptr);

  if (!m_hWnd) {
    MessageBox(0, L"创建窗口失败", nullptr, 0);
    return false;
  }

  ShowWindow(m_hWnd, SW_SHOW);
  UpdateWindow(m_hWnd);
  return true;
}

}