#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."

 
 
mode=${1:-hybrid}
case "$mode" in
    full|hybrid) ;;
    *) echo 'Usage: bash scripts/build-cli-static.sh [full|hybrid]' >&2; exit 2 ;;
esac
curl_prefix=${CURL_STATIC_PREFIX:-"$PWD/build/curl-static-prefix"}
openssl_prefix=${OPENSSL_STATIC_PREFIX:-"$PWD/build/openssl-static-prefix"}
openssl_libdir="$openssl_prefix/lib"
if [[ ! -f "$openssl_libdir/libcrypto.a" && -f "$openssl_prefix/lib64/libcrypto.a" ]]; then
    openssl_libdir="$openssl_prefix/lib64"
fi
output="$PWD/build/daemon-static/bin/venera-cli"
command -v pkg-config >/dev/null
json_libdir=$(pkg-config --variable=libdir json-c)
curl_archive="$curl_prefix/lib/libcurl.a"
json_archive="$json_libdir/libjson-c.a"
ssl_archive="$openssl_libdir/libssl.a"
crypto_archive="$openssl_libdir/libcrypto.a"
for archive in "$curl_archive" "$json_archive" "$ssl_archive" "$crypto_archive"; do
    [[ -f "$archive" ]] || { echo "Missing static archive: $archive" >&2; exit 1; }
done
[[ -f "$openssl_prefix/include/openssl/opensslv.h" ]] || {
    echo "Missing OpenSSL headers under $openssl_prefix; run scripts/build-static-deps.sh first." >&2
    exit 1
}
mkdir -p "$(dirname "$output")"
 
 
json_cflags=$(pkg-config --cflags json-c)
read -r -a headers <<< "$json_cflags"
compiler=${CXX:-g++}
common=(-O2 -s -std=c++17 -march=x86-64 -mtune=generic -pthread
        -DOPENSSL_SUPPRESS_DEPRECATED -I"$openssl_prefix/include"
        -I"$curl_prefix/include" "${headers[@]}")
link_mode=(-static-libstdc++ -static-libgcc)
if [[ "$mode" == full ]]; then link_mode=(-static); fi
printf 'OpenSSL headers: %s\nOpenSSL archives: %s\n' "$openssl_prefix/include" "$openssl_libdir"
"$compiler" "${common[@]}" "${link_mode[@]}" \
    cli/venera_cli_local.cpp -o "$output" \
    "$curl_archive" "$json_archive" "$ssl_archive" "$crypto_archive" \
    -ldl -pthread -lm
"$output" --self-test
echo "Built $output ($mode)"
echo "Test public DNS/TLS without loading a wallet: $output --check-bsc-rpc"
for tool in file readelf; do command -v "$tool" >/dev/null; done
file "$output"
if [[ "$mode" == full ]] && readelf -l "$output" | grep -q INTERP; then
    echo 'Unexpected dynamic interpreter in fully static CLI' >&2; exit 1
fi
