#!/usr/bin/env bash
 
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
jobs=${JOBS:-2}
[[ $(uname -m) == x86_64 ]] || { echo 'These flags target Linux x86-64.' >&2; exit 1; }
mkdir -p build/downloads build/deps-src
fetch() {
    local url=$1 target=$2
    if [[ ! -s "$target" ]]; then
        curl --fail --location --retry 3 --proto '=https' --tlsv1.2 "$url" -o "$target.part"
        mv "$target.part" "$target"
    fi
}
fetch https://github.com/openssl/openssl/releases/download/openssl-3.5.9/openssl-3.5.9.tar.gz "$root/build/downloads/openssl-3.5.9.tar.gz"
fetch https://github.com/openssl/openssl/releases/download/openssl-3.5.9/openssl-3.5.9.tar.gz.sha256 "$root/build/downloads/openssl-3.5.9.tar.gz.sha256"
expected=$(awk '{print $1; exit}' "$root/build/downloads/openssl-3.5.9.tar.gz.sha256")
[[ $expected =~ ^[0-9a-fA-F]{64}$ ]] || { echo 'Invalid OpenSSL checksum response' >&2; exit 1; }
printf '%s  %s\n' "$expected" "$root/build/downloads/openssl-3.5.9.tar.gz" | sha256sum -c -
if [[ ! -f build/openssl-static-prefix/lib/libcrypto.a ]]; then
    tar -xf build/downloads/openssl-3.5.9.tar.gz -C build/deps-src
    (
        cd build/deps-src/openssl-3.5.9
        ./Configure linux-x86_64 no-shared no-module no-dso no-engine no-tests no-zlib \
            --prefix="$root/build/openssl-static-prefix" --libdir=lib \
            -O2 -march=x86-64 -mtune=generic
        make -j"$jobs"
        make install_sw
    )
fi
fetch https://github.com/zeromq/libzmq/releases/download/v4.3.5/zeromq-4.3.5.tar.gz "$root/build/downloads/zeromq-4.3.5.tar.gz"
if [[ ! -f build/zmq-static-prefix/lib/libzmq.a ]]; then
    tar -xf build/downloads/zeromq-4.3.5.tar.gz -C build/deps-src
    cmake -S build/deps-src/zeromq-4.3.5 -B build/zmq-static \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$root/build/zmq-static-prefix" \
        -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_SHARED=OFF -DBUILD_STATIC=ON -DBUILD_TESTS=OFF \
        -DENABLE_CURVE=OFF -DENABLE_DRAFTS=OFF -DWITH_LIBSODIUM=OFF \
        -DWITH_GSSAPI_KRB5=OFF -DWITH_OPENPGM=OFF -DWITH_NORM=OFF -DWITH_LIBBSD=OFF \
        -DCMAKE_C_FLAGS='-O2 -march=x86-64 -mtune=generic' \
        -DCMAKE_CXX_FLAGS='-O2 -march=x86-64 -mtune=generic'
    cmake --build build/zmq-static -j"$jobs"
    cmake --install build/zmq-static
fi
fetch https://curl.se/download/curl-8.23.0.tar.xz "$root/build/downloads/curl-8.23.0.tar.xz"
if [[ ! -f build/curl-static-prefix/lib/libcurl.a ]]; then
    tar -xf build/downloads/curl-8.23.0.tar.xz -C build/deps-src
    (
        cd build/deps-src/curl-8.23.0
        export PKG_CONFIG_PATH="$root/build/openssl-static-prefix/lib/pkgconfig"
        export CPPFLAGS="-I$root/build/openssl-static-prefix/include"
        export LDFLAGS="-L$root/build/openssl-static-prefix/lib"
        export CFLAGS='-O2 -march=x86-64 -mtune=generic'
        ./configure --prefix="$root/build/curl-static-prefix" --libdir="$root/build/curl-static-prefix/lib" \
            --disable-shared --enable-static --with-openssl="$root/build/openssl-static-prefix" \
            --disable-ldap --disable-ldaps --without-gssapi --without-librtmp \
            --without-libidn2 --without-libpsl --without-brotli --without-zstd --without-zlib \
            --without-nghttp2 --without-nghttp3 --without-ngtcp2 --without-libssh --without-libssh2 \
            --disable-ares --enable-threaded-resolver \
            --with-ca-bundle=/etc/ssl/certs/ca-certificates.crt --with-ca-path=/etc/ssl/certs
        make -j"$jobs"
        make install
    )
fi
printf 'Dependency prefixes are ready under %s/build\n' "$root"
