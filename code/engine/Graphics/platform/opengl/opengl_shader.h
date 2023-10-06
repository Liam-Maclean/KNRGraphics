#pragma once

#include "graphics/shader.h"
#include "glad/glad.h"

typedef std::unordered_map<GLenum, std::string> ShaderMap;

namespace Qualia
{
	class OpenGlShader 
		: public Shader
	{
	public:
		OpenGlShader(const char* name, const char* vs_shader, const char* ps_shader);
		OpenGlShader(const std::string& filepath);
		virtual ~OpenGlShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string name, const int value) override;
		virtual void SetIntArray(const std::string name, int* values, uint32_t count) override;
		virtual void SetFloat( const std::string name, const float value) override;
		virtual void SetFloat2(const std::string name, const Vector2f value) override;
		virtual void SetFloat3(const std::string name, const Vector3f value) override;
		virtual void SetFloat4(const std::string name, const Vector4f value) override;
		virtual void SetMat4(const std::string& name, const Mat4x4 value) override;

		virtual void UploadUniformInt(const std::string& name, const int value);		
		virtual void UploadUniformIntArray(const std::string name, int* values, uint32_t count);
		virtual void UploadUniformFloat(const std::string& name, const float value);
		virtual void UploadUniformFloat2(const std::string& name, const Vector2f value);
		virtual void UploadUniformFloat3(const std::string& name, const Vector3f value);
		virtual void UploadUniformFloat4(const std::string& name, const Vector4f value);
		virtual void UploadUniformMat4(const std::string& name, const Mat4x4 value);

		static const char* GetShaderVersion();
	private:
		void AddShaderFile(const std::string& filepath);
		std::string ReadFile(const std::string& filepath);
		ShaderMap Parse(const std::string& shaderSource);
		void Compile(ShaderMap & sources);
	private:
		uint32_t m_rendererId = 0;
	};
}