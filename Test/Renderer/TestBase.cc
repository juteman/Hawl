#include "Renderer.inl"

int main()
{
    Hawl::Renderer* handle = RendererCreate();
    RendererInit(handle,"test name");
    RendererDelete(handle);
}