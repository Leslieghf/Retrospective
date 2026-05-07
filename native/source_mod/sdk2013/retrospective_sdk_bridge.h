#pragma once

#include <stddef.h>

class RetrospectiveSdkBridge {
public:
    RetrospectiveSdkBridge();
    ~RetrospectiveSdkBridge();

    bool Startup(const char* lib_path, char* error_out, size_t error_out_len);
    void Shutdown();

    bool IsReady() const;

    int OnMapStart(const char* map_name) const;
    int OnFrame(unsigned int dt_ms) const;
    int OnPlayerSpawn(unsigned int player_slot) const;
    int OnPlayerDeath(unsigned int victim_slot, unsigned int attacker_slot) const;
    unsigned int FrameCount() const;
    unsigned int PlayerSpawnCount() const;
    unsigned int PlayerDeathCount() const;
    unsigned int LastSpawnPlayerSlot() const;
    unsigned int LastDeathVictimSlot() const;
    unsigned int LastDeathAttackerSlot() const;
    const char* StatusText(int code) const;
    unsigned int ApiVersion() const;
    const char* CoreId() const;

private:
    RetrospectiveSdkBridge(const RetrospectiveSdkBridge&);
    RetrospectiveSdkBridge& operator=(const RetrospectiveSdkBridge&);

    bool BindSymbol(const char* symbol_name, void** out_fn, char* error_out, size_t error_out_len);
    static void WriteError(char* error_out, size_t error_out_len, const char* message);

    void* module_;
    bool started_;

    unsigned int (*retro_api_version_)();
    const char* (*retro_core_id_)();
    int (*retro_init_)();
    void (*retro_shutdown_)();
    int (*retro_frame_)(unsigned int dt_ms);
    int (*retro_on_map_start_)(const char* map_name);
    int (*retro_on_player_spawn_)(unsigned int player_slot);
    int (*retro_on_player_death_)(unsigned int victim_slot, unsigned int attacker_slot);
    unsigned int (*retro_frame_count_)();
    unsigned int (*retro_player_spawn_count_)();
    unsigned int (*retro_player_death_count_)();
    unsigned int (*retro_last_spawn_player_slot_)();
    unsigned int (*retro_last_death_victim_slot_)();
    unsigned int (*retro_last_death_attacker_slot_)();
    const char* (*retro_status_text_)(int code);
};
