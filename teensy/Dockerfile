ARG ARDUINO_VERSION=1.8.8
ARG TEENSYDUINO_VERSION=145
# This should be your UID from the host, so files are created with you as owner:
ARG USER_ID=1000

FROM ubuntu as setup

RUN apt-get update && apt-get install -y \
    libfontconfig \
    libx11-6 \
    libxft2 \
    wget \
    xz-utils \
  && rm -rf /var/lib/apt/lists/*

ARG ARDUINO_VERSION
RUN wget --quiet -O arduino.tar.xz \
    https://downloads.arduino.cc/arduino-${ARDUINO_VERSION}-linux64.tar.xz && \
  tar xJf arduino.tar.xz && rm arduino.tar.xz && \
  mv arduino-${ARDUINO_VERSION}/ /arduino

ARG TEENSYDUINO_VERSION
ARG PJRC_ROOT=https://www.pjrc.com/teensy
RUN wget --quiet \
    ${PJRC_ROOT}/td_${TEENSYDUINO_VERSION}/TeensyduinoInstall.linux64 && \
  chmod +x TeensyduinoInstall.linux64 && \
  ./TeensyduinoInstall.linux64 --dir=/arduino && \
  rm TeensyduinoInstall.linux64


FROM ubuntu

RUN apt-get update && apt-get install -y \
    make \
  && rm -rf /var/lib/apt/lists/*

COPY --from=setup /arduino/hardware/tools/arm /usr/share/arm
ENV PATH "/usr/share/arm/bin:${PATH}"


ARG USER_ID
RUN adduser --uid ${USER_ID} --gecos '' \
  --disabled-password --home /workdir \
  builder
WORKDIR /workdir
USER ${USER_ID}:${USER_ID}
