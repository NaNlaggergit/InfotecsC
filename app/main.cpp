#include <iostream>
#include <thread>
#include <sstream>
#include "library.h"
#include "threadQueue.h"

struct LogMessage
{
    std::string text;
    LogLevel level;
};

int ParseLevel(const std::string& str, LogLevel& level)
{
    if (str == "INFO")
        level = LogLevel::Info;
    else if (str == "WARNING")
        level = LogLevel::Warning;
    else if (str == "ERROR")
        level = LogLevel::Error;
    else
        return -1;

    return 0;
}

bool ProcessSetLevelCommand(const std::string& input, Library* logger)
{
    std::string levelStr;
    if (input.size() > 8 && input[8] == ' ')
        levelStr = input.substr(9);
    else
    {
        std::cout << "Level: ";
        std::getline(std::cin, levelStr);
    }

    LogLevel newLevel;
    if (ParseLevel(levelStr, newLevel) == 0)
    {
        logger->SetLogLevel(newLevel);
        std::cout << "Default level changed\n";
        return true;
    }
    else
    {
        std::cout << "Invalid level\n";
        return false;
    }
}

void ProcessLogMessage(const std::string& input, LogLevel defaultLevel,
                      ThreadQueue<LogMessage>& queue)
{
    LogLevel level = defaultLevel;
    std::string message;

    if (input.empty())
    {
        std::cout << "Message: ";
        std::getline(std::cin, message);
        queue.Push({message, level});
        return;
    }

    LogLevel parsed;
    if (ParseLevel(input, parsed) == 0)
    {
        level = parsed;
        std::cout << "Message: ";
        std::getline(std::cin, message);
        queue.Push({message, level});
        return;
    }

    std::cout << "Unrecognized input; please enter a valid level or command.\n";
}

std::thread CreateLogWorker(Library* logger, ThreadQueue<LogMessage>& queue)
{
    return std::thread([logger, &queue]()
    {
        LogMessage msg;
        while (queue.Pop(msg))
        {
            try
            {
                logger->Log(msg.text, msg.level);
            }
            catch (const std::exception& e)
            {
                std::cerr << "Logging error: " << e.what() << "\n";
            }
        }
    });
}

void RunMainLoop(Library* logger, LogLevel defaultLevel,
                ThreadQueue<LogMessage>& queue)
{
    std::cout << "Enter messages (type 'exit' to quit)\n";

    while (true)
    {
        std::cout << "Level (INFO/WARNING/ERROR) or command (setlevel/exit), or press Enter for default: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "exit")
            break;

        if (input.rfind("setlevel", 0) == 0)
        {
            ProcessSetLevelCommand(input, logger);
            continue;
        }

        ProcessLogMessage(input, defaultLevel, queue);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./logger_app <log_file> <default_level>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::string defaultLevelStr = argv[2];

    LogLevel defaultLevel;
    if (ParseLevel(defaultLevelStr, defaultLevel) != 0)
    {
        std::cerr << "Invalid default log level\n";
        return 1;
    }

    std::unique_ptr<Library> logger;
    try
    {
        logger = std::make_unique<Library>(filename, defaultLevel);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to initialize logger: " << e.what() << "\n";
        return 1;
    }

    ThreadQueue<LogMessage> queue;
    std::thread worker = CreateLogWorker(logger.get(), queue);

    RunMainLoop(logger.get(), defaultLevel, queue);

    queue.Stop();
    worker.join();

    return 0;
}