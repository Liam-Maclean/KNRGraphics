%{
    //C stuff (includes, defines)
    //#include shadertokentypes.h

%}

//Options
%option outfile=ShaderCompilerLex.cpp
%option noyywrap
%option yylineno
%option reentrant
%option bison-bridge
%option bison-locations

%%
//Types
"uint"              return UINT;
"int"               return INT;
"int2"              return INT2;
"int3"              return INT3;
"int4"              return INT4;
"float"             return FLOAT;
"float2"            return FLOAT2;
"float3"            return FLOAT3;
"float4"            return FLOAT4;
"CBUFFER"           return CBUFFER;
"Sampler"           return SAMPLER;
"Sampler2D"         return SAMPLER2D;
"Texture2D"         return TEXTURE2D;
"TextureCube"       return TEXTURECUBE;
"TextureCUBE"       return TEXTURECUBE;

//Syntax
"("                 return PARENTHESISLEFT;
")"                 return PARENTHESISRIGHT;
"{"                 return CURLYBRACKETLEFT;
"}"                 return CURLYBRACKETRIGHT;
[                   return SQUAREBRACKETLEFT;
]                   return SQUAREBRACKETRIGHT;
:                   return COLON;
;                   return SEMICOLON;
"="                 return ASSIGN;
"=="                return EQUAL;
"+"                 return ADD;
"-"                 return SUB;
"*"                 return MULT;
"/"                 return DIV;
"%"                 return MOD;
"++"                return INC;
"--"                return DEC;
"+="                return ADD_ASSIGN;
"-="                return MINUS_ASSIGN;
"*="                return MULT_ASSIGN;
"/="                return DIV_ASSIGN;
"&"                 return AND;
"|"                 return OR;
"&&"                return LOGIC_AND;
"||"                return LOGIC_OR;
"in"                return IN;
"out"               return OUT;


//C style syntax
"while"             return WHILE;
"for"               return FOR;
"break"             return BREAK;
"continue"          return CONTINUE;
"if"                return IF;
"else"              return ELSE;
"true"              return TRUE;
"false"             return FALSE;
"return"            return RETURN;
"struct"            return STRUCT;
"void"              return VOID;
"const"             return CONST;
"bool"              return BOOL;
"discard"           return DISCARD;

//Other
[a-zA-Z]            return ALPHABET;
[0-9]               return INTEGER;
[ \t\r\n\f]+        return WHITESPACE;
.                   {/* Ignore */}
%%


//Need this for any successful compile (ideally somewhere else)
int yywrap(void)
{
    return 1;
}

