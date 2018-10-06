extern crate futures;
extern crate hyper;
extern crate hyper_reverse_proxy;
extern crate tokio_core;

use futures::Stream;
use hyper::Client;
use hyper::server::Http;
use hyper_reverse_proxy::ReverseProxy;
use tokio_core::net::TcpListener;
use tokio_core::reactor::Core;
use std::net::{SocketAddr, Ipv4Addr};
use std::io::{self, Write};

fn main(){
    if let Err(error) = run() {
        write!(&mut io::stderr(), "{}", error).expect("Error writing to stdeer");
        std::process::exit(1);
    }
}

fn run() -> hyper::Result<()> {
    let mut core = Core::new()?;
    let handle = core.handle;
    let listen_addr = SocketAddr::new(Ipv4::new(127, 0, 0, 1).into(), 8080) ;
    let listener = TcpListener::bind(&listen_addr, &handle)?;
    let http = Http::new();

    let server = listener.incoming().for_each((socket, addr))| {
        let client = Client::new(&handle);
        let service = ReverseProxy::new(client, Some(addr.ip()));
        http.bind_connection(&handle, socket, addr, service);
        Ok(())
    };
    core.run(server)?;
    Ok(())
}

