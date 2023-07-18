FROM gcc:latest

# Install project dependencies
RUN apt-get update; \
  apt-get install -y --no-install-recommends \
  wget \
  make \
  pkg-config \
  libjson-c-dev

# Install Maelstrom dependencies
RUN apt-get install -y --no-install-recommends \
  default-jre \
  default-jdk

# Set the working directory
WORKDIR /app

# Install Maelstrom
RUN wget https://github.com/jepsen-io/maelstrom/releases/download/v0.2.3/maelstrom.tar.bz2
RUN tar -xjf maelstrom.tar.bz2
