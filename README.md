# Fsociety MR-Toolkit & i3 Environment

> 🌐 **Language:** **English** | [Русский (README_Ru.md)](README_Ru.md)

A custom ecosystem of system, network, and desktop utilities (`mr-*`) themed after **Mr. Robot / Fsociety**, built for a Linux **i3wm + Polybar + Rofi + Alacritty** workflow.

## 📂 Repository Structure

- **`bin/`** — 21 scripts and utilities (Python and Bash) for managing the system, local network, files, and desktop environment:
  - `mr-dashboard` — central interactive TUI hub featuring live resource monitoring and quick tool launching.
  - `mr-help` — dynamic documentation engine that automatically parses docstrings and headers across all `mr-*` utilities.
  - `mr-bridge.py` — zero-config LAN P2P bridge (UDP clipboard sync and TCP file transfer between desktop and laptop).
  - `mr-banner.py` — network service and open-port banner inspector.
  - `mr-portcheck`, `mr-pinger`, `mr-dirbuster` — rapid network diagnostics launched in floating terminal windows.
  - `stealth-mode` — LAN anonymity script (MAC address randomization and hostname spoofing).
  - `mr-look` — fast terminal directory navigator and file previewer.
  - `mr-project` — Rofi project launcher that automatically activates Python `venv` environments and spawns `fish` inside Alacritty.
  - `mr-net`, `mr-mixer`, `mr-clip`, `mr-wallpaper`, `mr-menu`, `mr-shot`, `mr-lock`, `mr-sysinfo` — desktop applets for Wi-Fi, PipeWire audio, clipboard, wallpapers, screenshots, and power management.
  - `mr-matrix` — `curses`-based digital rain terminal simulation supporting the signature Fsociety red theme (`--red`).
- **`src/`** — low-level **C++17** system utilities:
  - `mr-shadow.cpp` — ephemeral RAM sandbox powered by `Bubblewrap` (`bwrap`), mounting `/home`, `/tmp`, and `/run` inside `tmpfs` (RAM) while binding `/` as Read-Only with optional network isolation (`--no-net`).
  - `mr-netscan.cpp` — multi-threaded interactive TCP/ICMP local network and port scanner.
- **`i3/`**, **`polybar/`**, **`rofi/`**, **`alacritty/`** — desktop environment configuration files and custom `mrrobot.rasi` Rofi theme.
- **`ascii/`** — ASCII art collection (`~/.mr-ascii`).

## 🛠️ Building C++ Utilities & Installation

```bash
# Compile C++17 tools
g++ -O2 -std=c++17 src/mr-shadow.cpp -o ~/.local/bin/mr-shadow
g++ -O2 -std=c++17 -pthread src/mr-netscan.cpp -o ~/.local/bin/mr-netscan

# Install scripts
cp bin/* ~/.local/bin/
chmod +x ~/.local/bin/mr-* ~/.local/bin/stealth-mode

# Copy environment configs
cp -r rofi polybar i3 alacritty ~/.config/
cp -r ascii ~/.mr-ascii
```
