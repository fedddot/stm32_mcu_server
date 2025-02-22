FROM mcu-server-dev:latest

# pico-sdk deps
RUN apt install -y gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib gdb-multiarch

# install externals
ARG EXTERNAL_PATH=/usr/src/external
WORKDIR ${EXTERNAL_PATH} 

RUN git clone --branch=discard-factories https://github.com/fedddot/mcu_server.git mcu_server
ENV MCU_SERVER_PATH=${EXTERNAL_PATH}/mcu_server

RUN git clone --branch=v1.8.6 https://github.com/STMicroelectronics/STM32CubeF1.git stm32_cube_f1
RUN cd stm32_cube_f1 && git submodule update --init --recursive
ENV STM32_CUBE_F1_SRC_PATH=${EXTERNAL_PATH}/stm32_cube_f1

RUN git clone --branch=0.4.9 https://github.com/nanopb/nanopb.git nanopb
ENV NANOPB_SRC_PATH=${EXTERNAL_PATH}/nanopb

RUN apt install -y python3-pip
RUN pip3 install protobuf grpcio-tools
RUN apt-get install bsdmainutils

# server sources should be mapped to this path during container run
WORKDIR /usr/src/app

CMD ["/bin/bash"]
