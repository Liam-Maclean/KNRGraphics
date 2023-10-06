#include "graphics/platform/opengl/opengl_shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <array>

namespace Qualia
{
	Ref<Shader> Shader::Create(const char* name, const char* vertexSrc, const char* fragmentSrc)
	{
		return MakeRef<OpenGlShader>(name, vertexSrc, fragmentSrc);
	}

	const char* Shader::GetShaderVersion()
	{
		return OpenGlShader::GetShaderVersion();
	}

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		return MakeRef<OpenGlShader>(filepath);
	}

	namespace ShaderExt
	{
		constexpr uint32_t vs = ".ps"_hs;
		constexpr uint32_t geom = ".geom"_hs;
		constexpr uint32_t comp = ".comp"_hs;
		constexpr uint32_t ps = ".ps"_hs;
		constexpr uint32_t pipeline[4] = { vs, geom, comp, ps };
	}

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
		{
			return GL_VERTEX_SHADER;
		}
		if (type == "fragment")
		{
			return GL_FRAGMENT_SHADER;
		}		
		if (type == "geometry")
		{
			return GL_GEOMETRY_SHADER;
		}
		return 0;
	}

	OpenGlShader::OpenGlShader(const char* name, const char* vs_src, const char* ps_src)
	{
		ShaderMap sources;
		sources.emplace(std::pair(GL_VERTEX_SHADER, vs_src));
		sources.emplace(std::pair(GL_FRAGMENT_SHADER, ps_src));
		Compile(sources);

		m_name = name;
	}

	OpenGlShader::OpenGlShader(const std::string& filepath)
	{
		std::string shaderSource = ReadFile(filepath);
		auto sources = Parse(shaderSource);
		Compile(sources);

		auto last = filepath.find_last_of("/\\");
		last = last == std::string::npos ? 0 : last + 1;
		auto last_dot = filepath.rfind('.');
		auto count = last_dot == std::string::npos ? filepath.size() - last : last_dot - last;
		m_name = filepath.substr(last, count);
	}

	OpenGlShader::~OpenGlShader()
	{
		glDeleteProgram(m_rendererId);
	}

	void OpenGlShader::Bind() const
	{
		glUseProgram(m_rendererId);
	}

	void OpenGlShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGlShader::SetInt(const std::string name, const int value)
	{
		UploadUniformInt(name, value);
	}

	void OpenGlShader::SetIntArray(const std::string name, int* values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void OpenGlShader::SetFloat(const std::string name, const float value)
	{
		UploadUniformFloat(name, value);
	}

	void OpenGlShader::SetFloat2(const std::string name, const Vector2f value)
	{
		UploadUniformFloat2(name, value);
	}

	void OpenGlShader::SetFloat3(const std::string name, const Vector3f value)
	{
		UploadUniformFloat3(name, value);
	}

	void OpenGlShader::SetFloat4(const std::string name, const Vector4f value)
	{
		UploadUniformFloat4(name, value);
	}

	void OpenGlShader::SetMat4(const std::string& name, const Mat4x4 value)
	{
		UploadUniformMat4(name, value);
	}


	void OpenGlShader::UploadUniformInt(const std::string& name, const int value)
	{
		GLint location = glGetUniformLocation(m_rendererId, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGlShader::UploadUniformIntArray(const std::string name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_rendererId, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGlShader::UploadUniformFloat(const std::string& name, const float value)
	{
		GLint location = glGetUniformLocation(m_rendererId, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGlShader::UploadUniformFloat2(const std::string& name, const Vector2f value)
	{
		GLint location = glGetUniformLocation(m_rendererId, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGlShader::UploadUniformFloat3(const std::string& name, const Vector3f value)
	{
		GLint location = glGetUniformLocation(m_rendererId, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGlShader::UploadUniformFloat4(const std::string& name, const Vector4f value)
	{
		GLint location = glGetUniformLocation(m_rendererId, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGlShader::UploadUniformMat4(const std::string& name, const Mat4x4 value)
	{
		GLint location = glGetUniformLocation(m_rendererId, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	const char* OpenGlShader::GetShaderVersion()
	{
		return (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	}

	void OpenGlShader::AddShaderFile(const std::string& filepath)
	{
	}

	std::string OpenGlShader::ReadFile(const std::string& filepath)
	{
		std::string res;
		std::ifstream file(filepath, std::ios::in | std::ios::binary);

		if (file)
		{
			file.seekg(0, std::ios::end);
			res.resize(file.tellg());
			file.seekg(0, std::ios::beg);
			file.read(&res[0], res.size());
			file.close();
		}
		else
		{
			Q_CORE_ERROR("Could not open file {0}", filepath);
		}
		return res;
	}

	ShaderMap OpenGlShader::Parse(const std::string& source)
	{
		ShaderMap shader_sources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);

			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);

			shader_sources[ShaderTypeFromString(type)]
				= source.substr(nextLinePos,
					pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

		}

		return shader_sources;
	}

	void OpenGlShader::Compile(ShaderMap& sources)
	{
		if (sources.size() == 0)
			return;

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		GLuint program = glCreateProgram();

		size_t max_size = sources.size();

		std::vector<GLenum> shader_ids(max_size);

		size_t idx = 0;

		for (auto& entry : sources)
		{
			GLenum type = entry.first;
			const std::string& source = entry.second;

			GLuint shader = glCreateShader(type);
			const GLchar* gl_source = (const GLchar*)source.c_str();

			glShaderSource(shader, 1, &gl_source, 0);
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				// Use the infoLog as you see fit.
				Q_CORE_ERROR("Shader {0} could not be compiled: {1}", source, infoLog.data());
				assert(false);
				// In this simple program, we'll just leave
				return;
			}

			shader_ids.at(idx++) = shader;
			glAttachShader(program, shader);
		}

		m_rendererId = program;

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.

			for (auto& id : shader_ids)
			{
				glDeleteShader(id);
			}

			// Use the infoLog as you see fit.
			Q_CORE_ERROR("{0}", infoLog.data());
			assert(false);

			// In this simple program, we'll just leave
			return;
		}

		// Always detach shaders after a successful link.
		for (auto& id : shader_ids)
		{
			glDetachShader(program, id);
		}
	}
}
