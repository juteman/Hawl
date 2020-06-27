/**
 *  Copyright 2020 juteman
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#pragma once
#ifndef HAWL_WINAPP_H
#  define HAWL_WINAPP_H
#  include "IAppBase.h"
#  include <Windows.h>
namespace Hawl {
class WindowsApp : IApp
{
public:
  WindowsApp();
  /// 禁用拷贝构造
  WindowsApp(const WindowsApp& rhs) = delete;
  WindowsApp& operator=(const WindowsApp& rhs) = delete;
  /// 虚构方法
  virtual bool Init();
  /// 初始化Windows窗体结构
  /// @param title 窗口标题
  /// @note 可在继承中override 此方法
  virtual void        InitWindowClass();
  virtual void        CreateMainWindow();
  virtual void        Exit();
  virtual bool        Load();
  virtual void        Unload();
  virtual void        Update(FLOAT32 deltaTime);
  virtual void        Draw();
  virtual const char* GetName();

public:
  const WCHAR* m_title;

protected:
  HINSTANCE  m_instance = nullptr;
  HWND       m_hWnd     = nullptr;
  WNDCLASSEX m_WndClass;

protected:
  static LRESULT CALLBACK WndProc(HWND   hWnd,
                                  UINT   message,
                                  WPARAM wParam,
                                  LPARAM lParam);
};
}
#endif