#include "retrospective_loader.hpp"

#include <dlfcn.h>

namespace {
template <typename T>
bool bind_symbol(void* handle, const char* name, T& out_fn, std::string& error_out) {
    dlerror();
    out_fn = reinterpret_cast<T>(dlsym(handle, name));
    if (const char* err = dlerror(); err != nullptr) {
        error_out = std::string("missing symbol: ") + name + " (" + err + ")";
        return false;
    }
    return true;
}
} // namespace

RetrospectiveLoader::~RetrospectiveLoader() {
    unload();
}

bool RetrospectiveLoader::load(const std::string& lib_path, std::string& error_out) {
    unload();

    handle_ = dlopen(lib_path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (handle_ == nullptr) {
        const char* err = dlerror();
        error_out = err != nullptr ? err : "dlopen failed";
        return false;
    }

    if (!bind_symbol(handle_, "retro_api_version", retro_api_version_, error_out) ||
        !bind_symbol(handle_, "retro_core_id", retro_core_id_, error_out) ||
        !bind_symbol(handle_, "retro_init", retro_init_, error_out) ||
        !bind_symbol(handle_, "retro_shutdown", retro_shutdown_, error_out) ||
        !bind_symbol(handle_, "retro_frame", retro_frame_, error_out) ||
        !bind_symbol(handle_, "retro_on_map_start", retro_on_map_start_, error_out) ||
        !bind_symbol(handle_, "retro_on_player_spawn", retro_on_player_spawn_, error_out) ||
        !bind_symbol(handle_, "retro_on_player_death", retro_on_player_death_, error_out) ||
        !bind_symbol(handle_, "retro_frame_count", retro_frame_count_, error_out) ||
        !bind_symbol(handle_, "retro_player_spawn_count", retro_player_spawn_count_, error_out) ||
        !bind_symbol(handle_, "retro_player_death_count", retro_player_death_count_, error_out) ||
        !bind_symbol(handle_, "retro_last_spawn_player_slot", retro_last_spawn_player_slot_, error_out) ||
        !bind_symbol(handle_, "retro_last_death_victim_slot", retro_last_death_victim_slot_, error_out) ||
        !bind_symbol(handle_, "retro_last_death_attacker_slot", retro_last_death_attacker_slot_, error_out) ||
        !bind_symbol(handle_, "retro_status_text", retro_status_text_, error_out)) {
        unload();
        return false;
    }

    return true;
}

void RetrospectiveLoader::unload() {
    if (handle_ != nullptr) {
        dlclose(handle_);
        handle_ = nullptr;
    }

    retro_api_version_ = nullptr;
    retro_core_id_ = nullptr;
    retro_init_ = nullptr;
    retro_shutdown_ = nullptr;
    retro_frame_ = nullptr;
    retro_on_map_start_ = nullptr;
    retro_on_player_spawn_ = nullptr;
    retro_on_player_death_ = nullptr;
    retro_frame_count_ = nullptr;
    retro_player_spawn_count_ = nullptr;
    retro_player_death_count_ = nullptr;
    retro_last_spawn_player_slot_ = nullptr;
    retro_last_death_victim_slot_ = nullptr;
    retro_last_death_attacker_slot_ = nullptr;
    retro_status_text_ = nullptr;
}

bool RetrospectiveLoader::is_loaded() const {
    return handle_ != nullptr;
}

std::uint32_t RetrospectiveLoader::api_version() const {
    return retro_api_version_ != nullptr ? retro_api_version_() : 0;
}

const char* RetrospectiveLoader::core_id() const {
    return retro_core_id_ != nullptr ? retro_core_id_() : "unloaded";
}

std::int32_t RetrospectiveLoader::init() const {
    return retro_init_ != nullptr ? retro_init_() : -1;
}

void RetrospectiveLoader::shutdown() const {
    if (retro_shutdown_ != nullptr) {
        retro_shutdown_();
    }
}

std::int32_t RetrospectiveLoader::frame(std::uint32_t dt_ms) const {
    return retro_frame_ != nullptr ? retro_frame_(dt_ms) : -1;
}

std::int32_t RetrospectiveLoader::on_map_start(const char* map_name) const {
    return retro_on_map_start_ != nullptr ? retro_on_map_start_(map_name) : -1;
}

std::int32_t RetrospectiveLoader::on_player_spawn(std::uint32_t player_slot) const {
    return retro_on_player_spawn_ != nullptr ? retro_on_player_spawn_(player_slot) : -1;
}

std::int32_t RetrospectiveLoader::on_player_death(std::uint32_t victim_slot, std::uint32_t attacker_slot) const {
    return retro_on_player_death_ != nullptr ? retro_on_player_death_(victim_slot, attacker_slot) : -1;
}

std::uint32_t RetrospectiveLoader::frame_count() const {
    return retro_frame_count_ != nullptr ? retro_frame_count_() : 0;
}

std::uint32_t RetrospectiveLoader::player_spawn_count() const {
    return retro_player_spawn_count_ != nullptr ? retro_player_spawn_count_() : 0;
}

std::uint32_t RetrospectiveLoader::player_death_count() const {
    return retro_player_death_count_ != nullptr ? retro_player_death_count_() : 0;
}

std::uint32_t RetrospectiveLoader::last_spawn_player_slot() const {
    return retro_last_spawn_player_slot_ != nullptr ? retro_last_spawn_player_slot_() : 0;
}

std::uint32_t RetrospectiveLoader::last_death_victim_slot() const {
    return retro_last_death_victim_slot_ != nullptr ? retro_last_death_victim_slot_() : 0;
}

std::uint32_t RetrospectiveLoader::last_death_attacker_slot() const {
    return retro_last_death_attacker_slot_ != nullptr ? retro_last_death_attacker_slot_() : 0;
}

const char* RetrospectiveLoader::status_text(std::int32_t code) const {
    return retro_status_text_ != nullptr ? retro_status_text_(code) : "unloaded";
}
