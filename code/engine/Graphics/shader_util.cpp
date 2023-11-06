#include "shader_util.h"
#include <wrl/client.h>
#define SHADER_DEBUG_TRACE 1

namespace KNR
{
	std::vector<uint32_t> ShaderUtil::CompileHLSL(const ShaderInfo& info)
	{
		std::vector<uint32_t> bytecode;
		/*
		//cut off the "bin" suffix and append hlsl on the end
		//All compiled shader files have 5 characters (QVert, QFrag, QMesh, QComp etc..)
		std::wstring hlslPath = std::wstring(info.sourcePath.begin(), info.sourcePath.end() - 5);
		std::wstring outputName = std::wstring(info.sourcePath.begin(), info.sourcePath.end());
		
		
		//Args for shader compilation
		std::vector<LPCWSTR> args{};
		
		switch (info.shaderStage)
		{
		case ShaderStage::pixel:
			args.push_back(L"-T");
			args.push_back(L"ps_6_0");
			break;
		case ShaderStage::vertex:
			args.push_back(L"-T");
			args.push_back(L"vs_6_0");
			break;
		case ShaderStage::domain:
			break;
		case ShaderStage::hull:
			break;
		case ShaderStage::compute:
			args.push_back(L"-T");
			args.push_back(L"cs_6_0");
			break;
		}

		hlslPath += L"hlsl";
		// Liam: Come back here later to properly set an entry point name and debug settings
		//args.push_back(L"main"); //entry point name
		//args.push_back(L"-Fd");
		//args.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);   //-WX Warnings are errors
		args.push_back(L"-I");
		args.push_back(L"/Assets/shaders");
		args.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);
		args.push_back(DXC_ARG_DEBUG); //-Zi Debug mode
		args.push_back(L"-Qstrip_debug");
		args.push_back(hlslPath.c_str());

		using namespace Microsoft::WRL;
		ComPtr<IDxcUtils> dxc_utils;
		auto hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxc_utils.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
		}

		ComPtr<IDxcIncludeHandler> include_handler;
		hr = dxc_utils->CreateDefaultIncludeHandler(include_handler.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
		}

		ComPtr<IDxcCompiler3> dxc_compiler;
		hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxc_compiler));
		if (FAILED(hr))
		{
		}

		ComPtr<IDxcBlobEncoding> pSource;
		hr = dxc_utils->LoadFile(hlslPath.c_str(), nullptr, &pSource);
		DxcBuffer sourceBuffer;
		sourceBuffer.Encoding = DXC_CP_ACP;
		sourceBuffer.Ptr = pSource->GetBufferPointer();
		sourceBuffer.Size = pSource->GetBufferSize();

		//Compile the shader in dxc
		ComPtr<IDxcResult> result;
		hr = dxc_compiler->Compile(
			&sourceBuffer,
			args.data(),
			static_cast<uint32_t>(args.size()),
			include_handler.Get(),
			IID_PPV_ARGS(&result));

		// Error Handling
		ComPtr<IDxcBlobUtf8> pErrors;
		hr = result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(pErrors.GetAddressOf()), nullptr);
		if (pErrors && pErrors->GetStringLength() > 0)
		{
#if SHADER_DEBUG_TRACE
			//Output shader errors
#endif
		}

		// Get shader output
		ComPtr<IDxcBlob> pObject;
		ComPtr<IDxcBlobUtf16> pObjPath;
		hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(pObject.GetAddressOf()), nullptr);
		if (FAILED(hr))
		{
			//Output shader errors
		}

		//// Get shader output
		ComPtr<IDxcBlob> pPDB;
		ComPtr<IDxcBlobUtf16> pPDBName;
		hr = result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pPDB), &pPDBName);
		if (pPDB)
		{
			FILE* fp = NULL;
			_wfopen_s(&fp, pPDBName->GetStringPointer(), L"wb");
			fwrite(pPDB->GetBufferPointer(), pPDB->GetBufferSize(), 1, fp);
			fclose(fp);
		}

		//Copy the dxc output to uint32_t vector and return
		bytecode.resize(pObject->GetBufferSize() / sizeof(uint32_t));
		memcpy(bytecode.data(), pObject->GetBufferPointer(), pObject->GetBufferSize());
		*/
		return bytecode;
	}
}