FROM ubuntu:22.04

# Установка зависимости
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    qt6-base-dev \
    libqt6network6 \
    && rm -rf /var/lib/apt/lists/*

# Копирование исходников
COPY . /app
WORKDIR /app

# Сборка проекта
RUN cmake -B build && cmake --build build

# Открытие порта
EXPOSE 44444

# Запуск сервера
CMD ["./build/Server"]
