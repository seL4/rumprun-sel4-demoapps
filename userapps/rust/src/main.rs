//
// Copyright 2017, Data61
// Commonwealth Scientific and Industrial Research Organisation (CSIRO)
// ABN 41 687 119 230.
//
// This software may be distributed and modified according to the terms of
// the BSD 2-Clause license. Note that NO WARRANTY is provided.
// See "LICENSE_BSD2.txt" for details.
//
// @TAG(DATA61_BSD)
//
/* Originally sourced from https://avacariu.me/articles/2015/rust-echo-server-example */

use std::net::{TcpListener, TcpStream};
use std::thread;

// traits
use std::io::Read;
use std::io::Write;

fn handle_client(mut stream: TcpStream) {
    let mut buf;
    loop {
        // clear out the buffer so we don't send garbage
        buf = [0; 512];
        let _ = match stream.read(&mut buf) {
            Err(e) => panic!("Got an error: {}", e),
            Ok(m) => {
                if m == 0 {
                    // we've got an EOF
                    break;
                }
                m
            }
        };

        match stream.write(&buf) {
            Err(_) => break,
            Ok(_) => continue,
        }
    }
}

fn main() {
    let listener = TcpListener::bind("0.0.0.0:8888").unwrap();
    for stream in listener.incoming() {
        match stream {
            Err(e) => println!("failed: {}", e),
            Ok(stream) => {
                thread::spawn(move || handle_client(stream));
            }
        }
    }
}
