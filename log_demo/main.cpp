#include <iostream>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

int main()
{
    try
    {
        spdlog::info("hello spdlog");
        spdlog::warn("this is a warning message");
        spdlog::error("this is an error message");

        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
        spdlog::info("pattern has been updated");

        auto file_logger = spdlog::basic_logger_mt("file_logger", "logs/app.log");

        file_logger->info("this message is written to logs/app.log");
        file_logger->warn("file logger warning");
        file_logger->error("file logger error");

        file_logger->flush();

        std::cout << "log demo finished." << std::endl;
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cerr << "spdlog init failed: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
