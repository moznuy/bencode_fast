FROM debian:11-slim

RUN apt-get update
RUN apt-get install -y --no-install-suggests --no-install-recommends curl xz-utils ca-certificates

WORKDIR /tmp
ENV PYTHON_VERSION=3.11.1
RUN curl -sSL -o python.tar.xz https://www.python.org/ftp/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tar.xz
RUN tar -xf python.tar.xz

WORKDIR /tmp/Python-${PYTHON_VERSION}
RUN apt-get install -y --no-install-suggests --no-install-recommends \
    clang llvm lld pkg-config

RUN apt-get install -y --no-install-suggests --no-install-recommends \
    libssl-dev zlib1g-dev libbz2-dev liblzma-dev lzma-dev libreadline-dev libsqlite3-dev libncurses5-dev \
    libncursesw5-dev xz-utils tk-dev libgdbm-dev libgdbm-compat-dev libdb-dev libffi-dev uuid-dev

RUN ./configure \
    --prefix /opt/python \
    --with-pydebug \
    --with-trace-refs \
    --with-assertions \
    --without-pymalloc \
    --with-address-sanitizer \
#    --enable-optimizations \
    --enable-shared \
    --with-lto=no \
    --with-ensurepip=install

RUN make -j 16
RUN make install
ENV PATH="/opt/python/bin:$PATH"
CMD python3
