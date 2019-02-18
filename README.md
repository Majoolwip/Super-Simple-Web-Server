# Super Simple Web Server #
---
## Description ##
This is a super simple web server (as the name implies) that I made to host my
personal website. "Why do that?" you may ask, well KNOWLEDGE! Simply I wanted
make something in C and I thought this would be a good way to make a program
and C and get a refresher on Linux system programming. This is a server made
for programmers as it is configured via modifing config.h in the header file.
This goes along with the philosophy behind suckless.org. The idea is to have as
little code as possible as the more code you have the more you program will
suck (and have bugs).

## Build ##
Simply configure config.h and execute:
> make server

## Testing ##
To test the server, build the test program via:
> make tester
This will make a program that will search your web-server's directory and send
get requests for every file and directory to make sure it returns only what you
want to return.

It also will execute some unit tests on the individual functions in server.c
