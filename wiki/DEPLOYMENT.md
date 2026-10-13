# Deployment & Operations

> **Status:** Outline / Planned  
> **Platform:** Linux (x86_64, aarch64)  
> **Source Directory:** `packaging/linux/`, `tools/`

## 1. Prerequisites and Toolchains

- Modern C++20 compliant compiler: GCC 13+ or Clang 17+
- CMake 3.22 or higher
- Ninja or Make build system
- SQLite 3.40+ development headers
- Cryptographic library (BLAKE3 / OpenSSL / Libsodium)
- Linux Kernel 5.10+ (supporting `inotify` and modern filesystem flags)

## 2. Build Configurations

### Debug Build (Sanitizers Enabled)
```bash
cmake --preset debug
cmake --build --preset debug
```

### Release Build (Optimized)
```bash
cmake --preset release
cmake --build --preset release
```

## 3. Background Daemon & Systemd Service

- User-level systemd unit file (`rewind.service`) for automatic background monitoring:
  ```ini
  [Unit]
  Description=Rewind Local File Recovery Daemon
  After=default.target

  [Service]
  ExecStart=%h/.local/bin/rewind daemon
  Restart=on-failure
  RestartSec=5s

  [Install]
  WantedBy=default.target
  ```

## 4. Packaging Targets

- Standalone relocatable tarball
- Debian package (`.deb`) via CPack
- RPM package (`.rpm`) via CPack

---
*Packaging manifests, installation scripts, and systemd units will be populated during release phases.*
