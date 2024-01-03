%{
    #include "ShaderFlexTokens.h"

%}

%option noyywrap
%option c++

%%
"uint"                  return UINT;
"int"                   return INT;
"int2"                  return INT2;
"int3"                  return INT3;
"int4"                  return INT4;
"float"                 return FLOAT;
"float2"                return FLOAT2;
"float3"                return FLOAT3;
"float4"                return FLOAT4;
"CBUFFER"               return CBUFFER;
"Sampler"               return SAMPLER;
"Sampler2D"             return SAMPLER2D;
"Texture2D"             return TEXTURE2D;
"TextureCube"           return TEXTURECUBE;
"TextureCUBE"           return TEXTURECUBE;
"("                     return PARENTHESISLEFT;
")"                     return PARENTHESISRIGHT;
"{"                     return CURLYBRACKETLEFT;
"}"                     return CURLYBRACKETRIGHT;
"["                     return SQUAREBRACKETLEFT;
"]"                     return SQUAREBRACKETRIGHT;
":"                     return COLON;
";"                     return SEMICOLON;
"="                     return ASSIGN;
"=="                    return EQUAL;
"+"                     return ADD;
"-"                     return SUB;
"*"                     return MULT;
"/"                     return DIV;
"%"                     return MOD;
"++"                    return INC;
"--"                    return DEC;
"+="                    return ADD_ASSIGN;
"-="                    return MINUS_ASSIGN;
"*="                    return MULT_ASSIGN;
"/="                    return DIV_ASSIGN;
"&"                     return AND;
"|"                     return OR;
"&&"                    return LOGIC_AND;
"||"                    return LOGIC_OR;
"in"                    return IN;
"out"                   return OUT;
"while"                 return WHILE;
"for"                   return FOR;
"break"                 return BREAK;
"continue"              return CONTINUE;
"if"                    return IF;
"else"                  return ELSE;
"true"                  return TRUE;
"false"                 return FALSE;
"return"                return RETURN;
"struct"                return STRUCT;
"void"                  return VOID;
"const"                 return CONST;
"bool"                  return BOOL;
"discard"               return DISCARD;
[a-zA-Z]                return ALPHABET;
[0-9]                   return INTEGER;
[ \t\r\n\f]+            return WHITESPACE;
.                   {/* Ignore */}
%%


int yywrap(void)
{
    return 1;
}

