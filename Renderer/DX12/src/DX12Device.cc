#include "DX12Device.h"
#include "BaseType.h"
namespace Hawl {

void
DX12Device::CreateDXGIFactory6(bool isDebug)
{
  UINT32 flags = isDebug ? DXGI_CREATE_FACTORY_DEBUG : 0;
  CreateDXGIFactory2(flags, IID_PPV_ARGS(m_dxgiFactory6.GetInitReference()));
  m_dxgiFactory6->QueryInterface(
    IID_PPV_ARGS(m_dxgiFactory6.GetInitReference()));
}

void
DX12Device::CreateDevice(bool isDebug)
{
  CreateDXGIFactory6(isDebug);
}

void
DX12Device::GetHardwareAdapter()
{
  IDXGIAdapter4* tempAdapter;
  for (UINT32 i = 0;
       m_dxgiFactory6->EnumAdapterByGpuPreference(
         i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tempAdapter)) !=
       DXGI_ERROR_NOT_FOUND;
       i++) {
    DXGI_ADAPTER_DESC3 desc;
    tempAdapter->GetDesc3(&desc);

    if (desc.VendorId == 0x10DE) {
      m_adapterDesc.adapterDesc  = desc;
      m_adapterDesc.adapterIndex = i;
      /// TODO Check the feature level support here
    }
  }
}
}