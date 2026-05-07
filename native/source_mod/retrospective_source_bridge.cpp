#include "retrospective_source_bridge.hpp"

#include "retrospective_loader.hpp"

namespace {
RetrospectiveLoader g_loader;
bool g_started = false;
} // namespace

bool RetrospectiveSourceBridge::startup(const std::string& rust_lib_path, std::string& error_out) {
    if (g_started) {
        return true;
    }

    if (!g_loader.load(rust_lib_path, error_out)) {
        return false;
    }

    const auto init_code = g_loader.init();
    if (init_code != 0) {
        error_out = g_loader.status_text(init_code);
        g_loader.unload();
        return false;
    }

    g_started = true;
    return true;
}

void RetrospectiveSourceBridge::shutdown() {
    if (!g_started) {
        return;
    }

    g_loader.shutdown();
    g_loader.unload();
    g_started = false;
}

void RetrospectiveSourceBridge::on_level_init(const char* map_name) {
    if (!g_started) {
        return;
    }
    g_loader.on_map_start(map_name);
}

void RetrospectiveSourceBridge::on_game_frame(std::uint32_t dt_ms) {
    if (!g_started) {
        return;
    }
    g_loader.frame(dt_ms);
}

void RetrospectiveSourceBridge::on_player_spawn(std::uint32_t player_slot) {
    if (!g_started) {
        return;
    }
    g_loader.on_player_spawn(player_slot);
}

void RetrospectiveSourceBridge::on_player_death(std::uint32_t victim_slot, std::uint32_t attacker_slot) {
    if (!g_started) {
        return;
    }
    g_loader.on_player_death(victim_slot, attacker_slot);
}

bool RetrospectiveSourceBridge::ready() const {
    return g_started && g_loader.is_loaded();
}

std::uint32_t RetrospectiveSourceBridge::frame_count() const {
    return g_loader.frame_count();
}

std::uint32_t RetrospectiveSourceBridge::player_spawn_count() const {
    return g_loader.player_spawn_count();
}

std::uint32_t RetrospectiveSourceBridge::player_death_count() const {
    return g_loader.player_death_count();
}
