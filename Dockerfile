FROM ubuntu:18.04
MAINTAINER FYLE_SYSTEM

# my commands
RUN apt-get update \
  && apt-get install -y build-essential \
  && apt-get install -y net-tools \
  && apt-get install -y netcat \
  && apt-get clean \
  && apt-get -y upgrade \
  && rm -rf /var/lib/apt/lists/*

EXPOSE 80/tcp