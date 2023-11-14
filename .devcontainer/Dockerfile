# Use a Microsoft C++ base image
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-buster

# Install necessary tools for Pico development
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    python3

# Clone the Pico SDK
RUN git clone https://github.com/raspberrypi/pico-sdk.git /pico-sdk

# Initialize and update submodules for the Pico SDK
RUN cd /pico-sdk && \
    git submodule update --init

ENV PICO_SDK_PATH=/pico-sdk

# Set the working directory
WORKDIR /workspace
