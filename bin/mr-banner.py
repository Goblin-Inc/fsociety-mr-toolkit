#!/usr/bin/env python3
"""
MR-BANNER // Service & Banner Inspector
Скрипт для определения запущенных служб по IP и порту в локальной сети.
"""

import socket
import sys

RED = "\033[1;31m"
WHITE = "\033[1;37m"
GRAY = "\033[90m"
GREEN = "\033[1;32m"
RESET = "\033[0m"

def inspect_service(ip, port):
    print(f"\n{RED}MR-BANNER // INSPECTING {ip}:{port}{RESET}")
    print(f"{GRAY}────────────────────────────────────────────────────────────{RESET}")
    
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(3.0)
        sock.connect((ip, int(port)))
        
        # Отправляем универсальный HTTP-запрос на случай, если это веб-сервер
        request = f"HEAD / HTTP/1.1\r\nHost: {ip}\r\nUser-Agent: mr-banner\r\n\r\n"
        sock.sendall(request.encode('utf-8'))
        
        banner = sock.recv(1024).decode('utf-8', errors='ignore').strip()
        sock.close()
        
        if banner:
            print(f"{GREEN}[+] Ответ от службы:{RESET}\n")
            for line in banner.split('\r\n')[:10]:  # Первые 10 строк
                print(f"  {WHITE}{line}{RESET}")
        else:
            print(f"{GRAY}[!] Порт открыт, но служба не вернула баннер.{RESET}")
            
    except socket.timeout:
        print(f"{RED}[!] Таймаут подключения к {ip}:{port}{RESET}")
    except Exception as e:
        print(f"{RED}[!] Ошибка подключения: {e}{RESET}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"{WHITE}Использование: python3 mr-banner.py <IP> <PORT>{RESET}")
        print(f"{GRAY}Пример: python3 mr-banner.py 192.168.1.1 80{RESET}")
        sys.exit(1)
        
    target_ip = sys.argv[1]
    target_port = sys.argv[2]
    inspect_service(target_ip, target_port)
