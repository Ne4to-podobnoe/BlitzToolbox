/*
* BlitzEncode - A part of BlitzToolbox
* Encoding converter.
*
* v1.0 2022.9.23
*/

#include "../BlitzToolbox.hpp"

BLITZ3D(int) GetCodePage() {
	return GetACP();
}

BLITZ3D(BBStr) ConvertEncoding(BBStr str, int sourceCodePage, int destCodePage) {
	CStringA cstr = str; // dont delete this or you will get crash
	BlitzToolbox::Converter(sourceCodePage, destCodePage, cstr);
	return BlitzToolbox::getCharPtr(cstr.GetBuffer());
}