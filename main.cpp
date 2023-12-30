#include "server.h"
#include <iostream>


int main() {
    try {
        unsigned short port_number = 12333;

        boost::asio::io_service io_service;

        Server server(io_service, port_number);
        server.run();

        // 여기서부터 MongoDB와의 상호 작용 코드 작성
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // 현재에는 무의미한 코드
    std::cout << "Press Enter to exit...";
    std::cin.get();

    return 0;
}


