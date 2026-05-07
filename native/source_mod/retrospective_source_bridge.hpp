#pragma once

#include <cstdint>
#include <string>

class RetrospectiveSourceBridge {
public:
    bool startup(const std::string& rust_lib_path, std::string& error_out);
    void shutdown();

    void on_level_init(const char* map_name);
    void on_game_frame(std::uint32_t dt_ms);

    bool ready() const;
    std::uint32_t frame_count() const;
};

