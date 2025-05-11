# To create the image:
#   $ docker build -t z88dk -f z88dk.Dockerfile .
# To run the container:
#   $ docker run -v ${PWD}:/src/ -it z88dk <command>

FROM ubuntu:22.04

ENV PROCESSOR=RP2040
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /home/dev

RUN apt-get update \
    && apt-get update \
    && apt-get install -y git cmake gcc-arm-none-eabi python3 g++ vim

RUN cd /home/dev \
    && git clone https://github.com/raspberrypi/pico-examples \
    && mkdir pico-examples/build \
    && cd pico-examples/build/ \
    && export PICO_SDK_FETCH_FROM_GIT=1 \
    && cmake .. \
    && make -j8

RUN mkdir /home/dev/git \
    && cd /home/dev/git \
    && git clone https://github.com/raspberrypi/pico-sdk.git --branch master \
    && cd pico-sdk \
    && git submodule update --init 

ENV PICO_SDK_PATH="/home/dev/git/pico-sdk"
ENV PICOTOOL_FETCH_FROM_GIT_PATH="/home/dev/pico-examples/build/_deps/"

COPY tools /tmp/tools

RUN sed -i '1i add_compile_definitions(PICO_PANIC_FUNCTION=pkk_guru_meditation)' /home/dev/git/pico-sdk/pico_sdk_init.cmake

RUN cd /tmp/tools \
    && gcc bin2c.c -o /usr/local/bin/bin2c

WORKDIR /src/
