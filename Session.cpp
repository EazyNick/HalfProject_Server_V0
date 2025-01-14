#include "Session.h"
#include "SessionManager.h"
#include <iostream>
#include "MySQLData/DataRead.h"
#include "Logger.h"

template <typename T>   
bool checkType(const T& value) {
    if constexpr (std::is_same<T, std::string>::value) {
        std::cout << "Type is std::string" << std::endl;
        return true;
    }
    else {
        std::cout << "Type is not std::string" << std::endl;
        return false;
    }
}

Session::Session(tcp::socket socket, SessionManager& manager) : socket_(std::move(socket)) {}

void Session::start(std::string client_id) {
    Logger::GetInstance().log(client_id + " " + "Client connected.");
    read();
}

void Session::start() {
    Logger::GetInstance().log("Client connected.");
    read();
}

void Session::read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_),

        [this, self](const boost::system::error_code& ec, std::size_t length) {
            if (!ec) {
                Logger::GetInstance().log("read");
                //data_.data()는 배열의 첫 번째 요소를 가리키는 포인터를 반환, H, e, l, l, o에서 H를 가리킴
                //length는 data_ 배열에 저장된 유효한 문자열 데이터의 바이트 수를 의미, 5 Byte
                //data_에 ["H","l","l","l","o"]로 저장이 됨
                std::string received_data(data_.data(), length);
                Logger::GetInstance().log("Received: " + received_data);
                //std::cout << "Received data length: " << length << " bytes" << std::endl;

                const std::string database_ = "example_db";

                int test = 1234;

                if (test == 12345) {
                    Logger::GetInstance().log("received_data == hello");
                    std::vector<std::string> DBData = readDataFromDB(kDatabaseServer, kDatabaseUsername, kDatabasePassword, database_);
                    
                    //std::cout << "Received data length: " << length << " bytes" << std::endl;
                    
                    write(DBData);
                }
                else if (checkType(received_data)) {

                    write(length);

                    //SessionManager session_manager;

                    //auto session = session_manager.get_session(2);
                    //if (session) {
                    //    // 2번 클라이언트에게 데이터 전송
                    //    session->write(length);
                    //}
                    //    read();
                }
                else
                {
                    write(length);
                }
            }
            else {
                if (ec == boost::asio::error::eof) {
                    Logger::GetInstance().log("Client disconnected.");
                }
                else {
                    Logger::GetInstance().log("Read error: " + ec.message());
                    std::cerr << "Read error: " << ec.message() << std::endl;
                }
                std::fill(data_.begin(), data_.end(), 0); // 버퍼를 0으로 초기화
            }
        });
}

// Write 이중 정의, 매개변수 값 다름.
void Session::write(std::size_t length) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data_.data(), length), // null값을 기준으로 끊어서 보냄
        [this, self](const boost::system::error_code& ec, std::size_t) {
            if (!ec) {
                Logger::GetInstance().log("Write Message");
                read();
            }
            else {
                std::cerr << "Write error: " << ec.message() << std::endl;
                Logger::GetInstance().log(ec.message());
            }
        });
}

void Session::write(const std::vector<std::string>&data) {
    auto self(shared_from_this());
    // 각 문자열을 순회하면서 async_write 호출
    for (const auto& str : data) {
        boost::asio::async_write(socket_, boost::asio::buffer(str),
            [this, self](const boost::system::error_code& ec, std::size_t) {
                if (!ec) {
                    Logger::GetInstance().log("Write Message");
                    read();
                }
                else {
                    std::cerr << "Write error: " << ec.message() << std::endl;
                    Logger::GetInstance().log(ec.message());
                }
            });
    }
}