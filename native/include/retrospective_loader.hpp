#pragma once

#include <cstdint>
#include <string>

class RetrospectiveLoader {
public:
    RetrospectiveLoader() = default;
    ~RetrospectiveLoader();

    RetrospectiveLoader(const RetrospectiveLoader&) = delete;
    RetrospectiveLoader& operator=(const RetrospectiveLoader&) = delete;

    bool load(const std::string& lib_path, std::string& error_out);
    void unload();
    bool is_loaded() const;

    std::uint32_t api_version() const;
    const char* core_id() const;
    std::int32_t init() const;
    void shutdown() const;
    std::int32_t frame(std::uint32_t dt_ms) const;
    std::int32_t on_map_start(const char* map_name) const;
    std::uint32_t frame_count() const;
    const char* status_text(std::int32_t code) const;

private:
    using RetroApiVersionFn = std::uint32_t (*)();
    using RetroCoreIdFn = const char* (*)();
    using RetroInitFn = std::int32_t (*)();
    using RetroShutdownFn = void (*)();
    using RetroFrameFn = std::int32_t (*)(std::uint32_t);
    using RetroOnMapStartFn = std::int32_t (*)(const char*);
    using RetroFrameCountFn = std::uint32_t (*)();
    using RetroStatusTextFn = const char* (*)(std::int32_t);

    void* handle_ = nullptr;
    RetroApiVersionFn retro_api_version_ = nullptr;
    RetroCoreIdFn retro_core_id_ = nullptr;
    RetroInitFn retro_init_ = nullptr;
    RetroShutdownFn retro_shutdown_ = nullptr;
    RetroFrameFn retro_frame_ = nullptr;
    RetroOnMapStartFn retro_on_map_start_ = nullptr;
    RetroFrameCountFn retro_frame_count_ = nullptr;
    RetroStatusTextFn retro_status_text_ = nullptr;
};

