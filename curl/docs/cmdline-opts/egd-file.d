Long: egd-file
Arg: <file>
Help: EGD socket path for random data
Protocols: TLS
See-also: random-file
Category: tls
Example: --egd-file /random/here $URL
Added: 7.7
---
Specify the path name to the Entropy Gathering Daemon socket. The socket is
used to seed the random engine for SSL connections.
