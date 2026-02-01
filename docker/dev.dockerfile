FROM base

RUN apk add gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib gdb-base
RUN apk add bsdmainutils
RUN apk add python3-pip
RUN pip3 install --break-system-packages protobuf grpcio-tools

ARG EXTERNAL_PATH=/usr/src/external
WORKDIR ${EXTERNAL_PATH} 

RUN git clone --branch=v1.8.6 https://github.com/STMicroelectronics/STM32CubeF1.git stm32_cube_f1
RUN cd stm32_cube_f1 && git submodule update --init --recursive
ENV STM32_CUBE_SRC_PATH=${EXTERNAL_PATH}/stm32_cube_f1

RUN git clone --branch=epic/optimize-for-mbedded-systems https://github.com/fedddot/mcu_server.git mcu_server
ENV MCU_SERVER_PATH=${EXTERNAL_PATH}/mcu_server

# server sources should be mapped to this path during container run
WORKDIR /usr/src/app

ENV PATH=${PATH}:/usr/bin

CMD ["/bin/bash"]
