# Shamelessly copied and modified from https://hub.docker.com/r/tkelman/docker-mingw/~/dockerfile/
FROM debian:stretch
# LLVM has an undefined reference to SHGetKnownFolderPath when using
# pre-4.0 mingw-w64-x86_64-dev, so jessie is too old

RUN apt-get update -q && \
    DEBIAN_FRONTEND=noninteractive apt-get install -qy --no-install-recommends \
      ca-certificates git build-essential cmake mingw-w64 && \
      apt-get clean && rm -rf /var/lib/apt