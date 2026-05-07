use core::ffi::c_char;
use std::ffi::CStr;
use std::sync::atomic::{AtomicBool, AtomicU32, Ordering};

static INITIALIZED: AtomicBool = AtomicBool::new(false);
static FRAME_COUNT: AtomicU32 = AtomicU32::new(0);
static PLAYER_SPAWN_COUNT: AtomicU32 = AtomicU32::new(0);
static PLAYER_DEATH_COUNT: AtomicU32 = AtomicU32::new(0);
static LAST_SPAWN_PLAYER_SLOT: AtomicU32 = AtomicU32::new(0);
static LAST_DEATH_VICTIM_SLOT: AtomicU32 = AtomicU32::new(0);
static LAST_DEATH_ATTACKER_SLOT: AtomicU32 = AtomicU32::new(0);

const CORE_ID: &[u8] = b"retrospective_core/0.1.0\0";
const STATUS_OK: &[u8] = b"ok\0";
const STATUS_NOT_INITIALIZED: &[u8] = b"core_not_initialized\0";
const STATUS_NULL_MAP: &[u8] = b"null_map_name\0";
const STATUS_INVALID_PLAYER_SLOT: &[u8] = b"invalid_player_slot\0";
const STATUS_INVALID_VICTIM_SLOT: &[u8] = b"invalid_victim_slot\0";

const RETRO_OK: i32 = 0;
const RETRO_ERR_NOT_INITIALIZED: i32 = -1;
const RETRO_ERR_NULL_MAP: i32 = -2;
const RETRO_ERR_INVALID_PLAYER_SLOT: i32 = -3;
const RETRO_ERR_INVALID_VICTIM_SLOT: i32 = -4;

fn reset_runtime_state() {
    FRAME_COUNT.store(0, Ordering::SeqCst);
    PLAYER_SPAWN_COUNT.store(0, Ordering::SeqCst);
    PLAYER_DEATH_COUNT.store(0, Ordering::SeqCst);
    LAST_SPAWN_PLAYER_SLOT.store(0, Ordering::SeqCst);
    LAST_DEATH_VICTIM_SLOT.store(0, Ordering::SeqCst);
    LAST_DEATH_ATTACKER_SLOT.store(0, Ordering::SeqCst);
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_api_version() -> u32 {
    1
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_core_id() -> *const c_char {
    CORE_ID.as_ptr().cast()
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_init() -> i32 {
    INITIALIZED.store(true, Ordering::SeqCst);
    reset_runtime_state();
    RETRO_OK
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_shutdown() {
    reset_runtime_state();
    INITIALIZED.store(false, Ordering::SeqCst);
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_frame(dt_ms: u32) -> i32 {
    if !INITIALIZED.load(Ordering::SeqCst) {
        return RETRO_ERR_NOT_INITIALIZED;
    }

    let _ = dt_ms;
    FRAME_COUNT.fetch_add(1, Ordering::SeqCst);
    RETRO_OK
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn retro_on_map_start(map_name: *const c_char) -> i32 {
    if !INITIALIZED.load(Ordering::SeqCst) {
        return RETRO_ERR_NOT_INITIALIZED;
    }
    if map_name.is_null() {
        return RETRO_ERR_NULL_MAP;
    }

    // Validate map name pointer and encoding; behavior wiring comes later.
    let _ = unsafe { CStr::from_ptr(map_name) };
    reset_runtime_state();
    RETRO_OK
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_on_player_spawn(player_slot: u32) -> i32 {
    if !INITIALIZED.load(Ordering::SeqCst) {
        return RETRO_ERR_NOT_INITIALIZED;
    }
    if player_slot == 0 {
        return RETRO_ERR_INVALID_PLAYER_SLOT;
    }

    PLAYER_SPAWN_COUNT.fetch_add(1, Ordering::SeqCst);
    LAST_SPAWN_PLAYER_SLOT.store(player_slot, Ordering::SeqCst);
    RETRO_OK
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_on_player_death(victim_slot: u32, attacker_slot: u32) -> i32 {
    if !INITIALIZED.load(Ordering::SeqCst) {
        return RETRO_ERR_NOT_INITIALIZED;
    }
    if victim_slot == 0 {
        return RETRO_ERR_INVALID_VICTIM_SLOT;
    }

    PLAYER_DEATH_COUNT.fetch_add(1, Ordering::SeqCst);
    LAST_DEATH_VICTIM_SLOT.store(victim_slot, Ordering::SeqCst);
    LAST_DEATH_ATTACKER_SLOT.store(attacker_slot, Ordering::SeqCst);
    RETRO_OK
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_frame_count() -> u32 {
    FRAME_COUNT.load(Ordering::SeqCst)
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_player_spawn_count() -> u32 {
    PLAYER_SPAWN_COUNT.load(Ordering::SeqCst)
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_player_death_count() -> u32 {
    PLAYER_DEATH_COUNT.load(Ordering::SeqCst)
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_last_spawn_player_slot() -> u32 {
    LAST_SPAWN_PLAYER_SLOT.load(Ordering::SeqCst)
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_last_death_victim_slot() -> u32 {
    LAST_DEATH_VICTIM_SLOT.load(Ordering::SeqCst)
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_last_death_attacker_slot() -> u32 {
    LAST_DEATH_ATTACKER_SLOT.load(Ordering::SeqCst)
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_status_text(code: i32) -> *const c_char {
    match code {
        RETRO_OK => STATUS_OK.as_ptr().cast(),
        RETRO_ERR_NOT_INITIALIZED => STATUS_NOT_INITIALIZED.as_ptr().cast(),
        RETRO_ERR_NULL_MAP => STATUS_NULL_MAP.as_ptr().cast(),
        RETRO_ERR_INVALID_PLAYER_SLOT => STATUS_INVALID_PLAYER_SLOT.as_ptr().cast(),
        RETRO_ERR_INVALID_VICTIM_SLOT => STATUS_INVALID_VICTIM_SLOT.as_ptr().cast(),
        _ => b"unknown\0".as_ptr().cast(),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::ffi::{CStr, CString};
    use std::sync::Mutex;

    static TEST_LOCK: Mutex<()> = Mutex::new(());

    fn status(code: i32) -> &'static str {
        let ptr = retro_status_text(code);
        unsafe { CStr::from_ptr(ptr) }
            .to_str()
            .expect("status text should be UTF-8")
    }

    #[test]
    fn lifecycle_requires_init() {
        let _guard = TEST_LOCK.lock().expect("test lock");
        retro_shutdown();

        assert_eq!(retro_frame(16), RETRO_ERR_NOT_INITIALIZED);
        assert_eq!(retro_on_player_spawn(1), RETRO_ERR_NOT_INITIALIZED);
        assert_eq!(retro_on_player_death(1, 0), RETRO_ERR_NOT_INITIALIZED);
        assert_eq!(
            unsafe { retro_on_map_start(std::ptr::null()) },
            RETRO_ERR_NOT_INITIALIZED
        );
        assert_eq!(status(RETRO_ERR_NOT_INITIALIZED), "core_not_initialized");

        retro_shutdown();
    }

    #[test]
    fn map_start_and_events_reset_and_track_state() {
        let _guard = TEST_LOCK.lock().expect("test lock");
        retro_shutdown();

        assert_eq!(retro_init(), RETRO_OK);
        assert_eq!(retro_frame(16), RETRO_OK);
        assert_eq!(retro_on_player_spawn(2), RETRO_OK);
        assert_eq!(retro_on_player_death(2, 4), RETRO_OK);
        assert_eq!(retro_frame_count(), 1);
        assert_eq!(retro_player_spawn_count(), 1);
        assert_eq!(retro_player_death_count(), 1);
        assert_eq!(retro_last_spawn_player_slot(), 2);
        assert_eq!(retro_last_death_victim_slot(), 2);
        assert_eq!(retro_last_death_attacker_slot(), 4);

        let first_map = CString::new("a1_experimental_01").expect("cstring");
        assert_eq!(unsafe { retro_on_map_start(first_map.as_ptr()) }, RETRO_OK);
        assert_eq!(retro_frame_count(), 0);
        assert_eq!(retro_player_spawn_count(), 0);
        assert_eq!(retro_player_death_count(), 0);
        assert_eq!(retro_last_spawn_player_slot(), 0);
        assert_eq!(retro_last_death_victim_slot(), 0);
        assert_eq!(retro_last_death_attacker_slot(), 0);

        assert_eq!(retro_frame(16), RETRO_OK);
        assert_eq!(retro_frame(16), RETRO_OK);
        assert_eq!(retro_on_player_spawn(5), RETRO_OK);
        assert_eq!(retro_on_player_death(5, 0), RETRO_OK);
        assert_eq!(retro_frame_count(), 2);
        assert_eq!(retro_player_spawn_count(), 1);
        assert_eq!(retro_player_death_count(), 1);
        assert_eq!(retro_last_spawn_player_slot(), 5);
        assert_eq!(retro_last_death_victim_slot(), 5);
        assert_eq!(retro_last_death_attacker_slot(), 0);

        retro_shutdown();
    }

    #[test]
    fn event_validation_and_status_texts() {
        let _guard = TEST_LOCK.lock().expect("test lock");
        retro_shutdown();

        assert_eq!(retro_init(), RETRO_OK);
        assert_eq!(
            unsafe { retro_on_map_start(std::ptr::null()) },
            RETRO_ERR_NULL_MAP
        );
        assert_eq!(retro_on_player_spawn(0), RETRO_ERR_INVALID_PLAYER_SLOT);
        assert_eq!(retro_on_player_death(0, 7), RETRO_ERR_INVALID_VICTIM_SLOT);
        assert_eq!(status(RETRO_ERR_NULL_MAP), "null_map_name");
        assert_eq!(status(RETRO_ERR_INVALID_PLAYER_SLOT), "invalid_player_slot");
        assert_eq!(status(RETRO_ERR_INVALID_VICTIM_SLOT), "invalid_victim_slot");

        retro_shutdown();
    }
}
