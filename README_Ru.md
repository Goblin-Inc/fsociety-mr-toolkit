# Fsociety MR-Toolkit & i3 Environment

> 🌐 **Язык:** [English (README.md)](README.md) | **Русский**

Кастомная экосистема системных, сетевых и десктопных утилит (`mr-*`) в стилистике **Mr. Robot / Fsociety**, созданная для рабочего окружения **i3wm + Polybar + Rofi + Alacritty** под Linux.

## 📂 Структура репозитория

- **`bin/`** — 21 скрипт и утилита (Python и Bash) для управления системой, сетью, файлами и рабочим столом:
  - `mr-dashboard` — центральный интерактивный TUI-хаб с мониторингом ресурсов и быстрым запуском инструментов.
  - `mr-help` — динамический движок документации (автоматически парсит docstrings и комментарии всех `mr-*` утилит).
  - `mr-bridge.py` — P2P-мост для локальной сети (синхронизация буфера обмена по UDP и передача файлов по TCP между ПК и ноутбуком без настройки).
  - `mr-banner.py` — инспектор сетевых служб и баннеров открытых портов.
  - `mr-portcheck`, `mr-pinger`, `mr-dirbuster` — быстрые сетевые утилиты в плавающих окнах терминала.
  - `stealth-mode` — скрипт маскировки в локальной сети (рандомизация MAC-адреса и подмена Hostname).
  - `mr-look` — быстрый консольный навигатор по директориям и просмотрщик файлов.
  - `mr-project` — лаунчер проектов через Rofi с автоматической активацией Python `venv` и запуском `fish` в Alacritty.
  - `mr-net`, `mr-mixer`, `mr-clip`, `mr-wallpaper`, `mr-menu`, `mr-shot`, `mr-lock`, `mr-sysinfo` — апплеты управления Wi-Fi, звуком PipeWire, буфером обмена, обоями и питанием.
  - `mr-matrix` — терминальная симуляция цифрового дождя на `curses` с поддержкой красной темы Fsociety (`--red`).
- **`src/`** — исходный код низкоуровневых утилит на **C++17**:
  - `mr-shadow.cpp` — эфемерная RAM-песочница на базе `Bubblewrap` (`bwrap`), переводящая `/home`, `/tmp`, `/run` в `tmpfs` (ОЗУ) и монтирующая корень `/` в режиме Read-Only с опциональной изоляцией сети (`--no-net`).
  - `mr-netscan.cpp` — многопоточный интерактивный TCP/ICMP сканер локальной сети и портов.
- **`i3/`**, **`polybar/`**, **`rofi/`**, **`alacritty/`** — конфигурационные файлы окружения и кастомная тема `mrrobot.rasi`.
- **`ascii/`** — коллекция ASCII-артов (`~/.mr-ascii`).

## 🛠️ Сборка C++ утилит и установка

```bash
# Компиляция C++ инструментов
g++ -O2 -std=c++17 src/mr-shadow.cpp -o ~/.local/bin/mr-shadow
g++ -O2 -std=c++17 -pthread src/mr-netscan.cpp -o ~/.local/bin/mr-netscan

# Установка скриптов
cp bin/* ~/.local/bin/
chmod +x ~/.local/bin/mr-* ~/.local/bin/stealth-mode

# Копирование конфигов
cp -r rofi polybar i3 alacritty ~/.config/
cp -r ascii ~/.mr-ascii
```
