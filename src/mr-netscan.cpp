#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <unistd.h>
#include <termios.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <fcntl.h>

const std::string RED = "\033[1;31m";
const std::string WHITE = "\033[1;37m";
const std::string GRAY = "\033[90m";
const std::string CYAN = "\033[0;36m";
const std::string GREEN = "\033[1;32m";
const std::string RESET = "\033[0m";
const std::string CLEAR = "\033[2J\033[H";

struct HostInfo {
    std::string ip;
    bool active = false;
    std::vector<int> open_ports;
};

std::vector<HostInfo> hosts;
std::mutex hosts_mutex;

void set_raw_mode(bool enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

// Быстрый TCP-чек порта
bool check_port(const std::string& ip, int port, int timeout_ms = 150) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;

    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);

    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout_ms * 1000;

    bool result = false;
    if (select(sock + 1, NULL, &fdset, NULL, &tv) > 0) {
        int so_error;
        socklen_t len = sizeof(so_error);
        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error == 0) result = true;
    }

    close(sock);
    return result;
}

// Опрос хоста на активность и порты
void ping_host(const std::string& ip) {
    // Проверяем 80, 22, 443, 8080 или 21 как маркеры активности
    std::vector<int> target_ports = {22, 80, 443, 8080, 21, 3389};
    std::vector<int> open_p;
    bool is_alive = false;

    for (int p : target_ports) {
        if (check_port(ip, p, 100)) {
            is_alive = true;
            open_p.push_back(p);
        }
    }

    if (is_alive) {
        std::lock_guard<std::mutex> lock(hosts_mutex);
        hosts.push_back({ip, true, open_p});
    }
}

std::string get_local_subnet() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return "192.168.1";

    sockaddr_in loopback;
    std::memset(&loopback, 0, sizeof(loopback));
    loopback.sin_family = AF_INET;
    loopback.sin_addr.s_addr = inet_addr("1.1.1.1");
    loopback.sin_port = htons(53);

    connect(sock, (struct sockaddr*)&loopback, sizeof(loopback));

    sockaddr_in name;
    socklen_t len = sizeof(name);
    getsockname(sock, (struct sockaddr*)&name, &len);

    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &name.sin_addr, buffer, INET_ADDRSTRLEN);
    close(sock);

    std::string ip_str(buffer);
    size_t last_dot = ip_str.find_last_of('.');
    if (last_dot != std::string::npos) {
        return ip_str.substr(0, last_dot);
    }
    return "192.168.1";
}

int main() {
    std::string subnet = get_local_subnet();

    std::cout << CLEAR << RED << "MR-NETSCAN // FAST TUI SUBNET SCANNER" << RESET << "\n";
    std::cout << GRAY << "────────────────────────────────────────────────────────────" << RESET << "\n";
    std::cout << WHITE << " [*] Сканирование подсети: " << CYAN << subnet << ".1 - " << subnet << ".254" << RESET << "\n";
    std::cout << GRAY << " [*] Запуск 254 потоков..." << RESET << "\n\n";

    std::vector<std::thread> threads;
    for (int i = 1; i <= 254; ++i) {
        std::string target_ip = subnet + "." + std::to_string(i);
        threads.emplace_back(ping_host, target_ip);
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    // Сортировка по возрастанию IP
    std::sort(hosts.begin(), hosts.end(), [](const HostInfo& a, const HostInfo& b) {
        int last_a = std::stoi(a.ip.substr(a.ip.find_last_of('.') + 1));
        int last_b = std::stoi(b.ip.substr(b.ip.find_last_of('.') + 1));
        return last_a < last_b;
    });

    int selected = 0;
    set_raw_mode(true);
    std::cout << "\033[?25l";

    while (true) {
        if (selected >= (int)hosts.size()) selected = (int)hosts.size() - 1;
        if (selected < 0) selected = 0;

        std::cout << CLEAR;
        std::cout << RED << "MR-NETSCAN // ACTIVE HOSTS (" << hosts.size() << " FOUND)" << RESET << "\n";
        std::cout << GRAY << "────────────────────────────────────────────────────────────" << RESET << "\n";
        std::cout << GRAY << " [j/k or Arrows] Navigate  |  [c] Copy IP  |  [q] Quit" << RESET << "\n";
        std::cout << GRAY << "────────────────────────────────────────────────────────────" << RESET << "\n\n";

        if (hosts.empty()) {
            std::cout << GRAY << "  (Активные устройства не найдены)" << RESET << "\n";
        } else {
            for (size_t i = 0; i < hosts.size(); ++i) {
                std::string ports_str = "";
                for (int p : hosts[i].open_ports) {
                    ports_str += std::to_string(p) + " ";
                }

                if ((int)i == selected) {
                    std::cout << RED << " > " << WHITE << hosts[i].ip << RESET;
                    if (!ports_str.empty()) std::cout << "  " << GREEN << "[Ports: " << ports_str << "]" << RESET;
                    std::cout << "\n";
                } else {
                    std::cout << "   " << GRAY << hosts[i].ip << RESET;
                    if (!ports_str.empty()) std::cout << "  " << GRAY << "[Ports: " << ports_str << "]" << RESET;
                    std::cout << "\n";
                }
            }
        }

        fflush(stdout);

        char c;
        if (read(STDIN_FILENO, &c, 1) <= 0) break;

        if (c == 'q' || c == 'Q' || c == 27) break;

        if (c == '\033') {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) > 0 && read(STDIN_FILENO, &seq[1], 1) > 0) {
                if (seq[0] == '[') {
                    if (seq[1] == 'A') selected--; // Up
                    if (seq[1] == 'B') selected++; // Down
                }
            }
        } else if (c == 'k') {
            selected--;
        } else if (c == 'j') {
            selected++;
        } else if (c == 'c' || c == 'C') {
            if (!hosts.empty()) {
                std::string copy_cmd = "echo -n '" + hosts[selected].ip + "' | xclip -selection clipboard 2>/dev/null";
                system(copy_cmd.c_str());
            }
        }
    }

    std::cout << "\033[?25h" << CLEAR;
    set_raw_mode(false);
    return 0;
}
