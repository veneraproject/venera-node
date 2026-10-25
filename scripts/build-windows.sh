#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
[[ ${MSYSTEM:-} == UCRT64 ]] || { echo 'Run in MSYS2 UCRT64 on Windows x64.' >&2; exit 1; }
mode=${1:-bundle}
case "$mode" in bundle|static) ;; *) echo 'Usage: bash scripts/build-windows.sh [bundle|static]' >&2; exit 2;; esac
for tool in cmake ninja g++ gcc pkg-config objdump curl tar windres; do command -v "$tool" >/dev/null; done
root=$PWD
root_native=$(cygpath -m "$root")
prefix=$(cygpath -m "$MINGW_PREFIX")
boost="$root/build/windows-boost-prefix"
boost_native=$(cygpath -m "$boost")
mkdir -p build/downloads build/windows-deps-src
boost_archive=build/downloads/boost_1_78_0.tar.bz2
if [[ ! -s $boost_archive ]]; then
    curl --fail --location --retry 3 --proto '=https' https://archives.boost.io/release/1.78.0/source/boost_1_78_0.tar.bz2 -o "$boost_archive.part"
    mv "$boost_archive.part" "$boost_archive"
fi
printf '%s  %s\n' 8681f175d4bdb26c52222665793eef08490d7758529330f98d3b29dd0735bccc "$boost_archive" | sha256sum -c -
if [[ ! -f $boost/.venera-boost-178-complete ]]; then
    tar -xf "$boost_archive" -C build/windows-deps-src
    (
        cd build/windows-deps-src/boost_1_78_0
        (
            cd tools/build/src/engine
            MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c "build.bat gcc"
        )
        cp tools/build/src/engine/b2.exe ./b2.exe
        ./b2.exe -j"${JOBS:-2}" toolset=gcc target-os=windows architecture=x86 address-model=64 \
            variant=release link=static runtime-link=shared threading=multi threadapi=win32 \
            --layout=system --prefix="$boost_native" \
            cxxflags='-std=c++14 -O2 -march=x86-64 -mtune=generic' \
            --with-system --with-filesystem --with-thread --with-date_time --with-chrono \
            --with-regex --with-serialization --with-program_options --with-locale \
            boost.locale.icu=off boost.locale.iconv=off install
    )
    touch "$boost/.venera-boost-178-complete"
fi
build="build/windows-$mode"
static=OFF
suffix=.dll.a
link_flags=''
cli_defines=()
platform_defines=''
if [[ $mode == static ]]; then
    static=ON;suffix=.a;link_flags=-static
    cli_defines=(-DCURL_STATICLIB -DJSON_C_STATIC)
    platform_defines=-DSODIUM_STATIC
fi
for stem in ssl crypto unbound zmq sodium; do
    [[ -f "$MINGW_PREFIX/lib/lib$stem$suffix" ]] || { echo "Missing $MINGW_PREFIX/lib/lib$stem$suffix" >&2; exit 1; }
done
pkg-config --atleast-version=8.13 libcurl
unbound="$prefix/lib/libunbound$suffix"
zmq="$prefix/lib/libzmq$suffix"
if [[ $mode == static ]]; then
    read -r -a unbound_extra <<< "$(pkg-config --libs --static libunbound)"
    for item in "${unbound_extra[@]}"; do
        case "$item" in -lunbound|-L*) ;; *) unbound+=";$item";; esac
    done
    read -r -a zmq_extra <<< "$(pkg-config --libs --static libzmq)"
    for item in "${zmq_extra[@]}"; do
        case "$item" in -lzmq|-L*) ;; *) zmq+=";$item";; esac
    done
fi
cmake -S daemon -B "$build" -G Ninja \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_POLICY_DEFAULT_CMP0167=OLD \
    -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DSTATIC="$static" \
    -DARCH=default -DARCH_ID=x86_64 -DNO_AES=ON -DUSE_LTO=OFF -DSTACK_TRACE=OFF -DSTRIP_TARGETS=ON -DUSE_READLINE=OFF \
    -DCMAKE_C_FLAGS="-march=x86-64 -mtune=generic $platform_defines" \
    -DCMAKE_CXX_FLAGS="-march=x86-64 -mtune=generic -Wa,-mbig-obj $platform_defines" \
    -DBOOST_ROOT:PATH="$boost_native" -DBOOST_INCLUDEDIR:PATH="$boost_native/include" \
    -DBOOST_LIBRARYDIR:PATH="$boost_native/lib" -DBoost_NO_SYSTEM_PATHS=ON \
    -DBoost_USE_STATIC_LIBS=ON -DBoost_USE_STATIC_RUNTIME=OFF \
    -DOPENSSL_INCLUDE_DIR:PATH="$prefix/include" \
    -DOPENSSL_SSL_LIBRARY:FILEPATH="$prefix/lib/libssl$suffix" \
    -DOPENSSL_CRYPTO_LIBRARY:FILEPATH="$prefix/lib/libcrypto$suffix" \
    -DUNBOUND_INCLUDE_DIR:PATH="$prefix/include" -DUNBOUND_LIBRARIES:STRING="$unbound" \
    -DZMQ_INCLUDE_PATH:PATH="$prefix/include" -DZMQ_LIB:STRING="$zmq" \
    -DMINIUPNP_INCLUDE_DIR:PATH="$root_native/daemon/external/miniupnp/miniupnpc" \
    -DMINIUPNP_LIBRARY:STRING=libminiupnpc-static \
    -DSODIUM_LIBRARY:FILEPATH="$prefix/lib/libsodium$suffix" \
    -DGSSAPI_LIBRARY:FILEPATH='' -DPGM_LIBRARY:FILEPATH='' -DNORM_LIBRARY:FILEPATH='' \
    -DPROTOLIB_LIBRARY:FILEPATH='' -DPROTOKIT_LIBRARY:FILEPATH='' -DBSD_LIBRARY:FILEPATH='' -DMD_LIBRARY:FILEPATH=''
cmake --build "$build" --target daemon wallet_rpc_server -j"${JOBS:-2}"
icon_objects=()
if [[ -f daemon/resources/venera.ico ]]; then
    windres -i "$build/venera.rc" -O coff -o "$build/venera-icon.o"
    icon_objects=("$build/venera-icon.o")
fi
read -r -a cli_headers <<< "$(pkg-config --cflags libcurl json-c openssl)"
if [[ $mode == static ]]; then
    read -r -a cli_libraries <<< "$(pkg-config --libs --static libcurl json-c openssl)"
else
    read -r -a cli_libraries <<< "$(pkg-config --libs libcurl json-c openssl)"
fi
g++ -O2 -s -std=c++17 -march=x86-64 -mtune=generic -pthread -Wa,-mbig-obj \
    -DOPENSSL_SUPPRESS_DEPRECATED -DNOMINMAX -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=0x0602 \
    "${cli_defines[@]}" "${cli_headers[@]}" ${link_flags:+"$link_flags"} \
    cli/venera_cli_local.cpp "${icon_objects[@]}" -o "$build/bin/venera-cli.exe" \
    "${cli_libraries[@]}" -ladvapi32 -lws2_32
"$build/bin/venera-cli.exe" --self-test
"$build/bin/venerad.exe" --version
"$build/bin/venera-rpc.exe" --version
queue=("$build/bin/venerad.exe" "$build/bin/venera-rpc.exe" "$build/bin/venera-cli.exe")
declare -A checked_dlls=()
for ((index=0; index<${#queue[@]}; ++index)); do
    binary=${queue[index]}
    imports=$(objdump -p "$binary")
    dlls=$(awk '/DLL Name:/ {print $3}' <<< "$imports")
    while read -r dll; do
        [[ -n $dll ]] || continue
        lower=${dll,,}
        case "$lower" in
            api-ms-*.dll|ext-ms-*.dll|kernel32.dll|kernelbase.dll|ntdll.dll|ucrtbase.dll|msvcrt.dll|advapi32.dll|bcrypt.dll|bcryptprimitives.dll|crypt32.dll|cryptbase.dll|dnsapi.dll|gdi32.dll|iphlpapi.dll|mswsock.dll|ncrypt.dll|normaliz.dll|ole32.dll|oleaut32.dll|powrprof.dll|psapi.dll|rpcrt4.dll|secur32.dll|setupapi.dll|shell32.dll|shlwapi.dll|user32.dll|userenv.dll|version.dll|winhttp.dll|wininet.dll|winmm.dll|winspool.dll|ws2_32.dll|wtsapi32.dll|wldap32.dll|wintrust.dll|dbghelp.dll|comctl32.dll|dwmapi.dll) continue ;;
            msys-2.0.dll|cygwin1.dll) echo "Non-native Windows runtime dependency: $dll" >&2; exit 1 ;;
        esac
        [[ $mode != static ]] || { echo "Unexpected non-system DLL in static build: $dll" >&2; exit 1; }
        [[ $dll =~ ^[A-Za-z0-9_.+-]+\.[Dd][Ll][Ll]$ ]] || { echo "Invalid DLL import: $dll" >&2; exit 1; }
        [[ -z ${checked_dlls[$lower]:-} ]] || continue
        checked_dlls[$lower]=1
        [[ -f "$MINGW_PREFIX/bin/$dll" ]] || { echo "Missing native runtime DLL: $dll" >&2; exit 1; }
        cp "$MINGW_PREFIX/bin/$dll" "$build/bin/$dll"
        queue+=("$build/bin/$dll")
    done <<< "$dlls"
done
printf 'Built node, RPC and CLI in %s/bin\n' "$build"
