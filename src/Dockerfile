FROM ubuntu:16.04

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install build-essential -y
RUN gcc -v 
RUN make -v 
RUN apt-get update
RUN apt-get install g++ valgrind -y
RUN apt-get update
RUN apt-get install libc6-dbg gdb valgrind -y
RUN apt-get update
RUN apt-get install libgsl0-dev -y
RUN apt-get install g++-4.8-multilib -y 
RUN apt-get update
RUN apt-get install libc6-i386 -y
RUN apt-get update




