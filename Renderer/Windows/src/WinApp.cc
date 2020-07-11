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
#include "Timer.h"
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

INT
WindowsApp::Init()
{
  InitWindowClass();
  INT ret = CreateMainWindow();
  // 如果返回1，则创建窗体失败
  return ret;
}

void
WindowsApp::Exit()
{}

bool
WindowsApp::Load()
{
  return true;
}

void
WindowsApp::Unload()
{}

void
WindowsApp::Update(FLOAT32 deltaTime)
{}

void
WindowsApp::Draw()
{}

void
WindowsApp::Run()
{
  INT ret = Init();
  if (ret) {
    MessageBox(0, L"Init Window Failed.", 0, 0);
    return;
  }
  Load();

  Unload();
  HTimer timer;
  MSG    msg = { 0 };
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else {
      FLOAT32 deltaTime = timer.GetSeconds(true);
      Update(deltaTime);
      Draw();
    }
  }

  Exit();
}

void
WindowsApp::InitWindowClass()
{
    m_WndClass.cbSize = sizeof(WNDCLASSEX);
    m_WndClass.style = CS_HREDRAW | CS_VREDRAW;
    m_WndClass.lpfnWndProc = WndProc;
    m_WndClass.cbClsExtra = 0;
    m_WndClass.cbWndExtra = 0;
    m_WndClass.hInstance = m_instance;
    m_WndClass.hIcon = LoadIcon(m_instance, IDI_APPLICATION);
    m_WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    m_WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    m_WndClass.lpszMenuName = NULL;
    m_WndClass.lpszClassName = L"WindowsClass";
    m_WndClass.hIconSm = LoadIcon(m_WndClass.hInstance, IDI_APPLICATION);
}

INT
WindowsApp::CreateMainWindow()
{
  if (!RegisterClassEx(&m_WndClass)) {
    MessageBox(0, L"Register failed.", 0, 0);
    return 1;
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
    MessageBox(0, L"Create Windows failed", nullptr, 0);
    return 2;
  }

  ShowWindow(m_hWnd, SW_SHOW);
  UpdateWindow(m_hWnd);
  return 0;
}

LRESULT CALLBACK
WindowsApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(hWnd, message, wParam, lParam);
}

} // end namespace