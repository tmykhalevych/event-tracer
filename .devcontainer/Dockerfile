FROM gcc:latest

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt-get update
RUN apt-get install -y build-essential curl zsh git wget cmake clang-format doxygen gdb libgtest-dev

RUN cd /usr/src/googletest && \
    cmake . && \
    cmake --build . --target install

RUN sh -c "$(wget -O- https://github.com/deluan/zsh-in-docker/releases/download/v1.1.2/zsh-in-docker.sh)" -- \
    -t robbyrussell \
    -p git
