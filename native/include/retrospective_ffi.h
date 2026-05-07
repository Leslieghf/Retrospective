#pragma once

#include <cstdint>

extern "C" {
std::uint32_t retro_api_version();
const char* retro_core_id();
std::int32_t retro_init();
void retro_shutdown();
std::int32_t retro_frame(std::uint32_t dt_ms);
std::int32_t retro_on_map_start(const char* map_name);
std::int32_t retro_on_player_spawn(std::uint32_t player_slot);
std::int32_t retro_on_player_death(std::uint32_t victim_slot, std::uint32_t attacker_slot);
std::uint32_t retro_frame_count();
std::uint32_t retro_player_spawn_count();
std::uint32_t retro_player_death_count();
std::uint32_t retro_last_spawn_player_slot();
std::uint32_t retro_last_death_victim_slot();
std::uint32_t retro_last_death_attacker_slot();
const char* retro_status_text(std::int32_t code);
}
