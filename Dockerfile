FROM ubuntu:18.04

# Updating packages' repo & installing needed packages:
#   - GL & X11 as needed graphical dependencies
#   - CMake, GCC & Clang to build RaZ
#   - Doxygen & Dot to generate the documentation
#   - Wget to download the FBX SDK
RUN apt-get update && apt-get install -y \
    libglew-dev libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev \
    cmake gcc-8 g++-8 clang-6.0 \
    doxygen python-pydot python-pydot-ng \
    wget

# Setting an alternative to map gcc & g++ to gcc-8 & g++-8
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 60 --slave /usr/bin/g++ g++ /usr/bin/g++-8 && \
    update-alternatives --install /usr/bin/clang clang /usr/bin/clang-6.0 60 --slave /usr/bin/clang++ clang++ /usr/bin/clang++-6.0

# Installing the FBX SDK
# The installation asks for software user agreement (send 'yes'), then afterwards if the README file should be opened (send 'n')
RUN wget http://download.autodesk.com/us/fbx/2019/2019.0/fbx20190_fbxsdk_linux.tar.gz -O ./fbx_sdk.tar.gz && \
    tar xvf ./fbx_sdk.tar.gz && \
    chmod +x ./fbx20190_fbxsdk_linux && \
    printf 'yes\nn' | ./fbx20190_fbxsdk_linux /usr

WORKDIR /RaZ
