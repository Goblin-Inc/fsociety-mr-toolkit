#!/usr/bin/env python3
"""
MR-BRIDGE // P2P LAN Sync & File Transfer
Zero-config clipboard & file bridge for Desktop <-> Laptop communication.
"""

import socket
import threading
import time
import sys
import os
import subprocess

RED = "\033[1;31m"
WHITE = "\033[1;37m"
GRAY = "\033[90m"
CYAN = "\033[0;36m"
GREEN = "\033[1;32m"
RESET = "\033[0m"

UDP_PORT = 9998
TCP_PORT = 9999
BUFFER_SIZE = 4096

def get_clipboard():
    try:
        return subprocess.check_output(["xclip", "-selection", "clipboard", "-o"], stderr=subprocess.DEVNULL).decode("utf-8")
    except:
        return ""

def set_clipboard(text):
    try:
        p = subprocess.Popen(["xclip", "-selection", "clipboard"], stdin=subprocess.PIPE)
        p.communicate(input=text.encode("utf-8"))
    except:
        pass

def listen_udp():
    """Слушаем входящие команды синхронизации буфера"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("", UDP_PORT))

    while True:
        data, addr = sock.recvfrom(65535)
        msg = data.decode("utf-8", errors="ignore")
        if msg.startswith("CLIP:"):
            content = msg[5:]
            set_clipboard(content)
            print(f"\n{GREEN}[+] [BRIDGE] Буфер обмена обновлен с {addr[0]}!{RESET}")

def listen_tcp():
    """Слушаем входящие файлы"""
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(("", TCP_PORT))
    server.listen(5)

    while True:
        conn, addr = server.accept()
        header = conn.recv(1024).decode("utf-8").strip()
        if header.startswith("FILE:"):
            _, filename, filesize = header.split(":")
            filesize = int(filesize)
            save_path = os.path.expanduser(f"~/{filename}")

            conn.sendall(b"OK")
            received = 0
            with open(save_path, "wb") as f:
                while received < filesize:
                    chunk = conn.recv(BUFFER_SIZE)
                    if not chunk:
                        break
                    f.write(chunk)
                    received += len(chunk)

            print(f"\n{GREEN}[+] [BRIDGE] Файл {filename} получен от {addr[0]} и сохранен в ~/ !{RESET}")
        conn.close()

def send_clip():
    text = get_clipboard()
    if not text:
        print(f"{RED}[!] Буфер обмена пуст.{RESET}")
        return

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    payload = ("CLIP:" + text).encode("utf-8")
    sock.sendto(payload, ("255.255.255.255", UDP_PORT))
    print(f"{CYAN}[*] Буфер обмена отправлен в сеть!{RESET}")

def send_file(filepath, target_ip):
    if not os.path.exists(filepath):
        print(f"{RED}[!] Файл не найден.{RESET}")
        return

    filename = os.path.basename(filepath)
    filesize = os.path.getsize(filepath)

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((target_ip, TCP_PORT))
        
        header = f"FILE:{filename}:{filesize}"
        sock.sendall(header.encode("utf-8"))

        ack = sock.recv(1024)
        if ack == b"OK":
            with open(filepath, "rb") as f:
                while chunk := f.read(BUFFER_SIZE):
                    sock.sendall(chunk)
            print(f"{GREEN}[+] Файл {filename} успешно отправлен на {target_ip}!{RESET}")
        sock.close()
    except Exception as e:
        print(f"{RED}[!] Ошибка передачи: {e}{RESET}")

def main():
    # Запуск фоновых слушателей
    threading.Thread(target=listen_udp, daemon=True).start()
    threading.Thread(target=listen_tcp, daemon=True).start()

    os.system("clear")
    print(f"{RED}MR-BRIDGE // P2P NETWORK SYNC{RESET}")
    print(f"{GRAY}────────────────────────────────────────────────────────────{RESET}")
    print(f"{WHITE} [1] Push Clipboard  {GRAY}(Отправить буфер обмена на второе устройство){RESET}")
    print(f"{WHITE} [2] Send File        {GRAY}(Передать файл по IP){RESET}")
    print(f"{WHITE} [q] Daemon Mode     {GRAY}(Оставить работать в фоновом режиме){RESET}")
    print(f"{GRAY}────────────────────────────────────────────────────────────{RESET}\n")

    while True:
        try:
            cmd = input(f"{RED}bridge > {RESET}").strip()
            if cmd == "1" or cmd == "clip":
                send_clip()
            elif cmd == "2" or cmd == "file":
                path = input(f"{CYAN} Путь к файлу: {RESET}").strip()
                ip = input(f"{CYAN} IP получателя (из mr-netscan): {RESET}").strip()
                send_file(path, ip)
            elif cmd == "q" or cmd == "exit":
                print(f"{GRAY}[*] Переход в фоновый режим демона...{RESET}")
                while True:
                    time.sleep(3600)
        except (KeyboardInterrupt, EOFError):
            print(f"\n{RED}[!] Завершение работы mr-bridge.{RESET}")
            sys.exit(0)

if __name__ == "__main__":
    main()
