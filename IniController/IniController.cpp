/*
* IniController - A part of BlitzToolbox
* Reading & writing INI files.
* 
* v1.09 20.04.2026
*/

#include "resource.h"
#include "../BlitzToolbox.hpp"
#include <fstream>
#include <unordered_map>
#include <Windows.h>
#include "simpleini/SimpleIni.h"

#ifdef NORMALIZE_PATH
#define NORMALIZE_PATH(path) BlitzToolbox::normalize_path(std::string(path))
#else
#define NORMALIZE_PATH(path) std::string(path)
#endif

template <typename T, typename V>
using IniMap = std::unordered_map<T, V>;

static IniMap<std::string, IniMap<std::string, IniMap<std::string, std::string>>> IniBuffer;

BLITZ3D(void) IniWriteBuffer(BBStr path, bool clearPrevious) {
	if (clearPrevious) IniBuffer[NORMALIZE_PATH(path)].clear();

	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile(BlitzToolbox::UTF8ToWide(path).c_str()) < 0) return;

	IniMap<std::string, IniMap<std::string, std::string>> buffer;

	CSimpleIniA::TNamesDepend sections;
	ini.GetAllSections(sections);
	for (const auto& sec : sections) {
		CSimpleIniA::TNamesDepend keys;
		ini.GetAllKeys(sec.pItem, keys);
		for (const auto& key : keys) {
			buffer[sec.pItem][key.pItem] = ini.GetValue(sec.pItem, key.pItem, "");
		}
	}
	IniBuffer[NORMALIZE_PATH(path)] = buffer;
}

BLITZ3D(void) IniAddBuffer(BBStr target, BBStr path) {
	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile(BlitzToolbox::UTF8ToWide(path).c_str()) < 0) return;

	CSimpleIniA::TNamesDepend sections;
	ini.GetAllSections(sections);
	for (const auto& sec : sections) {
		CSimpleIniA::TNamesDepend keys;
		ini.GetAllKeys(sec.pItem, keys);
		for (const auto& key : keys) {
			IniBuffer[NORMALIZE_PATH(target)][sec.pItem][key.pItem] = ini.GetValue(sec.pItem, key.pItem, "");
		}
	}
}

/* Read INI */

BLITZ3D(BBStr) IniGetString(BBStr path, BBStr section, BBStr key, BBStr defaultValue, bool allowBuffer) {
	if (allowBuffer) {
		if (IniBuffer[NORMALIZE_PATH(path)][section].contains(key)) {
			return IniBuffer[NORMALIZE_PATH(path)][section][key].c_str();
		}
	}

	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile(BlitzToolbox::UTF8ToWide(path).c_str()) < 0) return defaultValue;

	const char* val = ini.GetValue(section, key, nullptr);
	if (!val) return defaultValue;

	return BlitzToolbox::getCharPtr(val);
}

BLITZ3D(int) IniGetInt(BBStr path, BBStr section, BBStr key, int defaultValue, bool allowBuffer) {
	return atoi(IniGetString(path, section, key, std::to_string(defaultValue).c_str(), allowBuffer));
}

BLITZ3D(float) IniGetFloat(BBStr path, BBStr section, BBStr key, float defaultValue, bool allowBuffer) {
	return atof(IniGetString(path, section, key, std::to_string(defaultValue).c_str(), allowBuffer));
}

BLITZ3D(bool) IniSectionExist(BBStr path, BBStr section, bool allowBuffer) {
	if (allowBuffer) {
		if (IniBuffer[NORMALIZE_PATH(path)].contains(section)) return true;
	}

	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile(BlitzToolbox::UTF8ToWide(path).c_str()) < 0) return false;

	return ini.GetSectionSize(section) >= 0;
}

BLITZ3D(bool) IniBufferSectionExist(BBStr path, BBStr section) {
	return IniBuffer[NORMALIZE_PATH(path)].contains(section);
}

BLITZ3D(bool) IniKeyExist(BBStr path, BBStr section, BBStr key, bool allowBuffer) {
	if (allowBuffer) {
		if (IniBuffer[NORMALIZE_PATH(path)][section].contains(key)) return true;
	}

	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile(BlitzToolbox::UTF8ToWide(path).c_str()) < 0) return false;

	return ini.GetValue(section, key) != nullptr;
}

BLITZ3D(bool) IniBufferKeyExist(BBStr path, BBStr section, BBStr key) {
	return IniBuffer[NORMALIZE_PATH(path)][section].contains(key);
}

/*
* Get value from buffer ONLY.
*
* @param path           Path of ini file.
* @param section        Section of key.
* @param key            Key of value.
* @param defaultValue   Value that if can't find value from buffer.
*
* @return Value of key, or default value.
*/
BLITZ3D(BBStr) IniGetBufferString(BBStr path, BBStr section, BBStr key, BBStr defaultValue) {
	auto& map = IniBuffer[NORMALIZE_PATH(path)][section];
	auto it = map.find(key);

	return it != map.end() ? it->second.c_str() : defaultValue;
}

BLITZ3D(int) IniGetBufferInt(BBStr path, BBStr section, BBStr key, int defaultValue) {
	return atoi(IniGetBufferString(path, section, key, std::to_string(defaultValue).c_str()));
}

BLITZ3D(float) IniGetBufferFloat(BBStr path, BBStr section, BBStr key, float defaultValue) {
	return atof(IniGetBufferString(path, section, key, std::to_string(defaultValue).c_str()));
}

/* Write INI */

BLITZ3D(void) IniWriteString(BBStr path, BBStr section, BBStr key, BBStr value, bool updateBuffer) {
	std::wstring wPath = BlitzToolbox::UTF8ToWide(NORMALIZE_PATH(path));

	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(wPath.c_str());

	ini.SetValue(section, key, value);
	ini.SaveFile(wPath.c_str());

	if (updateBuffer) IniBuffer[NORMALIZE_PATH(path)][section][key] = value;
}

BLITZ3D(void) IniWriteInt(BBStr path, BBStr section, BBStr key, int value, bool updateBuffer) {
	IniWriteString(path, section, key, std::to_string(value).c_str(), updateBuffer);
}

BLITZ3D(void) IniWriteFloat(BBStr path, BBStr section, BBStr key, float value, bool updateBuffer) {
	IniWriteString(path, section, key, std::to_string(value).c_str(), updateBuffer);
}

BLITZ3D(void) IniRemoveKey(BBStr path, BBStr section, BBStr key, bool updateBuffer) {
	std::wstring wPath = BlitzToolbox::UTF8ToWide(NORMALIZE_PATH(path));

	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile(wPath.c_str()) == SI_OK) {
		ini.Delete(section, key);
		ini.SaveFile(wPath.c_str());
	}

	if (updateBuffer) IniBuffer[NORMALIZE_PATH(path)][section].erase(key);
}

BLITZ3D(void) IniCreateSection(BBStr path, BBStr section) {
	std::wstring wPath = BlitzToolbox::UTF8ToWide(NORMALIZE_PATH(path));

	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(wPath.c_str());

	ini.SetValue(section, nullptr, nullptr);
	ini.SaveFile(wPath.c_str());
}

BLITZ3D(void) IniRemoveSection(BBStr path, BBStr section, bool updateBuffer) {
	std::wstring wPath = BlitzToolbox::UTF8ToWide(NORMALIZE_PATH(path));

	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile(wPath.c_str()) == SI_OK) {
		ini.Delete(section, nullptr);
		ini.SaveFile(wPath.c_str());
	}

	if (updateBuffer) IniBuffer[NORMALIZE_PATH(path)].erase(section);
}

BLITZ3D(void) IniRemoveBufferKey(BBStr path, BBStr section, BBStr key) {
	IniBuffer[NORMALIZE_PATH(path)][section].erase(key);
}

BLITZ3D(void) IniRemoveBufferSection(BBStr path, BBStr section) {
	IniBuffer[NORMALIZE_PATH(path)].erase(section);
}

/* BUFFER */

BLITZ3D(void) IniClearBuffer(BBStr path) {
	IniBuffer[NORMALIZE_PATH(path)].clear();
}

BLITZ3D(void) IniClearAllBuffer() {
	IniBuffer.clear();
}

/* DANGER ZONE */

BLITZ3D(void) IniSetBufferValue(BBStr path, BBStr section, BBStr key, BBStr value) {
	IniBuffer[NORMALIZE_PATH(path)][section][key] = value;
}

BLITZ3D(void) IniSetExportBufferValue(IniMap<std::string, IniMap<std::string, std::string>>* buffer, BBStr section, BBStr key, BBStr value) {
	(*buffer)[section][key] = value;
}

extern "C" __declspec(dllexport) IniMap<std::string, IniMap<std::string, std::string>>*_stdcall IniGetBuffer(BBStr path) {
	return &IniBuffer[NORMALIZE_PATH(path)];
}

extern "C" __declspec(dllexport) IniMap<std::string, IniMap<std::string, IniMap<std::string, std::string>>>*_stdcall IniGetAllBuffer() {
	return &IniBuffer;
}

BLITZ3D(void) IniSetBuffer(BBStr path, IniMap<std::string, IniMap<std::string, std::string>>* buffer) {
	IniBuffer[NORMALIZE_PATH(path)] = *buffer;
}

BLITZ3D(void) IniSetAllBuffer(IniMap<std::string, IniMap<std::string, IniMap<std::string, std::string>>>* buffer) {
	IniBuffer = *buffer;
}

/* EXPORT */

IniMap<std::string, IniMap<std::string, std::string>>* GetIniMap(BBStr path, bool allowBuffer) {
	IniMap<std::string, IniMap<std::string, std::string>>* buffer;
	if (allowBuffer && IniBuffer.contains(path)) {
		buffer = &IniBuffer[NORMALIZE_PATH(path)];
	}
	else {
		buffer = new IniMap<std::string, IniMap<std::string, std::string>>();

		CSimpleIniA ini;
		ini.SetUnicode();
		if (ini.LoadFile(BlitzToolbox::UTF8ToWide(path).c_str()) == SI_OK) {
			CSimpleIniA::TNamesDepend sections;
			ini.GetAllSections(sections);
			for (const auto& sec : sections) {
				CSimpleIniA::TNamesDepend keys;
				ini.GetAllKeys(sec.pItem, keys);
				for (const auto& k : keys) {
					(*buffer)[sec.pItem][k.pItem] = ini.GetValue(sec.pItem, k.pItem, "");
				}
			}
		}
	}
	return buffer;
}

/* INI */
void ExportIni(IniMap<std::string, IniMap<std::string, std::string>>&& sectionBuffer, BBStr path, bool isMin) {
	std::ofstream ini(path);
	const char* space = isMin ? "" : " ";
	for (auto section = sectionBuffer.begin(); section != sectionBuffer.end(); section++) {
		IniMap<std::string, std::string>& keyBuffer = sectionBuffer[section->first];
		ini << "[" << section->first << "]" << '\n';
		for (auto key = keyBuffer.begin(); key != keyBuffer.end(); key++) {
			ini << key->first << space << "=" << space << keyBuffer[key->first] << '\n';
		}
		if (!isMin) ini << '\n';
	}
	ini.close();
}

BLITZ3D(void) IniExportIni(BBStr path, BBStr ini, bool isMin, bool allowBuffer) {
	ExportIni(std::move(*GetIniMap(path, allowBuffer)), ini, isMin);
}

BLITZ3D(void) IniBufferExportIni(BBStr path, BBStr ini, bool isMin) {
	ExportIni(std::move(IniBuffer[NORMALIZE_PATH(path)]), ini, isMin);
}

/* JSON */
// i dont know what is "rvalue reference" but i will have a try
void ExportJson(IniMap<std::string, IniMap<std::string, std::string>>&& sectionBuffer, BBStr path, bool isMin, bool stringOnly) {
	std::ofstream json(path);
	const char* endl = isMin ? "" : "\n";
	const char* indent = isMin ? "" : "    ";
	const char* space = isMin ? "" : " ";
	json << "{" << endl;
	for (auto section = sectionBuffer.begin(); section != sectionBuffer.end(); section++) {
		json << indent << "\"" << BlitzToolbox::json_friendly_string(section->first) << "\":" << space << "{" << endl;
		IniMap<std::string, std::string>& keyBuffer = sectionBuffer[section->first];
		for (auto key = keyBuffer.begin(); key != keyBuffer.end(); key++) {
			std::string value = BlitzToolbox::json_friendly_string(keyBuffer[key->first]);
			if (stringOnly) {
				value = "\"" + value + "\"";
			}
			else if (value == "true" || value == "True") {
				value = "true";
			}
			else if (value == "false" || value == "False") {
				value = "false";
			}
			else if (value == std::to_string(atoi(value.c_str()))) {
			}
			else if (value == std::to_string(atof(value.c_str())).substr(0, value.length())) {
			}
			else {
				value = "\"" + value + "\"";
			}
			json << indent << indent << "\"" << BlitzToolbox::json_friendly_string(key->first) << "\":" << space << value << (key == --keyBuffer.end() ? "" : ",") << endl;
		}
		json << indent << "}" << (section == --sectionBuffer.end() ? "" : ",") << endl;
	}
	json << "}";
	json.close();
}

BLITZ3D(void) IniExportJson(BBStr path, BBStr json, bool isMin, bool stringOnly, bool allowBuffer) {
	ExportJson(std::move(*GetIniMap(path, allowBuffer)), json, isMin, stringOnly);
}

BLITZ3D(void) IniBufferExportJson(BBStr path, BBStr json, bool isMin, bool stringOnly) {
	ExportJson(std::move(IniBuffer[NORMALIZE_PATH(path)]), json, isMin, stringOnly);
}

/* HTML */
void ExportHtml(IniMap<std::string, IniMap<std::string, std::string>>&& sectionBuffer, BBStr file, BBStr path, bool isMin, bool isList) {
	std::ofstream html(path);
	const char* endl = isMin ? "" : "\n";
	const char* indent = isMin ? "" : "    ";
	const std::string filename = std::filesystem::path(file).filename().generic_string();
	// writing head
	html << "<head>" << endl;
	html << indent << "<title>Index of " << filename << "</title>" << endl;
	html << "</head>" << endl;
	html << endl;
	html << "<html>" << endl;
	html << indent << "<h1>Index of " << filename << "</h1>" << endl;
	html << endl;
	if (isList) { // as list
		html << indent << "<ul>";
		for (auto section = sectionBuffer.begin(); section != sectionBuffer.end(); section++) {
			html << endl;
			html << indent << indent << "<li>" << endl;
			html << indent << indent << indent << "<b>" << BlitzToolbox::html_friendly_string(section->first) << "</b>" << endl;
			html << indent << indent << indent << "<ul>" << endl;
			IniMap<std::string, std::string>& keyBuffer = sectionBuffer[section->first];
			for (auto key = keyBuffer.begin(); key != keyBuffer.end(); key++) {
				html << indent << indent << indent << indent << "<li><i>" << BlitzToolbox::html_friendly_string(key->first) << "</i> - " << BlitzToolbox::html_friendly_string(keyBuffer[key->first]) << "</li>" << endl;
			}
			html << indent << indent << indent << "</ul>" << endl;
			html << indent << indent << "</li>";
		}
		html << endl;
		html << indent << "</ul>" << endl;
	}
	else { // as table
		html << indent << "<table width=\"100%\" border=\"1\">" << endl;
		html << indent << indent << "<tr>" << endl;
		html << indent << indent << indent << "<th>Section</th>" << endl;
		html << indent << indent << indent << "<th>Key</th>" << endl;
		html << indent << indent << indent << "<th>Value</th>" << endl;
		html << indent << indent << "</tr>" << endl;
		for (auto section = sectionBuffer.begin(); section != sectionBuffer.end(); section++) {
			html << indent << indent << "<tr>" << endl;
			IniMap<std::string, std::string>& keyBuffer = sectionBuffer[section->first];
			html << indent << indent << indent << "<td rowspan=\"" << keyBuffer.size() << "\">" << BlitzToolbox::html_friendly_string(section->first) << "</td>" << endl;
			for (auto key = keyBuffer.begin(); key != keyBuffer.end(); key++) {
				if (key != keyBuffer.begin())
					html << indent << indent << "<tr>" << endl;
				html << indent << indent << indent << "<td>" << BlitzToolbox::html_friendly_string(key->first) << "</td>" << endl;
				html << indent << indent << indent << "<td>" << BlitzToolbox::html_friendly_string(keyBuffer[key->first]) << "</td>" << endl;
				html << indent << indent << "</tr>" << endl;
			}
		}
		html << indent << "</table>" << endl;
	}
	html << indent << "Generate by <i>IniController</i> of <a href=\"https://github.com/ZiYueCommentary/BlitzToolbox\" target=\"_blank\">BlitzToolbox</a>.";
	html << endl;
	html << "</html>";
	html.close();
}

BLITZ3D(void) IniExportHtml(BBStr path, BBStr html, bool isMin, bool isList, bool allowBuffer) {
	ExportHtml(std::move(*GetIniMap(path, allowBuffer)), path, html, isMin, isList);
}

BLITZ3D(void) IniBufferExportHtml(BBStr path, BBStr html, bool isMin, bool isList) {
	ExportHtml(std::move(IniBuffer[NORMALIZE_PATH(path)]), path, html, isMin, isList);
}

/* XML */
void ExportXml(IniMap<std::string, IniMap<std::string, std::string>>&& sectionBuffer, BBStr file, BBStr path, bool isMin) {
	std::ofstream xml(path);
	const char* endl = isMin ? "" : "\n";
	const char* indent = isMin ? "" : "    ";
	std::string filename = std::filesystem::path(file).filename().generic_string();
	const std::string filexten = std::filesystem::path(file).filename().extension().generic_string();
	filename = filename.substr(filename.rfind(filexten));
	xml << "<?xml version=\"1.0\"?>" << endl;
	if (!isMin) {
		xml << "<!--" << endl;
		xml << indent << "Generate by \"IniController\" of BlitzToolbox." << endl;
		xml << indent << "https://github.com/ZiYueCommentary/BlitzToolbox" << endl;
		xml << "-->" << endl << endl;
	}
	xml << "<file name=\"" << BlitzToolbox::json_friendly_string(filename) << "\">" << endl; // json friendly == " -> \"
	for (auto section = sectionBuffer.begin(); section != sectionBuffer.end(); section++) {
		xml << indent << "<section>" << endl;
		xml << indent << indent << "<name>" << BlitzToolbox::xml_friendly_string(section->first) << "</name>" << endl;
		IniMap<std::string, std::string>& keyBuffer = sectionBuffer[section->first];
		for (auto key = keyBuffer.begin(); key != keyBuffer.end(); key++) {
			xml << indent << indent << "<key>" << endl;
			xml << indent << indent << indent << "<name>" << BlitzToolbox::xml_friendly_string(key->first) << "</name>" << endl;
			xml << indent << indent << indent << "<value>" << BlitzToolbox::xml_friendly_string(keyBuffer[key->first]) << "</value>" << endl;
			xml << indent << indent << "</key>" << endl;
		}
		xml << indent << "</section>" << endl;
	}
	xml << "</file>" << endl;
	xml.close();
}

BLITZ3D(void) IniExportXml(BBStr path, BBStr xml, bool isMin, bool allowBuffer) {
	ExportXml(std::move(*GetIniMap(path, allowBuffer)), path, xml, isMin);
}

BLITZ3D(void) IniBufferExportXml(BBStr path, BBStr xml, bool isMin) {
	ExportXml(std::move(IniBuffer[NORMALIZE_PATH(path)]), path, xml, isMin);
}