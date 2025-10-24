# README.txt

A brief explanation of how the program works.

## Client-Side
Both the TCP and UDP clients read in the port number and IP address from the user. Then, in each one, they create a socket, and attempt to bind it to the specified port and IP address.
Note that to specify the UDP protocol upon binding, the UDP client specifies type SOCK_DGRAM, and to specify TCP protocol, the TCP client specifies SOCK_STREAM.

Both clients use a variable called `server` of type `struct hostent` to store information about a particular host. 

Both clients use struct of type `sockaddr_in` to specify a socket address, and this has variable name `serv_addr`. `sin_family` is set to AF_INET to specify IPv4, `sin_addr.s_addr` specifies the IP address of the server, and `sin_port` specifies the port number. We use `htons` to convert the port number to the correct endian (host and network may use different endians).

The TCP client establishes a connection before attempting to send data, whereas the UDP client does not. This is because TCP is a connection-based protocol and UDP is connectionless.

Both clients prompt the user for a string, and stores it in a buffer of size 129. This is because we want 128 characters, and the last character is null termination character, \0, by default.

Afterwards, both clients send the data to the server (TCP client uses `send` and UDP client uses `sendto`), attempt to retrieve a response from the server.

To retrieve the response, the UDP client uses `recv_from` and the TCP client uses `recv`.

The server will either return "Sorry, cannot compute!" or a sequence of integers (sums) separated by a newline. Therefore, both clients iterate through the server response, and prints it accordingly. Each line of server response is prepended by "From server:", and there is a newline between each server response.

## Server-Side
Both servers take in one command line argument, the port number it listens on.

It then declares a buffer of 129 characters, since as specified in the instructions, there is a maximum of 128 characters, and the last character is a null termination character.

It then creates a socket, with both servers specifying AF_INET (IPV4), the UDP server specifying `SOCK_DRAM` and the TCP server specifying `SOCK_STREAM` to specify UDP and TCP protocols respectively.

In a similar fashion to the client files, the servers leverage variables `server_addr, client_addr` of type `socketaddr_in` to  specify the socket address. The servers bind themselves to the localhost IP address and the specified port, similar to how the client binds its socket.

The TCP server listens at that socket, waiting for clients to connect with, whereas since UDP is connectionless, there is no listening happening for the UDP server. When a client connects with the TCP server, it has to accept the connection, whereas for the UDP server, it does not need to do anything, since it is connectionless.

Both servers then attempt to receive a message from the client. TCP uses `recv`, whereas UDP uses `recvfrom` to do this.

Upon reception of the client's message, both servers ensure that the buffer ends with \0 and ensure any newlines are replaced with \0, for downstream parsing tasks.

If the message is not all numerical digits, the server returns message "Sorry, cannot compute!"

Otherwise, it calculates the sum of all the digits and returns it, and continues this process with the previous sum, until the sum is a single digit. To do this, it defines a variable named result of size 129 bytes (result is guaranteed to be smaller than the input string). Then, we iteratively save the sum into a variable called temp of size 6 bytes (largest possible sum is 9*128 = 1152 which is 4 characters and therefore 4 bytes, temp has a newline and a termination character). We then save the temp variable into the result variable, since result stores all the sums. 

We continue this process until the sum is a single digit (less than 10). Then, we save the single digit sum, and send the result to the client. UDP uses `sendto` and TCP uses `sent`. Afterwards, the TCP server closes the client connection.

Note that this all occurs in a while loop, since the server is supposed to remain active indefinitely. This way, multiple clients can connect to the server.


## References
https://man7.org/linux/man-pages/man3/sockaddr.3type.html
https://man7.org/linux/man-pages/man7/ip.7.html
https://en.cppreference.com/w/c/io/fgets
https://pubs.opengroup.org/onlinepubs/009695099/functions/listen.html
https://learn.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-hostent