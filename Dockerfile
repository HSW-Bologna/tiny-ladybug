FROM debian:latest
RUN apt-get update
RUN apt-get -y install scons gcc-avr avr-libc avrdude
