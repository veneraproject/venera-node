#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
compiler=${CC:-gcc}
archive() {
    local path
    path=$("$compiler" -print-file-name="$1")
    [[ "$path" = /* && -f "$path" ]] || { echo "Missing static archive $1; install its development package." >&2; exit 1; }
    printf '%s' "$path"
}
[[ -f build/zmq-static-prefix/lib/libzmq.a ]] || { echo 'Run bash scripts/build-static-deps.sh first.' >&2; exit 1; }
unbound=$(archive libunbound.a)
event=$(archive libevent.a)
expat=$(archive libexpat.a)
hogweed=$(archive libhogweed.a)
nettle=$(archive libnettle.a)
gmp=$(archive libgmp.a)
sodium=$(archive libsodium.a)
ssl=$(archive libssl.a)
crypto=$(archive libcrypto.a)
 
cmake -S daemon -B build/daemon-static \
    -DCMAKE_BUILD_TYPE=Release -DSTATIC=ON -DBUILD_SHARED_LIBS=OFF \
    -DARCH=default -DNO_AES=ON -DUSE_LTO=OFF -DSTACK_TRACE=OFF -DSTRIP_TARGETS=ON \
    -DCMAKE_C_FLAGS='-march=x86-64 -mtune=generic' \
    -DCMAKE_CXX_FLAGS='-march=x86-64 -mtune=generic' \
    -DCMAKE_C_FLAGS_RELEASE='-O2 -DNDEBUG' -DCMAKE_CXX_FLAGS_RELEASE='-O2 -DNDEBUG' \
    -DCMAKE_EXE_LINKER_FLAGS='-static' -DOPENSSL_USE_STATIC_LIBS=ON \
    -DOPENSSL_INCLUDE_DIR:PATH=/usr/include \
    -DOPENSSL_SSL_LIBRARY:FILEPATH="$ssl" -DOPENSSL_CRYPTO_LIBRARY:FILEPATH="$crypto" \
    -DUNBOUND_LIBRARIES:STRING="$unbound;$event;$expat;$hogweed;$nettle;$gmp" \
    -DSODIUM_LIBRARY:FILEPATH="$sodium" \
    -DVENERA_BLOCK_APPROVAL_HEIGHT=574969 \
    -DVENERA_EMISSION_CAP_HEIGHT=575928 \
    -DVENERA_BLOCK_APPROVAL_PUBKEY=807595e10f4d4a0c033434740dd14af448ea0315eaea07a19013a6e5bda2985d \
    -DUSE_DEVICE_TREZOR=OFF -DUSE_DEVICE_TREZOR_LIBUSB=OFF \
    -DZMQ_INCLUDE_PATH:PATH="$root/build/zmq-static-prefix/include" \
    -DZMQ_LIB:FILEPATH="$root/build/zmq-static-prefix/lib/libzmq.a" \
    -DGSSAPI_LIBRARY:FILEPATH='' -DPGM_LIBRARY:FILEPATH='' -DNORM_LIBRARY:FILEPATH='' \
    -DPROTOLIB_LIBRARY:FILEPATH='' -DPROTOKIT_LIBRARY:FILEPATH='' \
    -DBSD_LIBRARY:FILEPATH='' -DMD_LIBRARY:FILEPATH=''
cmake --build build/daemon-static --target daemon wallet_rpc_server -j"${JOBS:-2}"
for name in venerad venera-rpc; do
    file "build/daemon-static/bin/$name"
    if readelf -l "build/daemon-static/bin/$name" | grep -q INTERP; then
        echo "Unexpected dynamic interpreter in $name" >&2; exit 1
    fi
done
