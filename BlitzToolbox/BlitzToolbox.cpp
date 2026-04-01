/*
* BlitzToolbox - A part of BlitzToolbox
* Custom library for scpcb-ue.
*
* v1.0 2023.9.12
*/

#include "../BlitzToolbox.hpp"
#include <Windows.h>
#include <filesystem>
#include <wininet.h>
#include <unordered_map>
#include <thread>

BLITZ3D(BBStr) FindNextDirectory(BBStr path, BBStr directory, BBStr def) {
    using namespace std;

    WIN32_FIND_DATAA fdFindData;
    HANDLE hFind;
    std::string filename = path + "\\*.*"s;
    int count = 0;
    BOOL done;
    BOOL nextfolder = !std::filesystem::exists(std::filesystem::path(path + "\\"s + directory));
    hFind = FindFirstFileA(filename.c_str(), &fdFindData);
    done = hFind != INVALID_HANDLE_VALUE;
    while (done)
    {
        if (strcmp(fdFindData.cFileName, ".") && strcmp(fdFindData.cFileName, ".."))
        {
            filename = path + "\\"s + fdFindData.cFileName;
            if ((fdFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
                if (nextfolder) return BlitzToolbox::getCharPtr(BlitzToolbox::replace_all(filename, path + "\\"s, ""));
                if (filename == (path + "\\"s + directory)) nextfolder = TRUE;
            }
        }
        done = FindNextFileA(hFind, &fdFindData);
    }
    FindClose(hFind);
    return def;
}

static int size = 0;

BLITZ3D(int) GetDownloadFileThreadSize() {
    return size;
}

BLITZ3D(void) DownloadFileThread(BBStr url, BBStr file) {
    size = 0;

    std::string cUrl = url;
    std::string cFile = file;

    std::thread([cUrl, cFile]() {
        DeleteFile(std::filesystem::absolute(cFile).generic_string().c_str());

        byte Temp[1024];
        ULONG Number = 1;

        FILE* stream;
        HINTERNET hSession = InternetOpen("RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
        if (hSession != NULL)
        {
            HINTERNET handle2 = InternetOpenUrl(hSession, cUrl.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
            if (handle2 != NULL)
            {
                fopen_s(&stream, cFile.c_str(), "ab+");
                while (Number > 0)
                {
                    InternetReadFile(handle2, Temp, 1024 - 1, &Number);
                    size += Number;
                    fwrite(Temp, sizeof(char), Number, stream);
                }
                fclose(stream);
                InternetCloseHandle(handle2);
                handle2 = NULL;
            }
            InternetCloseHandle(hSession);
            hSession = NULL;
        }
        }).detach();
}

typedef std::unordered_map<std::string, int> S2IMap;
typedef std::unordered_map<int, int> I2IMap;

BLITZ3D(S2IMap*) CreateS2IMap() {
    return new S2IMap;
}

BLITZ3D(int) S2IMapSize(S2IMap* map) {
    return map->size();
}

BLITZ3D(void) S2IMapErase(S2IMap* map, BBStr key) {
    map->erase(key);
}

BLITZ3D(void) S2IMapSet(S2IMap* map, BBStr key, int value) {
    (*map)[key] = value;
}

BLITZ3D(int) S2IMapGet(S2IMap* map, BBStr key) {
    return (*map)[key];
}

BLITZ3D(int) S2IMapContains(S2IMap* map, BBStr key) {
    return map->contains(key);
}

BLITZ3D(void) ClearS2IMap(S2IMap* map) {
    map->clear();
}

BLITZ3D(void) DestroyS2IMap(S2IMap* map) {
    delete map;
    map = nullptr;
}

// I2IMap
BLITZ3D(I2IMap*) CreateI2IMap() {
    return new I2IMap;
}

BLITZ3D(int) I2IMapSize(I2IMap* map) {
    return (int)map->size();
}

BLITZ3D(void) I2IMapErase(I2IMap* map, int key) {
    map->erase(key);
}

BLITZ3D(void) I2IMapSet(I2IMap* map, int key, int value) {
    (*map)[key] = value;
}

BLITZ3D(int) I2IMapGet(I2IMap* map, int key) {
    auto it = map->find(key);
    if (it != map->end()) return it->second;
    return 0;
}

BLITZ3D(int) I2IMapContains(I2IMap* map, int key) {
    return map->count(key);
}

BLITZ3D(void) ClearI2IMap(I2IMap* map) {
    map->clear();
}

BLITZ3D(void) DestroyI2IMap(I2IMap* map) {
    delete map;
}

// IntVector
typedef std::vector<int> IntVector;

BLITZ3D(IntVector*) CreateIntVector() {
    return new IntVector;
}

BLITZ3D(int) IntVectorSize(IntVector* vec) {
    return (int)vec->size();
}

BLITZ3D(void) IntVectorPushBack(IntVector* vec, int value) {
    vec->push_back(value);
}

BLITZ3D(void) IntVectorPopBack(IntVector* vec) {
    if (!vec->empty()) vec->pop_back();
}

BLITZ3D(int) IntVectorGet(IntVector* vec, int index) {
    if (index >= 0 && index < (int)vec->size()) {
        return (*vec)[index];
    }
    return 0;
}

BLITZ3D(void) IntVectorSet(IntVector* vec, int index, int value) {
    if (index >= 0 && index < (int)vec->size()) {
        (*vec)[index] = value;
    }
}

BLITZ3D(int) IntVectorFind(IntVector* vec, int value) {
    auto it = std::find(vec->begin(), vec->end(), value);
    if (it != vec->end()) return (int)std::distance(vec->begin(), it);
    return -1;
}

BLITZ3D(void) IntVectorErase(IntVector* vec, int index) {
    if (index >= 0 && index < (int)vec->size()) {
        vec->erase(vec->begin() + index);
    }
}

BLITZ3D(void) IntVectorReserve(IntVector* vec, int capacity) {
    vec->reserve(capacity);
}

BLITZ3D(void) ClearIntVector(IntVector* vec) {
    vec->clear();
}

BLITZ3D(void) DestroyIntVector(IntVector* vec) {
    delete vec;
}