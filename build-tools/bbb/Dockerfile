FROM ubuntu:16.04

ENV LANG en_US.UTF-8

RUN apt-get update -y && apt-get install -y git locales && sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && dpkg-reconfigure --frontend=noninteractive locales && update-locale LANG=en_US.UTF-8
RUN apt-get update -y && apt-get install -y git cmake build-essential wget cpio python unzip bc doxygen curl libcurl4-openssl-dev bash ncurses-dev nano
