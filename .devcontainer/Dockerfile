FROM gcc:latest

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
USER root

RUN apt-get update
RUN apt-get install -y build-essential curl zsh git wget cmake clang-format doxygen gdb libgtest-dev

RUN cd /usr/src/googletest && \
    cmake . && \
    cmake --build . --target install

RUN sh -c "$(wget -O- https://github.com/deluan/zsh-in-docker/releases/download/v1.1.2/zsh-in-docker.sh)" -- \
    -t robbyrussell \
    -p git

RUN apt install -y python3 python3-venv
RUN python3 -m venv "/root/.venv"
ENV PATH="${PATH}:/root/.venv/bin"
RUN pip install pre-commit
