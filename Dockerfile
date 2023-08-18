FROM gcc:latest

# Set encoding
ENV LANG=C.UTF-8

# Install project dependencies
RUN apt-get update; \
  apt-get install -y --no-install-recommends \
  wget \
  make \
  pkg-config \
  valgrind \
  gdb \
  libjson-c-dev

# Install Maelstrom dependencies
RUN apt-get install -y --no-install-recommends \
  default-jre \
  default-jdk \
  gnuplot \
  graphviz

# Set the working directory
WORKDIR /app

# Install Maelstrom to root
RUN mkdir tmp
RUN wget -O tmp/maelstrom.tar.bz2 "https://github.com/jepsen-io/maelstrom/releases/download/v0.2.3/maelstrom.tar.bz2"
RUN tar -xvjf tmp/maelstrom.tar.bz2 -C / && rm tmp/maelstrom.tar.bz2
RUN echo "alias maelstrom='/maelstrom/maelstrom'" >> /root/.bashrc

# Install Unity testing framework
RUN wget -O tmp/unity.tar.bz2 "https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.5.2.tar.gz"
RUN tar -xvf tmp/unity.tar.bz2 -C / && rm tmp/unity.tar.bz2
RUN mv "../Unity-2.5.2" ../unity
