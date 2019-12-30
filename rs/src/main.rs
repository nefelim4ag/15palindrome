
use core::mem;
use primesieve_sys::primesieve_iterator;
use primesieve_sys::primesieve_init;
use primesieve_sys::primesieve_next_prime;
use primesieve_sys::primesieve_free_iterator;
use std::process;

fn is_palindrome15(ptr: &[u8]) -> bool
{
    return ptr[0] == ptr[14]
        && ptr[1] == ptr[13]
        && ptr[2] == ptr[12]
        && ptr[3] == ptr[11]
        && ptr[4] == ptr[10]
        && ptr[5] == ptr[9]
        && ptr[6] == ptr[8];
}

fn find_palindrome(s: &[u8], offset: usize) -> bool
{
    for _i in 0..=(s.len() - 15) {
        if is_palindrome15(&s[_i.._i+15]) {
            println!("{}", _i + offset);
            if let Ok(s) = std::str::from_utf8(&s[_i.._i+15]) {
                println!("{}", s);
            }
            return true;
        }
    }
    return false;
}

fn base36(mut ptr: *mut u8, x: u64) -> *mut u8
{
    static A: [u8; 36] = [
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
        65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
        75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
        85, 86, 87, 88, 89, 90
    ];

    let z = x / 36;
    let y = (x % 36) as usize;
    if z > 0 {
        ptr = base36(ptr, z);
    }
    unsafe {
        *ptr = A[y];
    };
    return unsafe { ptr.add(1) };
}


fn main()
{
    let mut pi: primesieve_iterator = unsafe { mem::zeroed() };

    unsafe {
        primesieve_init(&mut pi);
    }

    let mut offset: usize = 0;
    let mut _s: [u8; 1024*128] = [0; 1024*128];
    let _start: *mut u8 = &mut _s[0];
    let mut ptr: *mut u8 = _start;
    for _ in 1..=1000 {
        //println!("{} ...", offset);
        for _ in 1..=1000 {
            for _ in 1..=10000 {
                let x: u64 = unsafe { primesieve_next_prime(&mut pi) };
                ptr = base36(ptr, x);
            }
            let s = unsafe { std::slice::from_raw_parts_mut(_start, ptr as usize - _start as usize) };
            if find_palindrome(s, offset) {
                process::exit(0);
            }
            offset += s.len() - 14;
            s.copy_within(s.len()-14..s.len(), 0);
            ptr = unsafe{ _start.add(14) };
        }
    }

    unsafe {
        primesieve_free_iterator(&mut pi);
    }
}

