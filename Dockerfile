FROM ubuntu:18.04

# Updating & installing needed packages
RUN apt-get update

# GL & X11 as needed dependencies
RUN apt-get install -y libglew-dev libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev
# CMake, GCC & Clang to build RaZ
RUN apt-get install -y cmake gcc clang
# Doxygen & Dot to generate the documentation
RUN apt-get install -y doxygen python-pydot python-pydot-ng

# Installing the FBX SDK
RUN wget http://download.autodesk.com/us/fbx/2019/2019.0/fbx20190_fbxsdk_linux.tar.gz -O ./fbx_sdk.tar.gz
RUN tar xvf ./fbx_sdk.tar.gz
RUN chmod +x ./fbx20190_fbxsdk_linux
# The installation asks for software user agreement (say 'yes'), then afterwards if the README file should be opened (say 'n')
RUN printf 'yes\nn' | ./fbx20190_fbxsdk_linux /usr

RUN mkdir /RaZ
WORKDIR /RaZ
