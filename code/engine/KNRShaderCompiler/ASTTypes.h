#include <string>
#include <iostream>
#include <vector>

class ASTNodeGroup
{
public:
	ASTNodeGroup()
	{

	}
	~ASTNodeGroup()
	{

	}

	void AddASTNode(const ASTBase& astNode)
	{
		m_ASTNodes.push_back(astNode);
	}

	void RunASTNodeTree(std::ifstream* stream)
	{
		for (size_t i = 0; i < m_ASTNodes.size(); ++i)
		{
			m_ASTNodes[i].WriteToFile(stream);
		}
	}

	std::vector<ASTBase> m_ASTNodes;
};

class ASTBase
{
public:
	ASTBase() {};
	~ASTBase() {};

	virtual void WriteToFile(std::ifstream* stream)
	{

	}
};

class ASTFloat : public ASTBase
{
public:
	ASTFloat()
		:ASTBase()
	{

	}
	~ASTFloat()
	{

	}

	virtual void WriteToFile(std::ifstream* stream) override
	{

	}

private:
	float m_value;
};

class ASTInt : public ASTBase
{
public:
	ASTInt()
		:ASTBase()
	{

	}
	~ASTInt()
	{

	}

	virtual void WriteToFile(std::ifstream* stream) override
	{

	}

private:
	float m_value;
};

class ASTInt2 : public ASTBase
{
public:
	ASTInt2()
		:ASTBase()
	{

	}
	~ASTInt2()
	{

	}

	virtual void WriteToFile(std::ifstream* stream) override
	{

	}

private:
	float m_value;
};

class ASTInt3 : public ASTBase
{
public:
	ASTInt3()
		:ASTBase()
	{

	}
	~ASTInt3()
	{

	}

	virtual void WriteToFile(std::ifstream* stream) override
	{

	}

private:
	float m_value;
};

class ASTInt4 : public ASTBase
{
public:
	ASTInt4()
		:ASTBase()
	{

	}
	~ASTInt4()
	{

	}

	virtual void WriteToFile(std::ifstream* stream) override
	{

	}

private:
	float m_value;
};

class ASTChar : public ASTBase
{
public:
	ASTChar()
		:ASTBase()
	{

	}
	~ASTChar()
	{

	}

	virtual void WriteToFile(std::ifstream* stream) override
	{

	}

private:
	char m_value;
};

class ASTString : public ASTBase
{
public:
	ASTString()
		:ASTBase()
	{

	}
	~ASTString()
	{

	}

	virtual void WriteToFile(std::ifstream* stream) override
	{

	}

private:
	std::string m_value;
};

class ASTOperation : public ASTBase
{
	enum ASTOperationType
	{
		Addition,
		Subtraction,
		Multiplication,
		Division,
		Modulo,
		BitshiftRight,
		BitshiftLeft,
	};
public:
	ASTOperation()
		:ASTBase()
	{

	}
	~ASTOperation()
	{

	}

	virtual void WriteToFile(std::ifstream* stream) override
	{

	}

private:
	ASTOperationType m_value;
};

class ASTMisc : public ASTBase
{
public:
	ASTMisc()
	{

	}

	~ASTMisc()
	{

	}

	virtual void WriteToFile(std::ifstream* stream) override
	{

	}

};

class ASTSampler : public ASTBase
{
	enum ASTSamplerType
	{
		Sampler,
		Sampler2D,
	};
public:
	ASTSampler()
		:ASTBase()
	{

	}
	~ASTSampler()
	{

	}

	virtual void WriteToFile(std::ifstream* stream) override
	{

	}

private:
	ASTSamplerType m_value;
};

class ASTConstant : public ASTBase
{
public:
	ASTConstant()
		:ASTBase()
	{

	}
	~ASTConstant()
	{

	}

	virtual void WriteToFile(std::ifstream* stream) override
	{

	}

private:
	std::string m_value;
};

class ASTTexture : public ASTBase
{
	enum ASTTextureType
	{
		Texture1D,
		Texture2D,
		Texture2DArray,
		Texture3D,
		TextureCube,
		TextureCubeArray,
	};
public:
	ASTTexture()
		:ASTBase()
	{

	}
	~ASTTexture()
	{

	}

	virtual void WriteToFile(std::ifstream* stream) override
	{

	}

private:
	ASTTextureType m_value;
};