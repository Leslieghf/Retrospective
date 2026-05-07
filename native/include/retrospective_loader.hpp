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
    std::int32_t on_player_spawn(std::uint32_t player_slot) const;
    std::int32_t on_player_death(std::uint32_t victim_slot, std::uint32_t attacker_slot) const;
    std::uint32_t frame_count() const;
    std::uint32_t player_spawn_count() const;
    std::uint32_t player_death_count() const;
    std::uint32_t last_spawn_player_slot() const;
    std::uint32_t last_death_victim_slot() const;
    std::uint32_t last_death_attacker_slot() const;
    const char* status_text(std::int32_t code) const;

private:
    using RetroApiVersionFn = std::uint32_t (*)();
    using RetroCoreIdFn = const char* (*)();
    using RetroInitFn = std::int32_t (*)();
    using RetroShutdownFn = void (*)();
    using RetroFrameFn = std::int32_t (*)(std::uint32_t);
    using RetroOnMapStartFn = std::int32_t (*)(const char*);
    using RetroOnPlayerSpawnFn = std::int32_t (*)(std::uint32_t);
    using RetroOnPlayerDeathFn = std::int32_t (*)(std::uint32_t, std::uint32_t);
    using RetroFrameCountFn = std::uint32_t (*)();
    using RetroPlayerSpawnCountFn = std::uint32_t (*)();
    using RetroPlayerDeathCountFn = std::uint32_t (*)();
    using RetroLastSpawnPlayerSlotFn = std::uint32_t (*)();
    using RetroLastDeathVictimSlotFn = std::uint32_t (*)();
    using RetroLastDeathAttackerSlotFn = std::uint32_t (*)();
    using RetroStatusTextFn = const char* (*)(std::int32_t);

    void* handle_ = nullptr;
    RetroApiVersionFn retro_api_version_ = nullptr;
    RetroCoreIdFn retro_core_id_ = nullptr;
    RetroInitFn retro_init_ = nullptr;
    RetroShutdownFn retro_shutdown_ = nullptr;
    RetroFrameFn retro_frame_ = nullptr;
    RetroOnMapStartFn retro_on_map_start_ = nullptr;
    RetroOnPlayerSpawnFn retro_on_player_spawn_ = nullptr;
    RetroOnPlayerDeathFn retro_on_player_death_ = nullptr;
    RetroFrameCountFn retro_frame_count_ = nullptr;
    RetroPlayerSpawnCountFn retro_player_spawn_count_ = nullptr;
    RetroPlayerDeathCountFn retro_player_death_count_ = nullptr;
    RetroLastSpawnPlayerSlotFn retro_last_spawn_player_slot_ = nullptr;
    RetroLastDeathVictimSlotFn retro_last_death_victim_slot_ = nullptr;
    RetroLastDeathAttackerSlotFn retro_last_death_attacker_slot_ = nullptr;
    RetroStatusTextFn retro_status_text_ = nullptr;
};
