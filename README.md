# Mole

Mole is a very simple tunnelling cli that uses TUN, TAP and TCP to tunnel through IPv4 and ARP. It also uses Rust to 
handle a reserve proxy, this ensure that the headers are stripped correctly in both directions and adds in the clients IP
to then forward the address header.

# Need to know

While it uses basic socket address imported from linux they have been slightly changed (see mole.c) to allow the process
to be executed a bit easier.

This project does work, however not efficiently. This may not work on your server, it was built around a personal server.