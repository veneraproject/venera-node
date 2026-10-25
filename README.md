# Venera Node, RPC & CLI Wallet

**v0.2.2 · Ubuntu / Linux x86-64 · Windows x64**

Synchronize the Venera blockchain locally and manage VNR, BNB, USDT and USDC from a native commandline wallet. Wallet generation, key derivation and transaction signing take place locally.

This is the public **non-mining** build therefore solo mining is currently not available. Solo mining will become available in the next releases.

## Contents

- [Components and features](#components-and-features)
- [Dependencies](#dependencies)
- [Static build on Ubuntu](#static-build-on-ubuntu)
- [Static build on Windows](#static-build-on-windows)
- [Run the node and wallet](#run-the-node-and-wallet)
- [Wallet commands](#wallet-commands)
- [Burning, staking and rewards](#burning-staking-and-rewards)
- [Wallet RPC](#wallet-rpc)
- [Wallet files and security](#wallet-files-and-security)
- [Troubleshooting](#troubleshooting)
- [License](#license)

## Components and features

| Executable | Purpose |
| --- | --- |
| `venerad` | Downloads and verifies the Venera chain, connects to peers, exposes node RPC and relays VNR transactions. |
| `venera-rpc` | VNR wallet creation/restoration, scanning, signing and wallet JSON-RPC. The CLI manages it automatically. |
| `venera-cli` | Interactive VNR/BSC wallet with local signing and Venera rewards API integration. |

On Windows, executable names end in `.exe`.

Features include:

- Local wallet generation and restoration from an encrypted `.keys` file or an existing 25-word VNR mnemonic.
- A linked BSC account derived locally from the VNR spend key.
- VNR transfers and burns, native BNB transfers, and USDT/USDC token transfers on BSC mainnet.
- Cached VNR balances, password-authorized refreshes and a `current/target` wallet scan indicator.
- VNR history with transaction hashes, UTC timestamps, amounts, heights, confirmations and unlock information.
- Readable node status and colored command output.
- Password prompts for operations that reopen the wallet; the idle CLI retains public addresses and a balance snapshot.
- Staking registration, burn statistics and USDT burn-yield claims through the Venera rewards service.
- Configurable VNR node RPC through `set-node`. Remote nodes must use SSL in this version.

### Network services

| Service | Default | Role |
| --- | --- | --- |
| Venera seed peer | `120.25.94.32:29929` | P2P blockchain synchronization. |
| Local node RPC | `http://127.0.0.1:29930` | Blockchain access for wallets. |
| Managed wallet RPC | Available loopback HTTPS port | Temporary wallet operations; the CLI generates session credentials and a TLS identity. |
| BSC RPC | `https://bsc-dataseed.binance.org/` | BSC balances, gas estimates and signed-transaction broadcast. |
| Rewards API | `https://wallet.veneralabs.org/api/` | Burn/stake registration, statistics and yield claims. |

The BSC endpoint and supported token contracts are built into the production CLI. BNB is the native gas asset. Select **BNB Smart Chain / BSC mainnet, chain ID 56** when depositing BNB, USDT or USDC into this wallet.

VNR can use your locally synchronized blockchain. BSC queries use a public RPC service, and rewards use the Venera backend. These services receive the public information needed for their requests; the normal wallet flows do not send them seeds or private signing keys.

## Dependencies

Run build commands from the repository root containing `daemon/`, `cli/` and `scripts/`.

| Dependency | Used for |
| --- | --- |
| GCC/G++ with C++17 support | Native compilation. |
| CMake and Make or Ninja | Node/RPC builds. |
| Boost | Node and VNR wallet libraries. |
| OpenSSL | TLS and cryptographic operations. |
| libcurl and json-c | CLI HTTPS, JSON-RPC and JSON handling. |
| libsodium | Node/wallet cryptographic support. |
| libunbound and its dependencies | DNS support. |
| ZeroMQ | Node/wallet messaging support. |
| Python, Perl and archive utilities | Build tooling. |
| CA certificates | Public HTTPS verification. |

The Ubuntu dependency script currently selects OpenSSL **3.5.9**, curl **8.23.0** and ZeroMQ **4.3.5**. The Windows script builds private Boost **1.78.0** and uses native UCRT64 packages for other libraries. These are script-selected versions, rather than a statement that each is the latest upstream release.

Static builds require matching static archives for linked dependencies. Begin with two parallel jobs and increase only if memory permits. Allow disk space for dependency sources, build products and the growing blockchain database. Python is a build dependency; users do not need Python to run the finished wallet.

## Static build on Ubuntu

These scripts target **Linux x86-64**. Install the dependencies:

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake pkg-config python3 perl curl openssl ca-certificates \
  tar bzip2 xz-utils binutils file \
  libboost-all-dev libssl-dev libsodium-dev \
  libunbound-dev libevent-dev libexpat1-dev \
  libreadline-dev libncurses-dev libjson-c-dev nettle-dev libgmp-dev
```

Build the private dependencies, node, wallet RPC and fully static CLI:

```bash
export JOBS=2
bash scripts/build-static-deps.sh
bash scripts/build-daemon-static.sh
bash scripts/build-cli-static.sh full
```

The scripts configure their own build directories. A fresh source extraction does not need a previous CMake cache. Dependency downloads require outbound HTTPS to the upstream source hosts. The node/RPC script uses matching distribution static OpenSSL and unbound archives; the CLI uses the private curl/OpenSSL prefixes.

Outputs:

```text
build/daemon-static/bin/venerad
build/daemon-static/bin/venera-rpc
build/daemon-static/bin/venera-cli
```

### Linux CLI link modes

| Command | Result |
| --- | --- |
| `bash scripts/build-cli-static.sh full` | Fully static CLI link using glibc. Validate DNS/TLS on each target system. |
| `bash scripts/build-cli-static.sh hybrid` | Static curl, json-c, OpenSSL and C++ runtime libraries, with the host's dynamic glibc. Recommended when deploying the CLI across compatible Ubuntu systems. |

Both modes write the same CLI file. Switching CLI modes does not rebuild the node or wallet RPC.

A fully static glibc link can still depend on host name-resolution behavior. Warnings mentioning `getaddrinfo`, `getpwuid_r`, `gethostbyname` or `dlopen` are not necessarily link failures. A successful link does not establish runtime portability. If a fully static CLI fails on DNS/HTTPS, rebuild in `hybrid` mode and repeat the public connection check. Hybrid mode is **not fully static**. HTTPS still requires a current CA certificate store.

Check the build:

```bash
file build/daemon-static/bin/venerad \
     build/daemon-static/bin/venera-rpc \
     build/daemon-static/bin/venera-cli

./build/daemon-static/bin/venerad --version
./build/daemon-static/bin/venera-rpc --version
```

The scripts use baseline `-march=x86-64 -mtune=generic` flags for CPU portability. Static archive availability can vary between Ubuntu releases.

## Static build on Windows

Use **MSYS2 UCRT64** on Windows x64. These commands build native Windows executables.

1. Install [MSYS2](https://www.msys2.org/) and open its **UCRT64** terminal.
2. Update the complete installation:

   ```bash
   pacman -Syu
   ```

   If the update closes the terminal, reopen UCRT64 and repeat until the update completes. MSYS2 supports full system upgrades; partial updates can leave GCC runtime packages at incompatible revisions. See the official [update guide](https://www.msys2.org/docs/updating/).

3. Install dependencies:

   ```bash
   pacman -S --needed base-devel python curl tar bzip2 \
     mingw-w64-ucrt-x86_64-gcc \
     mingw-w64-ucrt-x86_64-binutils \
     mingw-w64-ucrt-x86_64-cmake \
     mingw-w64-ucrt-x86_64-ninja \
     mingw-w64-ucrt-x86_64-pkgconf \
     mingw-w64-ucrt-x86_64-openssl \
     mingw-w64-ucrt-x86_64-libsodium \
     mingw-w64-ucrt-x86_64-unbound \
     mingw-w64-ucrt-x86_64-zeromq \
     mingw-w64-ucrt-x86_64-curl \
     mingw-w64-ucrt-x86_64-json-c
   ```

4. Extract the source into a private NTFS folder under your Windows user profile. Use an ASCII directory name. Enter that folder in UCRT64; for example:

   ```bash
   cd /c/Users/YOUR_USER/Projects/venera-local-0.2.2
   ```

5. Build:

   ```bash
   bash scripts/build-windows.sh static
   ```

The script downloads and verifies the pinned Boost archive, builds its private prefix, configures the node/RPC and compiles the CLI. It explicitly selects x86-64 hashing sources and runs smoke checks, the CLI self-test and executable version checks before packaging.

| Output | Location |
| --- | --- |
| Node | `build/windows-static/bin/venerad.exe` |
| Wallet RPC | `build/windows-static/bin/venera-rpc.exe` |
| CLI | `build/windows-static/bin/venera-cli.exe` |

Static Windows executables still import normal Windows system DLLs. The packager rejects non-system DLL dependencies in static mode. If a dependency lacks a usable static archive, use:

```bash
bash scripts/build-windows.sh bundle
```

Bundle mode writes `build/windows-bundle/bin/` and `dist/venera-windows-x64-bundle.zip`, including required native application DLLs. Distribute the entire bundle together. The receiving computer does not need MSYS2 installed.

Before packaging the same mode again, move aside its existing `dist/venera-windows-x64-MODE/` directory. The packager refuses to overwrite a distribution directory. Keep Linux and Windows build caches and dependency prefixes separate.

## Run the node and wallet

### Ubuntu

From the repository root, start the node:

```bash
./build/daemon-static/bin/venerad \
  --data-dir "$HOME/venera-local-chain" \
  --rpc-bind-ip 127.0.0.1 --rpc-bind-port 29930 \
  --no-igd --hide-my-port
```

Leave it running. Open a second terminal at the repository root:

```bash
./build/daemon-static/bin/venera-cli
```

### Windows / PowerShell

From the repository root:

```powershell
.\build\windows-static\bin\venerad.exe `
  --data-dir "$env:USERPROFILE\venera-local-chain" `
  --rpc-bind-ip 127.0.0.1 --rpc-bind-port 29930 `
  --no-igd --hide-my-port
```

In another PowerShell terminal at the same root:

```powershell
.\build\windows-static\bin\venera-cli.exe
```

If using the packaged runtime, run `./venerad` and `./venera-cli` on Linux, or `.\venerad.exe` and `.\venera-cli.exe` on Windows, from the folder containing the executables.

**The CLI starts wallet RPC automatically; it does not start the blockchain node.** For local-chain use, keep `venerad` running. Node operators can run `venerad` alone without starting the CLI or wallet RPC.

### Create, open or restore a wallet

At startup, enter a wallet key filename, `create` or `import`:

| Input | Action |
| --- | --- |
| `create` | Choose a new filename and password. Generate a VNR wallet and linked BSC account. |
| `mywallet.keys` | Open an existing CLI key file with its password. |
| `import` | Choose a new filename/password and enter an existing 25-word VNR mnemonic. |

Without a matching cache, restoration scans from height zero and can take time.


### Choose a node or peer

Inside the CLI:

```text
set-node http://127.0.0.1:29930
refresh
```

For a remote node, use its hostname and a valid HTTPS certificate:

```text
set-node https://node.example.org:29930
refresh
```

Replace the example hostname with your node. Remote HTTP is rejected; loopback HTTP is permitted. `set-node` selects a **node RPC endpoint**, not a P2P peer.

To select a remote node before the first wallet launch, create `venera-local.json` in the directory from which you will launch the CLI:

```json
{"daemon_rpc":"https://node.example.org:29930"}
```

Use a node configured to serve HTTPS with a certificate trusted by your system.

To choose a blockchain peer, add `--add-priority-node HOST:29929` when starting `venerad`. To connect only to specified peers, use `--add-exclusive-node HOST:29929`. Replace `HOST` with the intended peer. The default seed is already built in.

To serve incoming P2P connections, use `--p2p-bind-ip 0.0.0.0 --p2p-bind-port 29929` and allow inbound TCP 29929 as appropriate. Node RPC can remain on loopback. Serving peers does not require exposing wallet RPC.

### Node console

Use `help` for commands and `help COMMAND` for syntax. The public console includes height/block/transaction queries, synchronization and peer information, bandwidth limits, bans, cache/transaction pool maintenance and `stop_node` for a normal shutdown.

Mining and block-template commands are unavailable. The compiled approval public key verifies signatures and does not grant block-approval authority.

## Wallet commands

Enter commands at the `venera>` prompt. Amounts are whole asset units, such as `1.25`, rather than atomic units. VNR input supports up to nine decimal places.

| Command | Purpose |
| --- | --- |
| `help` | List commands. |
| `status` | Show node height, status, version, burned VNR total and peer counts. |
| `balance` / `balance VNR` | Cached VNR balance, unlocked balance and scanned height. |
| `balance all` | VNR, BNB, USDT and USDC balances. |
| `balance BNB` / `balance USDT` / `balance USDC` | Query a BSC asset. |
| `refresh` | Password-authorized VNR scan and balance update; also retry queued reward registrations when present. |
| `receive` | Show VNR and BSC receiving addresses. |
| `receive VNR` / `receive USDT` | Show the requested address. BNB, USDT and USDC share the BSC address. |
| `send VNR ADDRESS AMOUNT` | Prepare and confirm a VNR transfer. |
| `send BNB ADDRESS AMOUNT` | Sign and broadcast a BNB transfer on BSC. |
| `send USDT ADDRESS AMOUNT` / `send USDC ADDRESS AMOUNT` | Sign and broadcast a BSC token transfer. |
| `history` | VNR history with transaction hashes. |
| `receipt 0xHASH` | Query a BSC transaction receipt. |
| `burn AMOUNT` | Prepare a VNR burn and queue reward registration. |
| `stake AMOUNT DAYS` | Staking-pool transfer for 30, 60, 120 or 360 days. |
| `burn_stats` / `stake_stats` | Request reward statistics. |
| `rewards_sync` | Retry saved registrations without sending funds again. |
| `register_burn TXID` | Register an existing burn transaction; recovery/retry only. |
| `register_stake TXID DAYS` | Register an existing staking transaction with its original duration; recovery/retry only. |
| `claim_burn_yield` | Request a USDT payout to the linked BSC address. |
| `claim_status [REQUEST_ID]` | Query a saved yield claim. |
| `seed` | Unlock and explicitly confirm display of the secret VNR mnemonic. |
| `set-node URL` | Save a node RPC endpoint; apply on the next refresh. |
| `quit` | Close the CLI. |

New VNR payments may not appear in the cached balance until `refresh`. A higher daemon height prompts a refresh; it does not silently unlock the wallet. BSC balances are queried with the public address without reopening its signing key.

Transfers require the password and explicit broadcast confirmation. BSC transfers show the recipient, contract, amount and gas estimate. Keep enough **BNB** for gas, including when sending USDT or USDC. `history` covers VNR; use the BSC transaction hash with `receipt` for BSC transfers.

One Ctrl+C warns instead of closing the wallet; a second consecutive Ctrl+C exits. `quit` exits directly.

## Burning, staking and rewards

These commands use the production rewards API. No additional software needs to be installed on the wallet user's computer.

### Burn rewards

```text
burn 6.1
```

The CLI obtains the burn policy, prepares the burn, asks for broadcast confirmation and saves its transaction reference for registration. Burning destroys VNR irreversibly. After the backend's required confirmations:

```text
rewards_sync
burn_stats
```

The backend verifies the burn and applies its multiplier, points and yield policy. Confirmation requirements and eligibility are backend policy, not a fixed CLI setting.

### Staking

```text
stake 3 30
```

The CLI retrieves the staking destination over HTTPS, prepares a normal VNR transfer, asks for broadcast confirmation and submits proof and the selected duration for registration. Durations are **30, 60, 120 and 360 days**.

```text
rewards_sync
stake_stats
```

Statistics display staked principal, expected yield and UTC maturity dates. This is a **pool-based service**: the pool receives the funds, and the backend/separate payout system handles maturity and returns. The chosen duration is a service record, rather than a protocol-enforced lock on the pool's funds.

### Claim burn yield

```text
claim_burn_yield
claim_status
```

The locally authorized claim includes the linked BSC receiving address. The backend treasury distributes USDT. Claims depend on accrued rewards.

If registration fails after broadcast, keep the transaction hash and run `rewards_sync` after confirmations. **Do not repeat a burn or stake to fix registration.** The `.keys.rewards.json` receipt file preserves pending registrations and claim references. Use `register_*` when recovering an existing transaction whose registration was not queued.

## Wallet RPC

Normal CLI use requires no manual RPC setup. The CLI generates random credentials and a private TLS identity, starts `venera-rpc` on loopback and stops it after the operation. The production CLI does not attach to arbitrary existing wallet RPC instances.

For custom local **VNR** JSON-RPC clients, wallet RPC can run separately. It requires an active node, explicit authentication, loopback binding and TLS certificate/private-key paths. BSC signing is implemented in the CLI, rather than wallet RPC.

Example Ubuntu setup for an independent local RPC client:

```bash
umask 077
mkdir -p "$HOME/.venera-rpc/wallets"
chmod 700 "$HOME/.venera-rpc" "$HOME/.venera-rpc/wallets"

openssl req -x509 -newkey rsa:3072 -sha256 -nodes \
  -keyout "$HOME/.venera-rpc/tls.key" \
  -out "$HOME/.venera-rpc/tls.crt" \
  -days 365 -subj "/CN=localhost" \
  -addext "subjectAltName=DNS:localhost,IP:127.0.0.1"

cat > "$HOME/.venera-rpc/rpc.conf" <<EOF
wallet-dir=$HOME/.venera-rpc/wallets
daemon-address=http://127.0.0.1:29930
rpc-bind-ip=127.0.0.1
rpc-bind-port=29931
rpc-login=wallet:$(openssl rand -hex 32)
rpc-ssl=enabled
rpc-ssl-certificate=$HOME/.venera-rpc/tls.crt
rpc-ssl-private-key=$HOME/.venera-rpc/tls.key
log-file=$HOME/.venera-rpc/rpc.log
log-level=0
EOF

./build/daemon-static/bin/venera-rpc \
  --config-file "$HOME/.venera-rpc/rpc.conf"
```

Configure the custom client to trust that certificate and use the credentials in the private config file. RPC credentials are distinct from wallet encryption passwords. Keep these files private and out of Git. Windows uses the same configuration options with Windows paths in a private directory.

Use `venera-rpc --help` for options. Missing credentials and attempts to disable authentication are rejected by this build.

## Wallet files and security

| File or directory | Purpose |
| --- | --- |
| `mywallet.keys` | Password-encrypted wallet backup containing the VNR mnemonic. |
| `venera-local.json` | CLI settings in the working directory, including the node endpoint. |
| Node `--data-dir` | Blockchain database and node state. |

The current key file uses AES-256-GCM with PBKDF2-HMAC-SHA256 and 600,000 iterations. The BSC signing key is derived from the VNR private spend key using domain-separated HMAC-SHA256.

Back up the encrypted `.keys` file **and its password**, or securely back up the VNR mnemonic. A forgotten file password cannot be reset. A separately backed-up mnemonic can be imported into a new file with a new password. Without the mnemonic or a decryptable backup, the accounts cannot be recovered.

Wallet operations use short-lived child processes, managed RPC sessions stop afterward and sensitive buffers are explicitly cleansed where implemented. Scanning and signing still require secrets in memory while unlocked; these measures do not protect against a compromised operating system. Store wallet files privately and keep all three executables together in a trusted installation folder.

Rewards requests disclose wallet addresses and required proofs to the Venera API.

## Troubleshooting

| Problem | Action |
| --- | --- |
| `could not load cache` | Start with the build script; it configures CMake before building. |
| Missing `libzmq.a` on Ubuntu | Run `build-static-deps.sh` before the node build. |
| Missing LDAP/Kerberos/HTTP2 archives in a Linux CLI link | Use the private minimal curl built by the dependency script. |
| OpenSSL undefined references | Use matching headers and archives; the CLI script links its private archives explicitly. |
| MSYS2 GCC runtime dependency conflicts | Complete `pacman -Syu`, reopen UCRT64 if requested and repeat before installing packages. |
| Boost tries to run `cl` | Use the updated Windows script, which invokes the engine builder directly with GCC. |
| Missing integer types or CryptoNight/RandomX symbols on Windows | Use the current sources with explicit integer headers and x86-64 source selection. |
| `Illegal instruction` after moving binaries | Rebuild affected dependencies with generic CPU flags; replace archives built with `-march=native`. |
| Remote node rejects HTTP | Use HTTPS with a valid certificate, or loopback HTTP for a local node. |
| VNR balance is outdated | Compare heights and run `refresh`. |
| Insufficient BNB | Fund BNB for gas, including for token transfers. |
| Pending reward registration | Wait for confirmations and run `rewards_sync`; preserve receipts and do not resend funds. |
| Treasury has insufficient USDT | Retry later; the backend treasury needs liquidity. |
| Invalid JSON from rewards API | Check availability and HTTP responses; proxies can return HTML/text errors. |
| Distribution folder already exists | Move aside `dist/venera-windows-x64-MODE/` before packaging again. |


## License

See [LICENSE](LICENSE) and bundled third-party notices. Preserve applicable copyright and license notices when redistributing sources or binaries, including the upstream Monero, CryptoNote and other bundled-library notices.
