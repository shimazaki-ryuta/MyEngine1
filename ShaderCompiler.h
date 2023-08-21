#pragma once
//Shader関係
#include <cstdint>
#include <string>
#include <format>
#include <vector>
#include"ConvertString.h"


#include <cassert>


//Shader関係
#include <dxcapi.h>


#pragma comment(lib,"dxcompiler.lib")

IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);