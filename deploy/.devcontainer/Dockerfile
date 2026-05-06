FROM ubuntu:26.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    curl \
    wget \
    git \
    vim \
    nano \
    sudo \
    zsh \
    bash-completion \
    build-essential \
    cmake \
    ninja-build \
    gdb \
    clang \
    clangd \
    lldb \
    pkg-config \
    unzip \
    zip \
    tar \
    xz-utils \
    lsb-release \
    software-properties-common \
    locales \
    && rm -rf /var/lib/apt/lists/*

# 设置 UTF-8
RUN locale-gen en_US.UTF-8

ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

WORKDIR /workspace