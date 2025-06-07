/*
* RapidBson - A part of BlitzToolbox
* A fast JSON parser/generator for Blitz3D with both SAX/DOM style API.
*
* v1.0 2024.10.3
*/

///////////////////////////
/// IN DEVELOPING STAGE ///
///////////////////////////

#define BLITZ3DTSS

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <fstream>
#include <string>
#include <iostream>
#include <Windows.h>
#include "../BlitzToolbox.hpp"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        std::clog << "RapidBson: In the development stage. May not be stable or useable.\n";
    }
    return TRUE;
}

using namespace rapidjson;
using Array = GenericValue<UTF8<>>::Array;

struct __Writer {
    StringBuffer* buffer;
    Writer<StringBuffer>* writer;

    __Writer() {
        this->buffer = new StringBuffer();
        this->writer = new Writer(*buffer);
    }

    ~__Writer() {
        delete buffer;
        delete writer;
    }
};

enum __SuppressWarning {
    kNoSuppressedWarnings,
    kNoRuntimeExceptions,
    kNoExceptionLogs
};

static int _SuppressedWarnings = kNoSuppressedWarnings;

constexpr void __rapidbson_runtime_exception(BBStr function, const std::string& message) {
    if (!(_SuppressedWarnings & kNoRuntimeExceptions)) {
        BlitzToolbox::runtime_exception(function, message);
    }
    if (!(_SuppressedWarnings & kNoExceptionLogs)) {
        std::cerr << std::format("{}: {}\n", function, message);
    }
}

BLITZ3D(void) JsonSuppressWarnings(int flags) {
    _SuppressedWarnings = flags;
}

BLITZ3D(Value*) JsonParseFromFile(BBStr path) {
    std::ifstream file(path);
    if (!file.good()) {
        __rapidbson_runtime_exception("JsonParseFromFile", std::format("File does not exist: {}", path));
        return 0;
    }

    std::string json, line;
    while (std::getline(file, line)) {
        json += line + '\n';
    }
    Document* doc = new Document();
    doc->Parse<kParseCommentsFlag>(json.c_str());
    if (doc->HasParseError()) {
        __rapidbson_runtime_exception("JsonParseFromFile", std::format("Document parsing failed: {}", path));
    }
    return doc;
}

BLITZ3D(Value*) JsonParseFromString(BBStr json) {
    Document* val = new Document();
    val->Parse<kParseCommentsFlag>(json);
    if (val->HasParseError()) {
        __rapidbson_runtime_exception("JsonParseFromString", std::format("Document parsing failed: {}", json));
    }
    return val;
}

BLITZ3D(int) JsonHasParseError(Document* document) {
    if (document) {
        return document->HasParseError();
    }
    else {
        __rapidbson_runtime_exception("JsonHasParseError", "Invalid argument!");
        return false;
    }
}

BLITZ3D(int) JsonGetParseErrorCode(Document* document) {
    if (document) {
        return document->GetParseError();
    }
    else {
        __rapidbson_runtime_exception("JsonGetParseErrorCode", "Invalid argument!");
        return -1;
    }
}

BLITZ3D(Value*) JsonGetValue(Value* object, BBStr name) {
    if (!object || !object->HasMember(name)) return 0;
    return &(*object)[name];
}

BLITZ3D(int) JsonHasMember(Value* object, BBStr name) {
    if (!object) {
        __rapidbson_runtime_exception("JsonHasMember", "Invalid argument!");
        return false;
    }
    return object->HasMember(name);
}

BLITZ3D(int) JsonIsString(Value* object) {
    return (object && object->IsString());
}

BLITZ3D(int) JsonIsInt(Value* object) {
    return (object && object->IsInt());
}

BLITZ3D(int) JsonIsFloat(Value* object) {
    return (object && object->IsFloat());
}

BLITZ3D(int) JsonIsBool(Value* object) {
    return (object && object->IsBool());
}

BLITZ3D(int) JsonIsArray(Value* object) {
    return (object && object->IsArray());
}

BLITZ3D(int) JsonIsObject(Value* object) {
    return (object && object->IsObject());
}

BLITZ3D(int) JsonIsNull(Value* object) {
    if (!object) return true;
    return object->IsNull();
}

BLITZ3D(BBStr) JsonGetString(Value* object) {
    if (!object || !object->IsString()) {
        __rapidbson_runtime_exception("JsonGetString", "Not a string value!");
        return "";
    }
    else {
        return object->GetString();
    }
}

BLITZ3D(int) JsonGetInt(Value* object) {
    if (!object || !object->IsInt()) {
        __rapidbson_runtime_exception("JsonGetInt", "Not an integer value!");
        return 0;
    }
    else {
        return object->GetInt();
    }
}

BLITZ3D(float) JsonGetFloat(Value* object) {
    if (!object || !object->IsFloat()) {
        __rapidbson_runtime_exception("JsonGetFloat", "Not a float value!");
        return 0;
    }
    else {
        return object->GetFloat();
    }
}

BLITZ3D(int) JsonGetBool(Value* object) {
    if (!object || !object->IsBool()) {
        __rapidbson_runtime_exception("JsonGetBool", "Not a boolean value!");
        return 0;
    }
    else {
        return object->GetBool();
    }
}

BLITZ3D(Value*) JsonGetArray(Value* object) { // The object is already array itself
    if (!object || !object->IsArray()) return 0;
    return object;
}

BLITZ3D(int) JsonGetArraySize(Value* object) {
    if (!object || !object->IsArray()) return 0;
    return object->GetArray().Size();
}

BLITZ3D(Value*) JsonGetArrayValue(Value* object, int index) {
    if (!object || !object->IsArray()) {
        __rapidbson_runtime_exception("JsonGetArrayCapacity", "Invalid argument!");
        return 0;
    }
    if (object->GetArray().Size() <= index) {
        __rapidbson_runtime_exception("JsonGetArrayValue", "Array index out of bounds!");
        return 0;
    }
    else {
        return &(object->GetArray())[index];
    }
}

BLITZ3D(int) JsonGetArrayCapacity(Value* object) {
    if (object && object->IsArray()) {
        return object->GetArray().Capacity();
    }
    else {
        __rapidbson_runtime_exception("JsonGetArrayCapacity", "Invalid argument!");
        return 0;
    }
}

BLITZ3D(void) JsonFreeDocument(Document* object) {
    if (object) delete object;
    else {
        __rapidbson_runtime_exception("JsonFreeDocument", "Invalid argument!");
    }
}

BLITZ3D(__Writer*) JsonCreateWriter() {
    return new __Writer();
}

BLITZ3D(__Writer*) JsonGetNewWriter(Value* object) {
    if (!object) {
        __rapidbson_runtime_exception("JsonGetNewWriter", "Invalid argument!");
        return 0;
    }

    __Writer* writer = new __Writer();
    object->Accept(*writer->writer);
    return writer;
}

BLITZ3D(void) JsonDestroyWriter(__Writer* object) {
    if (object) delete object;
    else {
        __rapidbson_runtime_exception("JsonDestroyWriter", "Invalid argument!");
    }
}

BLITZ3D(BBStr) JsonGetWriterString(__Writer* object) {
    if (object) {
        return object->buffer->GetString();
    }
    else {
        __rapidbson_runtime_exception("JsonGetWriterString", "Invalid argument!");
    }
}

BLITZ3D(int) JsonGetWriterStringLength(__Writer* object) {
    if (object) {
        return object->buffer->GetLength();
    }
    else {
        __rapidbson_runtime_exception("JsonGetWriterStringLength", "Invalid argument!");
    }
}

BLITZ3D(BBStr) JsonGetFormattedWriterString(__Writer* object) {
    if (object) {
        Document document;
        document.Parse(object->buffer->GetString());
        StringBuffer buffer;
        PrettyWriter<StringBuffer> writer(buffer);
        document.Accept(writer);
        return BlitzToolbox::getCharPtr(buffer.GetString());
    }
    else {
        __rapidbson_runtime_exception("JsonGetFormattedJsonFromWriter", "Invalid argument!");
    }
}