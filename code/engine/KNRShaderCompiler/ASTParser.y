%define api.pure full
%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner}
%parse-param {ASTTreeGroup* ast}

%{

#include <string>
#include <cstring>
#include <math.h>
#include <map>
#include "ASTTypes.h"
#include "lang.yystype.h"

typedef void* yyscan_t;
void yyerror (yyscan_t yyscanner, char const *msg);
void yyerror (yyscan_t yyscanner, ASTTreeGroup* ast, char const *msg);
int yylex(YYSTYPE *yylval_param, yyscan_t yyscanner);
bool parseExpression(const std::string& inp);

%}

// Token declaration.
%token 		AND OR LE GE EQUALS NOTEQUALS
%token 		WHILE FOR IN OUT INOUT

// if, if-else expression
%token 		IF 
%nonassoc				NONASSOC_IF
%nonassoc				ELSE 
	
%token <str_val>		IDENT CODE_STRING RETURN
%token <float_val>		NUM_FLOAT
%token <int_val>		NUM_INT
%token 					ATTRIBUTE VARYING UNIFORM NATIVE_CODE



// Token precedence.

%left AND OR
%left EQUALS NOTEQUALS
%left LE '<' GE '>'
%left '+' '-'
%left '*' '/'
%left '[' ']'
%left '.'

%nonassoc NONASSOC_UNARY


// Grammar expression types (from yystype).
%type <ASTNode>	fncall_args expr_fncall fndecl_vardecl_var fndecl_vardecl
%type <ASTNode>	expr_base expr stmt
%type <ASTNode>	vardecl_var_list vardecl assign_stmt
%type <ASTNode>	stmt_list function_decl 
%type <ASTNode>	program grammar_elem grammar_list
%type <ASTNode>	shader_globals // These actually have no type.

// The root.
%start program

%%

program : grammar_list { ast->program = $1; $$ = $1; }

grammar_elem :
		function_decl 	{ $$ = $1; }
	|	shader_globals	{ $$ = nullptr; } // This is not a node, it just modifies the AST. 
	;

grammar_list : 

		grammar_elem  				
		{ 
			// Create a list of program elements(only functions so far).
			$$ = ast->push<ASTProgram>(); 
			
			// A program element is not necessary a node. 
			// For example vertexAttribs/varyings/uniforms they just do a add themselves
			// to a container containing all the declarations.
			if($1 != nullptr) $$->As<ASTProgram>().nodes.push_back($1); 
		}
	|	
		grammar_list grammar_elem	
		{ 
			if ($2 != nullptr) $1->As<ASTProgram>().nodes.push_back($2); 
			$$ = $1;
		}
	;
	
		
shader_globals : 
		// These aren't real nodes...
		ATTRIBUTE IDENT IDENT ':' IDENT ';'		{ $$ = nullptr; ast->vertexAttribs.push_back({TypeDesc($2), $3, $5}); }
	|	IN IDENT IDENT ';'						{ $$ = nullptr; ast->stageInputVaryings.push_back({TypeDesc($2), $3}); }
	|	OUT IDENT IDENT ';'						{ $$ = nullptr; ast->stageOutputVaryings.push_back({TypeDesc($2), $3}); }
	|	UNIFORM	IDENT IDENT	';'					{ $$ = nullptr; ast->uniforms.push_back({TypeDesc($2), $3}); }
	;
	
	//-------------------------------------------------
	// Function Declaration.
	//-------------------------------------------------
	
	// A single variable form the function declaration.
fndecl_vardecl_var : 
		IDENT IDENT 				{ $$ = ast->push<ASTFunctionDeclarationArguments>(TypeDesc($1), $2, nullptr, FNAT_In   ); }
	|	IDENT IDENT '=' expr		{ $$ = ast->push<ASTFunctionDeclarationArguments>(TypeDesc($1), $2, $4     , FNAT_In   ); }
	|	IN IDENT IDENT 				{ $$ = ast->push<ASTFunctionDeclarationArguments>(TypeDesc($2), $3, nullptr, FNAT_In	); }
	|	IN IDENT IDENT '=' expr		{ $$ = ast->push<ASTFunctionDeclarationArguments>(TypeDesc($2), $3, $5     , FNAT_In	); }
	|	OUT IDENT IDENT 			{ $$ = ast->push<ASTFunctionDeclarationArguments>(TypeDesc($2), $3, nullptr, FNAT_Out  ); }
	|	OUT IDENT IDENT '=' expr	{ $$ = ast->push<ASTFunctionDeclarationArguments>(TypeDesc($2), $3, $5     , FNAT_Out  ); }
	|	INOUT IDENT IDENT 			{ $$ = ast->push<ASTFunctionDeclarationArguments>(TypeDesc($2), $3, nullptr, FNAT_InOut); }
	|	INOUT IDENT IDENT '=' expr	{ $$ = ast->push<ASTFunctionDeclarationArguments>(TypeDesc($2), $3, $5     , FNAT_InOut); }
	;
	
	// A list of variables for the function declaration.
	// This must be used only for function declaration variables.
	// Basically this create the function declaration node.
	// the node is later finished by function_decl rule.
fndecl_vardecl : 
												{ $$ = ast->push<ASTFunctionDeclaration>(); }
	|	fndecl_vardecl_var						{ $$ = ast->push<ASTFunctionDeclaration>(); ((ASTFunctionDeclaration*)$$)->args.push_back($1); }
	|	fndecl_vardecl ',' fndecl_vardecl_var	{ $1->As<ASTFunctionDeclaration>().args.push_back($3); $$ = $1; }

	
	// The function delectation itself.
	// fndecl_vardecl create the function call node, 
	// we need to just add the return type name and statement.
function_decl : 
	IDENT IDENT '(' fndecl_vardecl ')' '{' stmt_list '}'	{ 
		auto& funcDecl = $4->As<ASTFunctionDeclaration>();
		funcDecl.retType = TypeDesc($1);
		funcDecl.name = $2;
		funcDecl.stmt = $7;
		$$ = $4;
	}
	;
	
	//-------------------------------------------------
	// Variable declaration statement.
	//-------------------------------------------------
	
	// A single variable(or a variable list followed by a single variable) and the optional assigment expression
	// type var, var = expr;
vardecl_var_list : 
		IDENT 								{ $$ = ast->push<ASTVariableDeclaration>(TypeDesc(), $1, nullptr); }
	|	IDENT '=' expr 						{ $$ = ast->push<ASTVariableDeclaration>(TypeDesc(), $1, $3); }
	|	vardecl_var_list ',' IDENT 			{ 
			$1->As<ASTVariableDeclaration>().ident.push_back($3);
			$1->As<ASTVariableDeclaration>().expr.push_back(nullptr);
			$$ = $1;
		}
	|	vardecl_var_list ',' IDENT '=' expr	{ 
			$$ = $1; 
			$$->As<ASTVariableDeclaration>().ident.push_back($3);
			$$->As<ASTVariableDeclaration>().expr.push_back($5);
		}
	
	// The actual variable declaration
vardecl :
			IDENT vardecl_var_list	{ $2->As<ASTVariableDeclaration>().type = TypeDesc($1); $$ = $2; }
		|	IDENT '[' NUM_INT ']' vardecl_var_list { $5->As<ASTVariableDeclaration>().type = TypeDesc($1, $3); $$ = $5; }
	;
	
	//-------------------------------------------------
	// Statements.
	//-------------------------------------------------
	
stmt : 
		vardecl ';'									{ $1->hasSemicolon = true; ; $$ = $1; }
	|	expr ';' 									{ $1->hasSemicolon = true; $$ = $1; }
	|	assign_stmt ';' 							{ $1->hasSemicolon = true; $$ = $1; }
	|	FOR '(' vardecl ';' expr ';' expr ')' stmt	{ $$ = ast->push<ASTConditionalIf>($3, $5, $7, $9); }
	|	WHILE '(' expr ')' stmt 					{ $$ = ast->push<ASTConditionalWhile>($3, $5); }
	|	IF '(' expr ')' stmt %prec NONASSOC_IF		{ $$ = ast->push<ASTConditionalIf>($3, $5, nullptr); }
	|	IF '(' expr ')' stmt ELSE stmt				{ $$ = ast->push<ASTConditionalIf>($3, $5, $7); }
	|	'{' stmt_list '}' 							{ $2->inBlock = true; $$ = $2; }
	|	RETURN expr ';'								{ $$ = ast->push<ASTConditionalReturn>($2); }
	|	RETURN ';'									{ $$ = ast->push<ASTConditionalReturn>(); }
	;

		//[TODO] This should become something like expr = expr at least because of array indexing.
assign_stmt : 
		expr '=' expr				{ $$ = ast->push<ASTAssignment>($1, $3); }
	;
	
	// A list of statements.
stmt_list : 
		stmt 				{ $$ = ast->push<ASTList>(); $$->As<ASTList>().nodes.push_back($1); }
	|	stmt_list stmt 		{ 
			$$ = $1;
			$1->As<ASTList>().nodes.push_back( {$2} );
		}
	;
	
	//-------------------------------------------------
	// Expressions (like +-*/ function calls, literals ect.).
	//-------------------------------------------------

expr : expr_base { $$ = $1; ast->addDeduct($1); }
expr_base :
		'(' expr_base ')'			    	{ $2->inParenthesis = true; $$ = $2; }
	|	expr_base '[' expr_base ']'			{ $$ = ast->push<ASTIndexing>($1, $3); } // shift-reduce don't know why...
	|	expr_base '.' IDENT					{ $$ = ast->push<ASTVariableMemberAccess>($1, $3); }
	|	IDENT					        	{ $$ = ast->push<ASTIdentifier>($1); }
	|	expr_base OR expr_base			    { $$ = ast->push<ASTExpressionBinary>(OP_OR, $1, $3); }
	|	expr_base AND expr_base			    { $$ = ast->push<ASTExpressionBinary>(OP_AND, $1, $3); }
	|	expr_base NOTEQUALS expr_base		{ $$ = ast->push<ASTExpressionBinary>(OP_NOTEQUAL, $1, $3); }
	|	expr_base EQUALS expr_base			{ $$ = ast->push<ASTExpressionBinary>(OP_EQUAL, $1, $3); }
	|	expr_base LE expr_base				{ $$ = ast->push<ASTExpressionBinary>(OP_LESSEQUAL, $1, $3); }
	|	expr_base '<' expr_base				{ $$ = ast->push<ASTExpressionBinary>(OP_LESS, $1, $3); }
	|	expr_base GE expr_base				{ $$ = ast->push<ASTExpressionBinary>(OP_GREATEREQUAL, $1, $3); }
	|	expr_base '>' expr_base				{ $$ = ast->push<ASTExpressionBinary>(OP_GREATER, $1, $3); }
	|	expr_base '+' expr_base				{ $$ = ast->push<ASTExpressionBinary>(OP_ADD, $1, $3); } 
	|	expr_base '-' expr_base				{ $$ = ast->push<ASTExpressionBinary>(OP_SUB, $1, $3); } 	
	|	expr_base '*' expr_base				{ $$ = ast->push<ASTExpressionBinary>(OP_MUL, $1, $3); } 
	|	expr_base '/' expr_base				{ $$ = ast->push<ASTExpressionBinary>(OP_DIV, $1, $3); }
	|	expr_base '%' expr_base				{ $$ = ast->push<ASTExpressionBinary>(OP_MOD, $1, $3); }
	|	NUM_FLOAT					        { $$ = ast->push<ASTValueLiteral>($1); }
	|	NUM_INT						        { $$ = ast->push<ASTValueLiteral>($1); }
	|	expr_fncall					        { $$ = $1; }	
	|	'-' expr_base %prec NONASSOC_UNARY	{ $2->expressionSign *= -1; $$ = $2; }
	|	'+' expr_base %prec NONASSOC_UNARY	{ $$ = $2; }
	;

	
	// Function arguments as a list.
fncall_args :
			{ $$ = ast->push<ASTFunctionCall>(); }
		| 	expr_base
			{ 
				Node* fnCall = ast->push<ASTFunctionCall>();
				fnCall->As<ASTFunctionCall>().args.push_back($1); 
				$$ = fnCall;
			}
		
	|	fncall_args ',' expr_base 	{ $1->As<ASTFunctionCall>().args.push_back($3); $$ = $1; }
	
	// The function call expression itself.
expr_fncall :
		IDENT '(' fncall_args ')'	{ 
			$3->As<ASTFunctionCall>().fnName = $1;
			$$ = $3; 
		}
	;
	//--------------------------------------------
	//
	//--------------------------------------------

%%

void yyerror (yyscan_t yyscanner, char const *msg){
    fprintf(stderr, "%s\n", msg);
}

void yyerror (yyscan_t yyscanner, Ast* ast, char const *msg) {
	yyerror(yyscanner, msg);
}