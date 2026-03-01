#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>
#include <fstream>

enum class LogLevel
{
    Info = 0,
    Warning = 1,
    Error = 2
};

#include <mutex>

class Library
{
public:
    Library(const std::string& filename, LogLevel defaultLevel);
    ~Library();

    Library(const Library&) = delete;
    Library& operator=(const Library&) = delete;

    Library(Library&& other) noexcept;
    Library& operator=(Library&& other) noexcept;

    void Log(const std::string& message, LogLevel level);
    void SetLogLevel(LogLevel level);
    LogLevel GetLogLevel() const noexcept;

private:
    std::string m_filename;
    LogLevel m_defaultLevel;
    std::ofstream m_file;
    mutable std::mutex m_mutex;

    std::string GetCurrentTime() const;
    std::string LevelToString(LogLevel level) const;
};

#endif