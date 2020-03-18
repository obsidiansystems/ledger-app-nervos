use crate::bindings::*;

#[repr(C)]
#[derive(Clone, Copy)]
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
    tx
}

#[no_mangle]
extern "C" fn handle_apdu_version(_instruction: u8) -> usize {
    unsafe {
        *(&mut G_io_apdu_buffer as *mut _ as *mut Version) = version;
    }
    let tx: usize = core::mem::size_of::<Version>();
    finalize_successful_send(tx)
}
