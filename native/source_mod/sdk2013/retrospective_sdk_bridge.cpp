#include "retrospective_sdk_bridge.h"

#include <dlfcn.h>
#include <string.h>

RetrospectiveSdkBridge::RetrospectiveSdkBridge()
    : module_(NULL),
      started_(false),
      retro_api_version_(NULL),
      retro_core_id_(NULL),
      retro_init_(NULL),
      retro_shutdown_(NULL),
      retro_frame_(NULL),
      retro_on_map_start_(NULL),
      retro_on_player_spawn_(NULL),
      retro_on_player_death_(NULL),
      retro_frame_count_(NULL),
      retro_player_spawn_count_(NULL),
      retro_player_death_count_(NULL),
      retro_last_spawn_player_slot_(NULL),
      retro_last_death_victim_slot_(NULL),
      retro_last_death_attacker_slot_(NULL),
      retro_status_text_(NULL) {}

RetrospectiveSdkBridge::~RetrospectiveSdkBridge() {
    Shutdown();
}

bool RetrospectiveSdkBridge::Startup(const char* lib_path, char* error_out, size_t error_out_len) {
    if (started_) {
        return true;
    }

    if (lib_path == NULL || lib_path[0] == '\0') {
        WriteError(error_out, error_out_len, "empty rust core path");
        return false;
    }

    module_ = dlopen(lib_path, RTLD_NOW | RTLD_LOCAL);
    if (module_ == NULL) {
        WriteError(error_out, error_out_len, dlerror());
        return false;
    }

    if (!BindSymbol("retro_api_version", reinterpret_cast<void**>(&retro_api_version_), error_out, error_out_len) ||
        !BindSymbol("retro_core_id", reinterpret_cast<void**>(&retro_core_id_), error_out, error_out_len) ||
        !BindSymbol("retro_init", reinterpret_cast<void**>(&retro_init_), error_out, error_out_len) ||
        !BindSymbol("retro_shutdown", reinterpret_cast<void**>(&retro_shutdown_), error_out, error_out_len) ||
        !BindSymbol("retro_frame", reinterpret_cast<void**>(&retro_frame_), error_out, error_out_len) ||
        !BindSymbol("retro_on_map_start", reinterpret_cast<void**>(&retro_on_map_start_), error_out, error_out_len) ||
        !BindSymbol("retro_frame_count", reinterpret_cast<void**>(&retro_frame_count_), error_out, error_out_len) ||
        !BindSymbol("retro_on_player_spawn", reinterpret_cast<void**>(&retro_on_player_spawn_), error_out, error_out_len) ||
        !BindSymbol("retro_on_player_death", reinterpret_cast<void**>(&retro_on_player_death_), error_out, error_out_len) ||
        !BindSymbol("retro_player_spawn_count", reinterpret_cast<void**>(&retro_player_spawn_count_), error_out, error_out_len) ||
        !BindSymbol("retro_player_death_count", reinterpret_cast<void**>(&retro_player_death_count_), error_out, error_out_len) ||
        !BindSymbol("retro_last_spawn_player_slot", reinterpret_cast<void**>(&retro_last_spawn_player_slot_), error_out, error_out_len) ||
        !BindSymbol("retro_last_death_victim_slot", reinterpret_cast<void**>(&retro_last_death_victim_slot_), error_out, error_out_len) ||
        !BindSymbol("retro_last_death_attacker_slot", reinterpret_cast<void**>(&retro_last_death_attacker_slot_), error_out, error_out_len) ||
        !BindSymbol("retro_status_text", reinterpret_cast<void**>(&retro_status_text_), error_out, error_out_len)) {
        Shutdown();
        return false;
    }

    const int init_code = retro_init_();
    if (init_code != 0) {
        WriteError(error_out, error_out_len, StatusText(init_code));
        Shutdown();
        return false;
    }

    started_ = true;
    return true;
}

void RetrospectiveSdkBridge::Shutdown() {
    if (retro_shutdown_ != NULL && started_) {
        retro_shutdown_();
    }

    started_ = false;
    retro_api_version_ = NULL;
    retro_core_id_ = NULL;
    retro_init_ = NULL;
    retro_shutdown_ = NULL;
    retro_frame_ = NULL;
    retro_on_map_start_ = NULL;
    retro_on_player_spawn_ = NULL;
    retro_on_player_death_ = NULL;
    retro_frame_count_ = NULL;
    retro_player_spawn_count_ = NULL;
    retro_player_death_count_ = NULL;
    retro_last_spawn_player_slot_ = NULL;
    retro_last_death_victim_slot_ = NULL;
    retro_last_death_attacker_slot_ = NULL;
    retro_status_text_ = NULL;

    if (module_ != NULL) {
        dlclose(module_);
        module_ = NULL;
    }
}

bool RetrospectiveSdkBridge::IsReady() const {
    return started_ && module_ != NULL;
}

int RetrospectiveSdkBridge::OnMapStart(const char* map_name) const {
    if (!IsReady() || retro_on_map_start_ == NULL) {
        return -1;
    }
    return retro_on_map_start_(map_name);
}

int RetrospectiveSdkBridge::OnFrame(unsigned int dt_ms) const {
    if (!IsReady() || retro_frame_ == NULL) {
        return -1;
    }
    return retro_frame_(dt_ms);
}

int RetrospectiveSdkBridge::OnPlayerSpawn(unsigned int player_slot) const {
    if (!IsReady() || retro_on_player_spawn_ == NULL) {
        return -1;
    }
    return retro_on_player_spawn_(player_slot);
}

int RetrospectiveSdkBridge::OnPlayerDeath(unsigned int victim_slot, unsigned int attacker_slot) const {
    if (!IsReady() || retro_on_player_death_ == NULL) {
        return -1;
    }
    return retro_on_player_death_(victim_slot, attacker_slot);
}

unsigned int RetrospectiveSdkBridge::FrameCount() const {
    if (!IsReady() || retro_frame_count_ == NULL) {
        return 0;
    }
    return retro_frame_count_();
}

unsigned int RetrospectiveSdkBridge::PlayerSpawnCount() const {
    if (!IsReady() || retro_player_spawn_count_ == NULL) {
        return 0;
    }
    return retro_player_spawn_count_();
}

unsigned int RetrospectiveSdkBridge::PlayerDeathCount() const {
    if (!IsReady() || retro_player_death_count_ == NULL) {
        return 0;
    }
    return retro_player_death_count_();
}

unsigned int RetrospectiveSdkBridge::LastSpawnPlayerSlot() const {
    if (!IsReady() || retro_last_spawn_player_slot_ == NULL) {
        return 0;
    }
    return retro_last_spawn_player_slot_();
}

unsigned int RetrospectiveSdkBridge::LastDeathVictimSlot() const {
    if (!IsReady() || retro_last_death_victim_slot_ == NULL) {
        return 0;
    }
    return retro_last_death_victim_slot_();
}

unsigned int RetrospectiveSdkBridge::LastDeathAttackerSlot() const {
    if (!IsReady() || retro_last_death_attacker_slot_ == NULL) {
        return 0;
    }
    return retro_last_death_attacker_slot_();
}

const char* RetrospectiveSdkBridge::StatusText(int code) const {
    if (retro_status_text_ == NULL) {
        return "status_unavailable";
    }
    return retro_status_text_(code);
}

unsigned int RetrospectiveSdkBridge::ApiVersion() const {
    if (retro_api_version_ == NULL) {
        return 0;
    }
    return retro_api_version_();
}

const char* RetrospectiveSdkBridge::CoreId() const {
    if (retro_core_id_ == NULL) {
        return "core_unavailable";
    }
    return retro_core_id_();
}

bool RetrospectiveSdkBridge::BindSymbol(
    const char* symbol_name,
    void** out_fn,
    char* error_out,
    size_t error_out_len) {
    if (module_ == NULL || out_fn == NULL) {
        WriteError(error_out, error_out_len, "bind failed: invalid loader state");
        return false;
    }

    dlerror();
    *out_fn = dlsym(module_, symbol_name);
    const char* dlsym_error = dlerror();
    if (dlsym_error != NULL || *out_fn == NULL) {
        WriteError(error_out, error_out_len, symbol_name);
        return false;
    }
    return true;
}

void RetrospectiveSdkBridge::WriteError(char* error_out, size_t error_out_len, const char* message) {
    if (error_out == NULL || error_out_len == 0) {
        return;
    }

    if (message == NULL) {
        message = "unknown error";
    }

    const size_t max_copy = error_out_len - 1;
    const size_t src_len = strlen(message);
    const size_t copy_len = src_len < max_copy ? src_len : max_copy;
    memcpy(error_out, message, copy_len);
    error_out[copy_len] = '\0';
}
