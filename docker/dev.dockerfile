FROM esf-dev-image

RUN apk add gcc-arm-none-eabi
RUN apk add newlib-arm-none-eabi
RUN apk add gdb
RUN apk add util-linux
RUN apk add python3 py3-pip
RUN pip3 install --break-system-packages protobuf grpcio-tools

# server sources should be mapped to this path during container run
WORKDIR /usr/src/app

ENV PATH=${PATH}:/usr/bin

ENTRYPOINT [ "sh" ]
