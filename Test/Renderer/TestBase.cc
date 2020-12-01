#include "BaseType.h"
#include "Logger.h"
#include "volk.h"
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

    for (const auto &ext : extensions)
    {
        Logger::info("{} | Extension Properties", ext.extensionName);
    }

    UINT32 count = 0;
    for (const auto &layer : layers)
    {
        Logger::info("{} vkinstance layer", layer.layerName);
        vkEnumerateInstanceExtensionProperties(layer.layerName, &count, nullptr);
        std::vector<VkExtensionProperties> exts(count);
        vkEnumerateInstanceExtensionProperties(layer.layerName, &count, extensions.data());
        for (const auto &ext : exts)
        {
            Logger::info("{}", ext.extensionName);
        }
    }
    return 0;
}