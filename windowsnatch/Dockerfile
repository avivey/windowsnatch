# This should be your UID from the host, so files are created with you as owner:
ARG USER_ID=1000


FROM ubuntu

RUN apt-get update && apt-get install -y \
    make \
    mingw-w64 \
  && rm -rf /var/lib/apt/lists/*

ARG USER_ID
RUN adduser --uid ${USER_ID} --gecos '' \
  --disabled-password --home /workdir \
  builder
WORKDIR /workdir
USER ${USER_ID}:${USER_ID}
