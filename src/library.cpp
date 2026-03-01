#include "library.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

Library::Library(const std::string &filename, LogLevel defaultLevel)
    : m_filename(filename),
      m_defaultLevel(defaultLevel),
      m_file(filename, std::ios::app)
{
    if (!m_file.is_open())
    {
        throw std::runtime_error("Failed to open log file");
    }
}

Library::~Library()
{
    if (m_file.is_open())
    {
        m_file.close();
    }
}

Library::Library(Library &&other) noexcept
    : m_filename(std::move(other.m_filename)),
      m_defaultLevel(other.m_defaultLevel),
      m_file(std::move(other.m_file))
{
}

Library &Library::operator=(Library &&other) noexcept
{
    if (this != &other)
    {
        m_filename = std::move(other.m_filename);
        m_defaultLevel = other.m_defaultLevel;
        m_file = std::move(other.m_file);
    }
    return *this;
}

void Library::Log(const std::string &message, LogLevel level)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (level < m_defaultLevel)
        throw std::runtime_error("Log level is below default level");

    if (!m_file.is_open())
        throw std::runtime_error("Log file is not open");

    m_file << "[" << GetCurrentTime() << "] "
           << "[" << LevelToString(level) << "] "
           << message << std::endl;
    
    if (m_file.fail())
        throw std::runtime_error("Failed to write to log file");
}

void Library::SetLogLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_defaultLevel = level;
}

LogLevel Library::GetLogLevel() const noexcept
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_defaultLevel;
}

std::string Library::GetCurrentTime() const
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm buf;
    localtime_r(&now_c, &buf);
    std::stringstream ss;
    ss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");

    return ss.str();
}

std::string Library::LevelToString(LogLevel level) const
{
    switch (level)
    {
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARNING";
    case LogLevel::Error:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}