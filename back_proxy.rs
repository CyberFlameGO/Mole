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

error_chain! {
    foreign_links {
        Io(std::io::Error);
        Hyper(hyper::Error);
    }
}

fn main(){
    if let Err(error) = run() {
        write!(&mut io::stderr(), "{}", error).expect("Error writing to stdeer");
        std::process::exit(1);
    }
}


fn shutdown_future(handle: &Handle) -> BoxFuture<(), std::io::Error> {
    use tokio_signal::unix::{Signal, SIGINT, SIGTERM};
    let sigint = Signal::net(SIGINT, handle).flatten_stream();
    let sigterm = Signal::new(SIGTERM, handle).flatten_stream();

    Stream::select(sigint, sigterm).into_future().map(|_| ()).map_err(|(e,_)| e).boxed()
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
    let shutdown = shutdown_future(&handle);
    match core.run(Future::select(shutdown, server)){
        Ok(((), _next)) => {}
        Err((error, _next)) => bail!(error),
         shutdown_future(&handle) => println!("Shutdown due to error!")
    }

    struct WaitUntilZero { info: Rc<RefCell<Info>>, }

    impl Future for WaitUntilZero {
        type Item = ();
        type Error = std::io::Error;

        fn poll(&mut self) -> futures::Poll<(), std::io::Error> {
            use futures::Async;
            let mut info = self.info.borrow_mut();

            if info.active == 0 {
                Ok(Async::Ready(()))
            } else {
                info.blocker = Some(task::current());
                Ok(Async::NotReady)
            }
        }
    }

    let timeout = Timeout::new(Duration::from_secs(20), &handle)?;
    let wait = WaitUntilZero{info: info.clone() };

    match core.run(Future::select(wait, timeout)){
        Ok(((), _next)) => Ok(()),
        Err((error, _next)) => Err(error.into()),
    }
}

quick_main!(run);


