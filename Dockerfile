FROM ubuntu:18.04

# Updating packages' repo
RUN apt-get update

# Installing needed packages:
#   - GL & X11 as needed graphical dependencies
#   - CMake, GCC & Clang to build RaZ
#   - Doxygen & Dot to generate the documentation
#   - Wget to download the FBX SDK
RUN apt-get install -y \
    libglew-dev libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev \
    cmake gcc-8 clang-6.0 \
    doxygen python-pydot python-pydot-ng \
    wget

# Installing the FBX SDK
RUN wget http://download.autodesk.com/us/fbx/2019/2019.0/fbx20190_fbxsdk_linux.tar.gz -O ./fbx_sdk.tar.gz
RUN tar xvf ./fbx_sdk.tar.gz
RUN chmod +x ./fbx20190_fbxsdk_linux
# The installation asks for software user agreement (say 'yes'), then afterwards if the README file should be opened (say 'n')
RUN printf 'yes\nn' | ./fbx20190_fbxsdk_linux /usr

# Adding /usr/bin to the PATH variable so that compilers are recognized easily
RUN export PATH=$PATH:/usr/bin

# Creating symlinks for g++, as it is not named as such
RUN ln -s /usr/bin/g++-8 /usr/bin/g++

RUN mkdir /RaZ
WORKDIR /RaZ
