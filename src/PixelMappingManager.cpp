#include "PixelMappingManager.hpp"

void
PixelMappingManager::registerPixelMapping(const std::string &name,
                                          MapFunction fn) noexcept
{
    // FIXME: Guard against duplicate insertion ?
    emit pixelMappingAdded(name);
    m_pixel_mappings[name] = fn;
}

std::vector<std::string>
PixelMappingManager::availablePixelMappings() const noexcept
{
    if (m_pixel_mappings.empty()) return {};

    std::vector<std::string> maps;
    maps.resize(m_pixel_mappings.size());

    for (const auto &map : m_pixel_mappings)
        maps.push_back(map.first);

    return maps;
}

std::vector<short>
PixelMappingManager::applyPixelMapping(const std::string &name,
                                       const std::vector<Pixel> &col) noexcept
{
    emit pixelMappingApplied(name);
    return m_pixel_mappings[name](col);
}

void
PixelMappingManager::unregisterPixelMapping(const std::string &name) noexcept
{
    if (!m_pixel_mappings.contains(name)) return;

    m_pixel_mappings.erase(name);
    emit pixelMappingRemoved(name);
}

std::optional<MapFunction>
PixelMappingManager::getPixelMapping(const std::string &name) noexcept
{
    if (m_pixel_mappings.contains(name)) return m_pixel_mappings[name];
    return std::nullopt;
}
