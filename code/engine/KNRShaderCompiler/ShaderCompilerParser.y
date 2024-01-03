
%code requires {
  #include <vector>
  #include <iostream>
  #include <fstream>
  #include "ShaderCompiler.h"

  std::ofstream outputShaderFile;

  ShaderCompiler shaderCompiler;

  std::vector<char*> shadersToCompile;
  void CompileShader();
}


/* tokens */
%token UINT			
%token INT			
%token INT2 				
%token INT3				
%token INT4			
%token FLOAT			
%token FLOAT2			
%token FLOAT3			
%token FLOAT4			
%token CBUFFER			
%token SAMPLER 			
%token SAMPLER2D		
%token TEXTURE2D	
%token TEXTURECUBE		
%token TEXTURECUBE	
%token PARENTHESISLEFT	
%token PARENTHESISRIGHT	
%token CURLYBRACKETLEFT	
%token CURLYBRACKETRIGHT
%token SQUAREBRACKETLEFT
%token SQUAREBRACKETRIGHT
%token COLON
%token SEMICOLON
%token ASSIGN		
%token EQUAL	
%token ADD				
%token SUB				
%token MULT				
%token DIV			
%token MOD				
%token INC				
%token DEC				
%token ADD_ASSIGN		
%token MINUS_ASSIGN		
%token MULT_ASSIGN 		
%token DIV_ASSIGN		
%token AND 				
%token OR 				
%token LOGIC_AND 	
%token LOGIC_OR			
%token IN 				
%token OUT 				
%token WHILE			
%token FOR 			
%token BREAK 		
%token CONTINUE 		
%token IF 			
%token ELSE 				
%token TRUE 				
%token FALSE				
%token RETURN 			
%token STRUCT 			
%token VOID 				
%token CONST 	
%token BOOL 				
%token DISCARD 			
%token ALPHABET 			
%token INTEGER 			
%token WHITESPACE 
	
%union {
  char*  text;
  double number;
}

%%

S 
  : statements TOK_EOF
;

statements
  : %empty /*tells bison clearly that it's supposed to be empty */
  | statements statement
;

statement 
  : shader
;

shader
  : PIXELSHADER PARENTHESISLEFT ALPHABET PARENTHESISRIGHT SEMICOLON 
  {
    //Grab the shader, add it to the compile list 
    shadersToCompile.Push_back($ALPHABET);
  }
  | VERTEXSHADER PARENTHESISLEFT ALPHABET PARENTHESISRIGHT SEMICOLON 
  {
    //Grab the shader, add it to the compile list
    shadersToCompile.Push_back($ALPHABET);
  }
;

%%