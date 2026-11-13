#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>

const std::string RED = "\033[1;31m";
const std::string WHITE = "\033[1;37m";
const std::string GRAY = "\033[90m";
const std::string CYAN = "\033[0;36m";
const std::string RESET = "\033[0m";

int main(int argc, char* argv[]) {
    bool no_net = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--no-net" || arg == "-n") {
            no_net = true;
        }
    }

    std::cout << "\n" << RED << "MR-SHADOW // EPHEMERAL RAM SANDBOX" << RESET << "\n";
    std::cout << GRAY << "────────────────────────────────────────────────────────────" << RESET << "\n";
    std::cout << WHITE << " [!] Корневая система смонтирована в режиме READ-ONLY." << RESET << "\n";
    std::cout << WHITE << " [!] Вся файловая активность переведена в оперативное ОЗУ (tmpfs)." << RESET << "\n";
    if (no_net) {
        std::cout << RED << " [!] Сетевой стек полностью изолирован (--unshare-net)." << RESET << "\n";
    } else {
        std::cout << CYAN << " [*] Сетевое подключение доступно." << RESET << "\n";
    }
    std::cout << GRAY << " [i] Для выхода и мгновенного уничтожения среды введите 'exit'." << RESET << "\n";
    std::cout << GRAY << "────────────────────────────────────────────────────────────" << RESET << "\n\n";

    // Формируем безопасный запуск изолятора bwrap
    std::string cmd = "bwrap ";
    cmd += "--ro-bind / / ";                    // Вся система только на чтение
    cmd += "--dev /dev ";                       // Системные устройства
    cmd += "--proc /proc ";                     // Процессы
    cmd += "--tmpfs /tmp ";                     // /tmp полностью в ОЗУ
    cmd += "--tmpfs /var/tmp ";                 // /var/tmp в ОЗУ
    cmd += "--tmpfs /run ";                     // /run в ОЗУ
    cmd += "--tmpfs /home ";                    // /home в ОЗУ (защита домашней папки)

    if (no_net) {
        cmd += "--unshare-net ";                // Полная отсечка от сети
    }

    // Кастомный капельный промпт для изолированного шелла
    std::string prompt = "\\[\\033[1;31m\\][mr-shadow@ram]\\[\\033[1;37m\\]-\\w\\$ \\[\\033[0m\\]";
    cmd += "--setenv PS1 \"" + prompt + "\" ";
    cmd += "/bin/bash";

    int status = std::system(cmd.c_str());

    std::cout << "\n" << RED << "[+] Песочница уничтожена. Все изменения в ОЗУ стёрты." << RESET << "\n\n";
    return status;
}
