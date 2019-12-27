
use core::mem;
use primesieve_sys::primesieve_iterator;
use primesieve_sys::primesieve_init;
use primesieve_sys::primesieve_next_prime;
use primesieve_sys::primesieve_free_iterator;
use radix_fmt::radix_36;
//use std::io::{self, Write};
use std::fmt::Write;
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
    //println!("{} {}", offset, s.len());
    for _i in 0..=(s.len() - 15) {
        if is_palindrome15(&s[_i.._i+15]) {
            println!("{}", _i + offset);
            if let Ok(s) = std::str::from_utf8(&s[_i.._i+15]) {
                println!("{}", s);
            }
            return true;
        }
        //println!("{}", _i);
    }
    return false;
}

fn main()
{
    let mut pi: primesieve_iterator = unsafe { mem::zeroed() };

    unsafe {
        primesieve_init(&mut pi);
    }

    let mut offset: usize = 0;
    let mut s = String::new();
    for _ in 1..=1000 {
        println!("{} ...", offset);
        for _ in 1..=1000 {
            for _ in 1..=10000 {
                let x: u64 = unsafe { primesieve_next_prime(&mut pi) };
                
                write!(&mut s, "{}", radix_36(x));
                
                //print!("{}", radix_36(x));
                //io::stdout().write_all(radix_36(x).to_string().as_bytes());
            }
            //println!("{}", s);
            if find_palindrome(s.as_bytes(), offset) {
                process::exit(0);
            }
            let len = s.len();
            offset += len - 14;
            let tail = String::from(&s[len-14..len]);
            s.replace_range(0..14, tail.as_str());
            s.truncate(14);
        }
    }

    unsafe {
        primesieve_free_iterator(&mut pi);
    }
}

