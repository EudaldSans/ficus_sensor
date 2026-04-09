#ifndef VERSION_HH
#define VERSION_HH

#include "inttypes.h"

#include <cstddef>
#include <string_view>

struct Version {
    char storage[16] {};
    size_t length = 0;

    const uint8_t major, minor, patch, test;

    /**
     * @brief Construct a new version from a compressed 32 bit integer. 
     * Format is 0xMMmmpptt where MM is major, mm is minor, pp is patch, and tt is test. 
     * Each part can be up to 255.
     */
    constexpr Version(uint32_t version) : 
        Version((version >> 24) & 0xFF, (version >> 16) & 0xFF, (version >> 8) & 0xFF, version & 0xFF) {}
    
    /**
     * @brief Construct a new version from major minor patch and test numbers. Each part can be up to 255.
     */
    constexpr Version(uint8_t major, uint8_t minor, uint8_t patch, uint8_t test) : major(major), minor(minor), patch(patch), test(test) {
        const uint8_t version_parts = 4;
        uint8_t parts[version_parts] = {major, minor, patch, test};

        for (int i = 0; i < version_parts; i++) {
            uint8_t part = parts[i];
            if (part >= 100) { storage[length++] = '0' + (part / 100); }
            if (part >= 10) { storage[length++] = '0' + ((part / 10) % 10); }
            storage[length++] = '0' + (part % 10);

            if (i <version_parts - 1) { storage[length++] = '.'; }
        }

        storage[length] = '\0';
    }

    constexpr uint32_t to_uint32() const {
        return (static_cast<uint32_t>(major) << 24) | (static_cast<uint32_t>(minor) << 16) | (static_cast<uint32_t>(patch) << 8) | static_cast<uint32_t>(test);
    }

    constexpr const char* c_str() const { return storage; }
    constexpr operator std::string_view() const { return {storage, length}; }
};

#endif