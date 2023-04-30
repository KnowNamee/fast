FROM ubuntu:20.04

RUN apt-get update && \
    DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata

RUN apt-get install -y software-properties-common
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN apt-get install -y wget rsync

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
RUN add-apt-repository -y "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-14 main"

RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | apt-key add -
RUN apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'

RUN apt-get update

RUN apt-get install -y \
    ssh \
    make \
    cmake \
    build-essential \
    linux-tools-common \
    linux-tools-generic

RUN apt-get install -y clang-14
RUN apt-get install -y clang-format-14
RUN apt-get install -y clang-tidy-14
RUN apt-get install -y libclang-rt-14-dev
RUN apt-get install -y libc++-14-dev
RUN apt-get install -y libc++abi-14-dev
RUN apt-get install -y clangd-14
RUN apt-get install -y gdb
RUN apt-get install -y binutils-dev
RUN apt-get install -y libdwarf-dev
RUN apt-get install -y libdw-dev
RUN apt-get install -y python3
RUN apt-get install -y ca-certificates
RUN apt-get install -y openssh-server
RUN apt-get install -y vim
RUN apt-get install -y autoconf
