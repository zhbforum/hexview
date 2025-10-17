# hexview

[![CI](https://github.com/zhbforum/hexview/actions/workflows/ci.yml/badge.svg)](https://github.com/zhbforum/hexview/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/zhbforum/hexview?display_name=tag)](https://github.com/zhbforum/hexview/releases/latest)
[![License: MIT](https://img.shields.io/badge/License-MIT-black.svg)](./LICENSE)

> Tiny cross-platform hex viewer written in pure C99.  
> Displays file bytes in hexadecimal and ASCII columns with color, compact mode, and dynamic offset width.

<div align="center">
  <img src="demo.gif" alt="hexview demo" width="720">
</div>

---

## Features

- Offset and length selection: `-o 0x100 -l 256`
- Adjustable width: `-w 8|16|32`
- Optional ASCII panel: `--no-ascii`
- Compact output: `--compact`
- ANSI color support: `--color=auto|always|never`
- Reads from stdin using `-`
- Automatically adjusts offset column width (8–16 hex digits)

---

## Installation

### Prebuilt binaries
Download the latest release from [Releases](https://github.com/zhbforum/hexview/releases/latest):
- `hexview-windows-x86_64.zip`
- `hexview-linux-x86_64.tar.gz`
- `hexview-macos-x86_64.tar.gz`

### Build from source

#### Linux / macOS
```bash
git clone https://github.com/zhbforum/hexview
cd hexview
make
```

#### Windows (PowerShell)
```powershell
choco install -y make mingw
git clone https://github.com/zhbforum/hexview
cd hexview
make
```

---

## Usage

```text
hexview <file|-> [options]

Options:
  -w, --width  N      Bytes per line (default: 16, max: 64)
  -o, --offset N      Start offset (dec or 0xHEX) [not for stdin]
  -l, --length N      Max bytes to read (dec or 0xHEX)
      --no-ascii      Disable ASCII panel
      --compact       Compact hex (no spaces between bytes)
      --color[=MODE]  Colorize output: auto|always|never (default: auto)
  -v, --version       Print version and exit
```

### Examples

```bash
# basic
./hexview sample.bin

# compact + color
./hexview sample.bin --compact --color=always

# specific section
./hexview big.bin -o 0x100 -l 64 -w 16

# via stdin (Git Bash / Linux / macOS)
cat big.bin | ./hexview -

# via stdin (PowerShell)
cmd /c type big.bin | .\hexview.exe -
```

---



## Contributing

1. Fork → create a feature branch  
2. Run tests locally with `make test`  
3. Submit a PR with a clear description and example output

### Development quick start

```bash
make            # build
make test       # run smoke tests (tests/smoke.sh)
./hexview -v
```

---

## License

This project is licensed under the MIT License — see [LICENSE](./LICENSE) for details.
