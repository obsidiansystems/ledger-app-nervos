use crate::bindings::*;

#[repr(C)]
struct Version {
    major: u8,
    minor: u8,
    patch: u8,
}

extern "C" {
    static version: Version;
}

#[inline]
fn finalize_successful_send(mut tx: usize) -> usize {
    unsafe {
        G_io_apdu_buffer[tx] = 0x90;
    }
    tx += 1;
    unsafe {
        G_io_apdu_buffer[tx] = 0x00;
    }
    tx += 1;
    return tx;
}

#[no_mangle]
extern "C" fn handle_apdu_version(_instruction: u8) -> usize {
    let mut tx: usize = 0;
    unsafe {
        G_io_apdu_buffer[tx] = version.major;
    }
    tx += 1;
    unsafe {
        G_io_apdu_buffer[tx] = version.minor;
    }
    tx += 1;
    unsafe {
        G_io_apdu_buffer[tx] = version.patch;
    }
    tx += 1;
    unsafe {
        // padding
        G_io_apdu_buffer[tx] = 0;
    }
    tx += 1;
    finalize_successful_send(tx)
}
