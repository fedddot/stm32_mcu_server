FROM esf-dev-image AS base_image

RUN apk add gcc-arm-none-eabi g++-arm-none-eabi newlib-arm-none-eabi gdb-multiarch
RUN apk add libbsd
RUN apk add py3-pip
RUN pip3 install --break-system-packages protobuf grpcio-tools

ARG EXTERNAL_PATH=/usr/src/external
WORKDIR ${EXTERNAL_PATH} 

RUN git clone --branch=v1.8.6 https://github.com/STMicroelectronics/STM32CubeF1.git stm32_cube_f1
RUN cd stm32_cube_f1 && git submodule update --init --recursive
ENV STM32_CUBE_SRC_PATH=${EXTERNAL_PATH}/stm32_cube_f1

RUN git clone --branch=v1.0.10 https://github.com/libdriver/ssd1306.git ssd1306
ENV SSD1306_PATH=${EXTERNAL_PATH}/ssd1306

# server sources should be mapped to this path during container run
WORKDIR /usr/src/app

ENV PATH=${PATH}:/usr/bin

CMD ["/bin/bash"]
