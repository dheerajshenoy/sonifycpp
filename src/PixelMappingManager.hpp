#pragma once

#include "pixel.hpp"

#include <QObject>
#include <QString>
#include <functional>
#include <optional>
#include <string>
#include <vector>

using MapFunction =
    std::function<std::vector<short>(const std::vector<Pixel> &)>;

class PixelMappingManager : public QObject
{
    Q_OBJECT
public:

    void registerPixelMapping(const std::string &name, MapFunction fn) noexcept;
    void unregisterPixelMapping(const std::string &name) noexcept;
    std::string currentPixelMapping() const noexcept;
    std::vector<std::string> availablePixelMappings() const noexcept;
    std::vector<short>
    applyPixelMapping(const std::string &name,
                      const std::vector<Pixel> &col) noexcept;
    std::optional<MapFunction>
    getPixelMapping(const std::string &name) noexcept;

signals:
    void pixelMappingAdded(const std::string &pixelMappingName);
    void pixelMappingRemoved(const std::string &pixelMappingName);
    void pixelMappingApplied(const std::string &pixelMappingName);

private:

    std::string m_current_pixel_mapping;
    std::unordered_map<std::string, MapFunction> m_pixel_mappings;
};
