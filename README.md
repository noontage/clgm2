# CLGM

Lightweight WebSocket Server on mruby.


### Install

```
make production
```


## License

Is released under the MIT License.


## Requirement Library

CMake
And, this software is dependent uWebSocket library so need a related library is as follows.

- OpenSSL
- Zlib

```
Fedora: sudo dnf install openssl-devel zlib-devel cmake
Homebrew: brew install openssl zlib libuv cmake
Vcpkg: vcpkg install openssl zlib libuv and/or vcpkg install openssl:x64-windows zlib:x64-windows libuv:x64-windows
```