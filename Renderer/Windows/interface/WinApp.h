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
#ifdef _WIN32
#  pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#ifndef HAWL_WINAPP_H
#  define HAWL_WINAPP_H
#  include "IAppBase.h"
#  include <Windows.h>
#  include <string>
namespace Hawl {
class WindowsApp : IApp
{
public:
  WindowsApp();
  /// 禁用拷贝构造
  WindowsApp(const WindowsApp& rhs) = delete;
  WindowsApp& operator=(const WindowsApp& rhs) = delete;
  virtual ~WindowsApp() {}
  /// 初始化Windows窗体结构
  /// @note 可在继承中override 此方法
  virtual void InitWindowClass();
  /// 注册创建窗口
  /// @note 可在继承中override 此方法
  virtual INT CreateMainWindow();

  /// 初始化函数，可初始化变量，执行在创建窗口程序之前
  /// @note 继承对象覆盖需要执行
  virtual INT  Init();
  virtual void Exit();
  virtual bool Load();
  virtual void Unload();
  virtual void Update(FLOAT32 deltaTime);
  virtual void Draw();

  /// 长和宽的调用设置接口
  inline UINT GetWidth() const { return m_width; }
  inline UINT GetHeight() const { return m_height; }
  inline void SetWidth(UINT width) { m_width = width; }
  inline void SetHeight(UINT height) { m_height = height; }

  /// 窗口标题的调用设置接口
  inline const WCHAR* GetTitle() const { return m_title.c_str(); }
  inline void         SetTitle(std::wstring title);

  /// 启动
  void Run();

public:
protected:
  HINSTANCE  m_instance = nullptr;
  HWND       m_hWnd     = nullptr;
  WNDCLASSEX m_WndClass;
  UINT       m_width  = 1200;
  UINT       m_height = 800;

protected:
  static LRESULT CALLBACK WndProc(HWND   hWnd,
                                  UINT   message,
                                  WPARAM wParam,
                                  LPARAM lParam);

private:
  std::wstring m_title = L"空白标题";
};
}
#endif