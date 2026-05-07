#pragma once

#include <cstdint>

extern "C" {
std::uint32_t retro_api_version();
const char* retro_core_id();
std::int32_t retro_init();
void retro_shutdown();
std::int32_t retro_frame(std::uint32_t dt_ms);
std::int32_t retro_on_map_start(const char* map_name);
std::uint32_t retro_frame_count();
const char* retro_status_text(std::int32_t code);
}

