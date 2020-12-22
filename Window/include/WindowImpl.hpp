#pragma once
#include "BaseType.h"
#include "WindowHandlePlatform.h"

namespace Hawl
{
class WindowImpl
{
  public:
    enum WindowMode {
        Minimized,
        Normal,
        FullScreen
    };


    struct Desc {
        uint32 width;
        uint32 height;
        eastl::string title;
        WindowMode windowMode;
        bool isResizeAble;
    };

    uint32 getHeight() const
    {
        return mDesc.height;
    }

    uint32 getWidth() const
    {
        return mDesc.width;
    }

    const WindowHandle& getWindowHandle() const
    {
        return mWindowHandle;
    }

  private:
    Desc         mDesc;
    WindowHandle mWindowHandle;
};
}