FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    qt6-base-dev \
    libqt6sql6-sqlite \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build

EXPOSE 44444

CMD ["./build/Server"]
