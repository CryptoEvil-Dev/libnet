FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Объединяем в один RUN. Это стандарт индустрии для Docker.
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Команда по умолчанию (если не переопределена в compose)
CMD ["cmake", "--workflow", "--preset", "build_all"]
