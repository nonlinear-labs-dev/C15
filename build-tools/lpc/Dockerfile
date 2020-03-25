FROM ubuntu:19.10

ARG group_id 
ARG user 
ARG user_id

RUN groupadd -g $group_id $user 
RUN useradd -l -u $user_id -g $user $user
RUN apt-get update -y && apt-get install -y git gcc-arm-none-eabi cmake build-essential crossbuild-essential-armhf crossbuild-essential-armel pkg-config
