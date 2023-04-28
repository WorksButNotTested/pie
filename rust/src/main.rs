#![no_std]
#![no_main]
#![feature(asm_experimental_arch)]

use {
    core::{arch::global_asm, panic::PanicInfo},
    libc::{c_void, write, STDOUT_FILENO},
};

#[no_mangle]
extern "C" fn unsafe_main(_argc: i32, _argv: *const *const i8) -> ! {
    unsafe {
        let bytes = b"HELLO\n\0";
        write(STDOUT_FILENO, bytes.as_ptr() as *const c_void, bytes.len());
        libc::_exit(0);
    }
}

global_asm!(
    ".section .init",
    "_start:",
    ".global _start",
    "bl unsafe_main"
);

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    unsafe {
        let bytes = b"PANIC\n\0";
        write(STDOUT_FILENO, bytes.as_ptr() as *const c_void, bytes.len());
        libc::_exit(0);
    }
}
