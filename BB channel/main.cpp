#include "InventorEx.h"

int main(int argc, char** argv) {
    InventorEx exc(argc, argv);
    std::cout << "��ѡ�Ľӿ���: " << std::endl;
    for (const auto& pair : exc.getFunctions()) {
        std::cout << pair.first << std::endl;
    }
    std::string command;
    do {
        std::cout << "������Ҫ���еĽӿ���������'exit'�˳���: ";
        std::getline(std::cin, command);
        if (command == "exit") {
            break;
        }
        exc.run(command);
    } while (true);

    return 0;
}