use core::ffi::c_char;
use std::ffi::CStr;
use std::sync::atomic::{AtomicBool, AtomicU32, Ordering};

static INITIALIZED: AtomicBool = AtomicBool::new(false);
static FRAME_COUNT: AtomicU32 = AtomicU32::new(0);

const CORE_ID: &[u8] = b"retrospective_core/0.1.0\0";
const STATUS_OK: &[u8] = b"ok\0";
const STATUS_NOT_INITIALIZED: &[u8] = b"core_not_initialized\0";
const STATUS_NULL_MAP: &[u8] = b"null_map_name\0";

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
    FRAME_COUNT.store(0, Ordering::SeqCst);
    0
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_shutdown() {
    INITIALIZED.store(false, Ordering::SeqCst);
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_frame(dt_ms: u32) -> i32 {
    if !INITIALIZED.load(Ordering::SeqCst) {
        return -1;
    }

    let _ = dt_ms;
    FRAME_COUNT.fetch_add(1, Ordering::SeqCst);
    0
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn retro_on_map_start(map_name: *const c_char) -> i32 {
    if !INITIALIZED.load(Ordering::SeqCst) {
        return -1;
    }
    if map_name.is_null() {
        return -2;
    }

    // Validate map name pointer and encoding; behavior wiring comes later.
    let _ = unsafe { CStr::from_ptr(map_name) };
    FRAME_COUNT.store(0, Ordering::SeqCst);
    0
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_frame_count() -> u32 {
    FRAME_COUNT.load(Ordering::SeqCst)
}

#[unsafe(no_mangle)]
pub extern "C" fn retro_status_text(code: i32) -> *const c_char {
    match code {
        0 => STATUS_OK.as_ptr().cast(),
        -1 => STATUS_NOT_INITIALIZED.as_ptr().cast(),
        -2 => STATUS_NULL_MAP.as_ptr().cast(),
        _ => b"unknown\0".as_ptr().cast(),
    }
}

