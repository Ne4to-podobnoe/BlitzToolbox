/*
* Header file of BlitzToolbox.
*/

#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <Windows.h>

#ifdef BLITZ3DTSS
    #define BLITZ3D_RUNTIME_ERROR 0xE0000001
    #define BLITZ3D_RUNTIME_EXCEPTION 0xE0000002
    #define _NORETURN [[noreturn]]
#endif

#define BLITZ3D(x) extern "C" __declspec(dllexport) x _stdcall
typedef const char* BBStr;

namespace BlitzToolbox {
    _NODISCARD inline BBStr getCharPtr(const std::string& str) {
        static std::string temp;
        temp.clear();
        temp = str;
        return temp.c_str();
    }

    _NODISCARD _CONSTEXPR20 std::string replace_all(const std::string& string, const std::string& pattern, const std::string& newpat) {
        std::string str = string;
        const unsigned nsize = newpat.size();
        const unsigned psize = pattern.size();

        for (unsigned pos = str.find(pattern, 0); pos != std::string::npos; pos = str.find(pattern, pos + nsize))
        {
            str.replace(pos, psize, newpat);
        }
        return str;
    }

    _NODISCARD _CONSTEXPR20 std::string json_friendly_string(const std::string& str) {
        std::string result = str;
        result = replace_all(result, "\\", "\\\\");
        result = replace_all(result, "\"", "\\\"");
        return result;
    }

    _NODISCARD _CONSTEXPR20 std::string html_friendly_string(const std::string& str) {
        std::string result = str;
        result = replace_all(result, "<", "&lt;");
        result = replace_all(result, ">", "&gt;");
        return result;
    }

    _NODISCARD _CONSTEXPR20 std::string xml_friendly_string(const std::string& str) {
        std::string result = str;
        result = replace_all(result, "\"", "&quot;");
        result = replace_all(result, "&", "&amp;");
        result = replace_all(result, "'", "&apos;");
        result = replace_all(result, ">", "&gt;");
        result = replace_all(result, "<", "&lt;");
        return result;
    }

    _NODISCARD _CONSTEXPR20 std::vector<std::string> split_string(const std::string& str, const std::string& split) {
        std::string string = str;
        std::vector<std::string> vector;
        int pos = string.find(split);
        while (pos != -1) {
            vector.push_back(string.substr(0, pos));
            string = string.substr(pos + split.length());
            pos = string.find(split);
        }
        vector.push_back(string);
        return vector;
    }

    _NODISCARD _CONSTEXPR20 std::string to_lower_string(const std::string& str) {
        std::string result = str;
        for (int i = 0; i < result.length(); i++) {
            result[i] = tolower(result[i]);
        }
        return result;
    }

    _NODISCARD _CONSTEXPR20 std::string normalize_path(const std::filesystem::path& path) {
        // Windows is not case sensitive
        return to_lower_string(std::filesystem::absolute(path).lexically_normal().generic_string());
    }

    std::wstring UTF8ToWide(const std::string& str) {
        if (str.empty()) return std::wstring();
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0);
        if (size_needed <= 0) return std::wstring();
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &wstrTo[0], size_needed);
        return wstrTo;
    }

    std::string WideToUTF8(const std::wstring& str) {
        if (str.empty()) return std::string();
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
        if (size_needed <= 0) return std::string();
        std::string result(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &result[0], size_needed, nullptr, nullptr);
        return result;
    }

#ifdef BLITZ3DTSS
    _NORETURN inline void runtime_error(const std::string& message) {
        ULONG_PTR args[1]{};
        args[0] = reinterpret_cast<ULONG_PTR>(message.c_str());
        RaiseException(BLITZ3D_RUNTIME_ERROR, 0, 1, args);
    }

    inline void runtime_exception(const std::string& function, const std::string& message) {
        ULONG_PTR args[2]{};
        args[0] = reinterpret_cast<ULONG_PTR>(function.c_str());
        args[1] = reinterpret_cast<ULONG_PTR>(message.c_str());
        RaiseException(BLITZ3D_RUNTIME_EXCEPTION, 0, 2, args);
    }
#else
    inline void runtime_error(const std::string& message) {}
    inline void runtime_exception(const std::string& function, const std::string& message) { }
#endif
}