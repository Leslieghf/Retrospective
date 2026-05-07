#pragma once

#include <cstdint>
#include <string>

class RetrospectiveSourceBridge {
public:
    bool startup(const std::string& rust_lib_path, std::string& error_out);
    void shutdown();

    void on_level_init(const char* map_name);
    void on_game_frame(std::uint32_t dt_ms);
    void on_player_spawn(std::uint32_t player_slot);
    void on_player_death(std::uint32_t victim_slot, std::uint32_t attacker_slot);

    bool ready() const;
    std::uint32_t frame_count() const;
    std::uint32_t player_spawn_count() const;
    std::uint32_t player_death_count() const;
};
