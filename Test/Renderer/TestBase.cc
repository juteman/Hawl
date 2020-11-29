#include "volk.h"
#include "BaseType.h"
#include "Logger.h"
#include <vector>
using namespace Hawl;

int main()
{
    volkInitialize();
    UINT32 extensionCount{0};
    UINT32 layerCount{0};
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    Logger::info("extension count is  {}", extensionCount);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    std::vector<VkLayerProperties>     layers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    for (const auto& extension : extensions) {
        Logger::info("extension name {}", extension.extensionName);
    }
    for (const auto &layer : layers)
    {
        Logger::info("{} vkinstance layer", layer.layerName);
    }
    return 0;
}