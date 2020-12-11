FROM ubuntu:20.04

# Defining the timezone to Europe/Paris, which is reqired by apt
# Updating packages' repo & installing only the needed packages:
#   - GL & X11 as needed graphical dependencies
#   - CMake, Make, GCC & Clang to build RaZ, and lcov to output code coverage
#   - xz-utils to uncompress Emscripten
#   - Python 3.X to use Emscripten
#   - Doxygen & Dot to generate the documentation
#   - OpenAL to handle the audio part
#   - Wget to download the FBX SDK
#   - Xvfb to launch a headless server (allows GL context initialisation without a screen)
#   - Rubygems to get the 'gem' command, used to download coveralls-lcov, and git to fetch info needed by coveralls-lcov
# Cleaning the apt lists & removing lists' cache entries to save image space
#   - See: https://docs.docker.com/develop/develop-images/dockerfile_best-practices/#run
# Downloading coveralls-lcov to upload coverage to Coveralls
RUN ln -snf /usr/share/zoneinfo/Europe/Paris /etc/localtime && echo Europe/Paris > /etc/timezone && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        libglew-dev libxi-dev libxcursor-dev libxrandr-dev libxinerama-dev libxxf86vm-dev \
        cmake make gcc-8 g++-8 clang-7 lcov \
        xz-utils \
        python3 \
        doxygen python3-pydot python3-pydot-ng \
        libopenal-dev \
        wget \
        xvfb \
        rubygems git && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    gem install coveralls-lcov

# Setting alternatives to map gcc, g++ & gcov to gcc-8, g++-8 & gcov-8, and clang & clang++ to clang-7 & clang++-7
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 60 --slave /usr/bin/g++ g++ /usr/bin/g++-8 && \
    update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-8 60 && \
    update-alternatives --install /usr/bin/clang clang /usr/bin/clang-7 60 --slave /usr/bin/clang++ clang++ /usr/bin/clang++-7

# Downloading & installing Emscripten, to build RaZ in WebAssembly
RUN git clone https://github.com/emscripten-core/emsdk.git && \
    ./emsdk/emsdk install latest && ./emsdk/emsdk activate latest

# Installing the FBX SDK
# The installation asks for software user agreement (send 'yes'), then afterwards if the README file should be opened (send 'n')
RUN wget http://download.autodesk.com/us/fbx/2019/2019.0/fbx20190_fbxsdk_linux.tar.gz -O ./fbx_sdk.tar.gz && \
    tar xvf ./fbx_sdk.tar.gz && \
    chmod +x ./fbx20190_fbxsdk_linux && \
    printf 'yes\nn' | ./fbx20190_fbxsdk_linux /usr

WORKDIR /RaZ
