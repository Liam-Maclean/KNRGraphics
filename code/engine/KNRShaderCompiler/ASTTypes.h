#include <string>
#include <iostream>
#include <vector>

enum BinaryOperation
{
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_GREATER,
	OP_GREATEREQUAL,
	OP_LESS,
	OP_LESSEQUAL,
	OP_EQUAL,
	OP_NOTEQUAL,
	OP_OR,
	OP_AND,
	OP_LEFTSHIFT,
	OP_RIGHTSHIFT,
};

enum FunctionArgumentType
{
	FUNCTION_ARG_IN,
	FUNCTION_ARG_OUT,
	FUNCTION_ARG_INOUT,
};

//Handler for variable types in hlsl
class ASTVariableType
{
public:
	ASTVariableType(std::string strType)
		: m_StrType(strType)
	{

	}

	bool operator ==(const ASTVariableType other)
	{
		m_StrType == other.m_StrType;
	}

private:
	const std::string m_StrType;
};

//Abstract syntax tree generated via bison
class ASTTreeGroup
{
public:

	template<typename T, typename... Args>
	T* AddASTNode(Args... args)
	{
		T node = new T(args...);
		m_ASTNodes.push_back(node);
		return node;
	}
private:
	ASTNode* m_RootNode = nullptr;
	std::vector<ASTNode> m_ASTNodes;
};

//Base node AST class
class ASTNode
{
public:
	ASTNode() {};
	~ASTNode() = default;

	template<typename T> T& As() { return *(T*)this; }

	std::string GenerateHLSLCode(ASTTreeGroup* ast)
	{
		std::string retval = CreateCodeString(ast);

		if (expressionSign < 0) { retval = '-' + retval; }
		if (inParenthesis < 0) { retval = '(' + retval + ')'; }
		if (inBlock < 0) { retval = '{' + retval + '}'; }
		if (hasSemicolon < 0) { retval = retval + ';'; }

		return retval;
	}

	virtual std::string CreateCodeString(ASTTreeGroup* ast) { return std::string(); }

private:
	int expressionSign = 1;
	bool inParenthesis = false;
	bool inBlock = false;
	bool hasSemicolon = false;
};

//A node the variable name (e.g. int x with x being the identifier)
class ASTIdentifier : public ASTNode
{
public:
	ASTIdentifier() = default;

	ASTIdentifier(const std::string& identifier)
		:m_value(identifier)
	{

	}
	~ASTIdentifier()
	{

	}

	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

private:
	const std::string m_value;
};

//Accessing a struct or class (e.g. IN.pos, IN.uv)
class ASTVariableMemberAccess : public ASTNode
{
public:
	ASTVariableMemberAccess() = default;
	ASTVariableMemberAccess(ASTNode* expression, const std::string& accessedMember)
		: m_Expression(expression), m_AccessedMember(accessedMember)
	{

	}

	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

private:
	const ASTNode* m_Expression;
	const std::string m_AccessedMember;
};

//Binary conditions such as ternaries, bit shifting (left ? right or width << 1)
class ASTExpressionBinary : public ASTNode
{
public:
	ASTExpressionBinary() = default;
	ASTExpressionBinary(BinaryOperation op, ASTNode* left, ASTNode* right)
		:m_Operation(op), m_Left(left), m_Right(right)
	{

	}

	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

private:
	const BinaryOperation m_Operation;
	const ASTNode* m_Left;
	const ASTNode* m_Right;
};

class ASTIndexing : public ASTNode
{
public:
	ASTIndexing() = default;
	ASTIndexing(ASTNode* member, ASTNode* index)
		: m_Member(member), m_Index(index)
	{

	}

	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

private:
	const ASTNode* m_Member;
	const ASTNode* m_Index;
};

class ASTAssignment : public ASTNode
{
public:
	ASTAssignment() = default;
	ASTAssignment(ASTNode* lvalue, ASTNode* expression)
		: m_LValue(lvalue), m_Expression(expression)
	{

	}

	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

private:
	ASTNode* m_LValue;
	ASTNode* m_Expression;
};

class ASTValueLiteral : public ASTNode
{
public:
	ASTValueLiteral() = default;
	ASTValueLiteral(float value)
		:m_ValueFloat(value), m_Type("float")
	{

	}

	ASTValueLiteral(int value)
		:m_ValueInt(value), m_Type("int")
	{

	}
	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

private:
	union
	{
		const float m_ValueFloat;
		const int m_ValueInt;
	};

	ASTVariableType m_Type;
};

class ASTVariableDeclaration : public ASTNode
{
	ASTVariableDeclaration() = default;

	ASTVariableDeclaration(ASTVariableType type, std::string typeIdentifier, ASTNode* firstExpression)
		: m_Type(type)
	{
		m_Identifier.push_back(typeIdentifier);
		m_Expression.push_back(firstExpression);
	}

	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

private:
	std::vector<std::string> m_Identifier;
	std::vector<ASTNode*> m_Expression;
	ASTVariableType m_Type;
};

//C style conditional coding
class ASTConditionalIf : public ASTNode
{
	ASTConditionalIf() = default;
	ASTConditionalIf(ASTNode* expression, ASTNode* trueStatement, ASTNode* falseStatement)
		:m_Expression(expression), m_TrueStatement(trueStatement), m_FalseStatement(falseStatement)
	{

	}

	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

private:
	ASTNode* m_Expression;
	ASTNode* m_TrueStatement;
	ASTNode* m_FalseStatement;
};

//C style conditional coding
class ASTConditionalWhile : public ASTNode
{
	ASTConditionalWhile() = default;
	ASTConditionalWhile(ASTNode* expression, ASTNode* bodyStatements)
		:m_Expression(expression), m_BodyStatements(bodyStatements)
	{

	}

	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

private:
	ASTNode* m_Expression;
	ASTNode* m_BodyStatements;
};

//C style conditional coding
class ASTConditionalFor : public ASTNode
{
	ASTConditionalFor() = default;
	ASTConditionalFor(ASTNode* variableDeclaration, ASTNode* booleanExpression, ASTNode* postExpression, ASTNode* statement)
		:m_VariableDeclaration(variableDeclaration), m_BoolExpression(booleanExpression), m_PostExpression(postExpression), m_Statement(statement)
	{

	}

	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

private:
	const ASTNode* m_VariableDeclaration = nullptr;
	const ASTNode* m_BoolExpression = nullptr;
	const ASTNode* m_PostExpression = nullptr;
	const ASTNode* m_Statement = nullptr;
};

class ASTConditionalReturn : public ASTNode
{
	ASTConditionalReturn() = default;
	ASTConditionalReturn(ASTNode* expression)
		:m_Expression(expression)
	{

	}

	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

private:
	ASTNode* m_Expression;
};

class ASTStatementList : public ASTNode
{
	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

private:
	std::vector<ASTNode*> m_Nodes;
};

class ASTFunctionCall : public ASTNode
{
	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;
private:
	std::string m_FunctionName;
	std::vector<ASTNode*> m_Args;
};

class ASTFunctionDeclarationArguments : public ASTNode
{
	ASTFunctionDeclarationArguments() = default;

	ASTFunctionDeclarationArguments(ASTVariableType type, const std::string& identifier, ASTNode* expression, FunctionArgumentType functionArgType)
	: m_Type(type), m_Identifier(identifier), m_Expression(expression), m_FunctionArgumentType(functionArgType)
	{

	}

	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;

	ASTVariableType m_Type;
	const std::string m_Identifier;
	ASTNode* m_Expression;
	FunctionArgumentType m_FunctionArgumentType;
}

class ASTFunctionDeclaration : public ASTNode
{
	std::string GenerateFunctionBody(ASTTreeGroup* ast);
	virtual std::string CreateCodeString(ASTTreeGroup* ast) override;
private:
	std::string m_FunctionName;
	std::vector<ASTNode*> m_Args;
	ASTNode* m_FunctionBody;
	ASTVariableType m_ReturnType;
};

class ASTProgram : public Node
{
public:
	std::string CreateCodeString(ASTTreeGroup* ast) override;

	std::vector<ASTNode*> nodes;
};
