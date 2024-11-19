#pragma once

namespace ErrorHandle {
    inline void CommonErrorMessageBox(std::string_view caption, std::string_view contents, const std::source_location& sl = std::source_location::current())
    {
        auto str = std::format("Error Occurred!\n\nFILE: {}\n\nFUNCTION: {}\n\nLINE: {}\n\nError: {}",
            sl.file_name(), sl.function_name(), sl.line(), contents);

        MessageBoxA(nullptr, str.c_str(), caption.data(), MB_ICONERROR);
    }

    inline void CommonErrorMessageBoxExit(std::string_view caption, std::string_view contents, const std::source_location& sl = std::source_location::current())
    {
        auto str = std::format("Error Occurred!\n\nFILE: {}\n\nFUNCTION: {}\n\nLINE: {}\n\nError: {}",
            sl.file_name(), sl.function_name(), sl.line(), contents);

        MessageBoxA(nullptr, str.c_str(), caption.data(), MB_ICONERROR);

        exit(EXIT_FAILURE);
    }

    inline void CommonErrorMessageBoxAbort(std::string_view caption, std::string_view contents, const std::source_location& sl = std::source_location::current())
    {
        auto str = std::format("Error Occurred!\n\nFILE: {}\n\nFUNCTION: {}\n\nLINE: {}\n\nError: {}",
            sl.file_name(), sl.function_name(), sl.line(), contents);

        MessageBoxA(nullptr, str.c_str(), caption.data(), MB_ICONERROR);

        abort();
    }

    inline void WSAErrorMessageBox(std::string_view caption, const std::source_location& sl = std::source_location::current())
    {
        auto errorCode = WSAGetLastError();

        LPVOID msg;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<char*>(&msg),
            0,
            NULL
        );

        auto str = std::format("Error Occurred!\n\nFILE: {}\n\nFUNCTION: {}\n\nLINE: {}\n\nError Code: {}\n\nError: {}",
            sl.file_name(), sl.function_name(), sl.line(), errorCode, reinterpret_cast<char*>(msg));

        MessageBoxA(nullptr, str.c_str(), caption.data(), MB_ICONERROR);

        LocalFree(msg);
    }

    inline void WSAErrorMessageBoxExit(std::string_view caption, const std::source_location& sl = std::source_location::current())
    {
        auto errorCode = WSAGetLastError();

        LPVOID msg;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<char*>(&msg),
            0,
            NULL
        );

        auto str = std::format("Error Occurred!\n\nFILE: {}\n\nFUNCTION: {}\n\nLINE: {}\n\nError Code: {}\n\nError: {}",
            sl.file_name(), sl.function_name(), sl.line(), errorCode, reinterpret_cast<char*>(msg));

        MessageBoxA(nullptr, str.c_str(), caption.data(), MB_ICONERROR);

        LocalFree(msg);

        exit(EXIT_FAILURE);
    }

    inline void WSAErrorMessageBoxAbort(std::string_view caption, const std::source_location& sl = std::source_location::current())
    {
        auto errorCode = WSAGetLastError();

        LPVOID msg;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<char*>(&msg),
            0,
            NULL
        );

        auto str = std::format("Error Occurred!\n\nFILE: {}\n\nFUNCTION: {}\n\nLINE: {}\n\nError Code: {}\n\nError: {}",
            sl.file_name(), sl.function_name(), sl.line(), errorCode, reinterpret_cast<char*>(msg));

        MessageBoxA(nullptr, str.c_str(), caption.data(), MB_ICONERROR);

        LocalFree(msg);

        abort();
    }
}

template <typename Iterator> requires std::_Is_iterator_v<Iterator>
auto DataAddress(Iterator& iter)
{
    return &(*iter);
}