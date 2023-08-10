#include "InventorEx.h"

int main(int argc, char** argv) {
    InventorEx exc(argc, argv);
    std::cout << "可选的接口有: " << std::endl;
    for (const auto& pair : exc.getFunctions()) {
        std::cout << pair.first << std::endl;
    }
    std::string command;
    do {
        std::cout << "请输入要运行的接口名（输入'exit'退出）: ";
        std::getline(std::cin, command);
        if (command == "exit") {
            break;
        }
        exc.run(command);
    } while (true);

    return 0;
}