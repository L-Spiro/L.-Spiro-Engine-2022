bison -ld --skeleton=lalr1.cc -o "LSGShaderParser.cpp" "LSGShaderParser.b"

copy LSGShaderParser.cpp "../../Src/ShaderLangGen/LSGShaderParser.cpp"
copy LSGShaderParser.hpp "../../Src/ShaderLangGen/LSGShaderParser.h"
copy location.hh "../../Src/ShaderLangGen/LSGShaderParserLocation.h"
copy position.hh "../../Src/ShaderLangGen/LSGShaderParserPosition.h"
copy stack.hh "../../Src/ShaderLangGen/LSGShaderParserStack.h"

del location.hh
del position.hh
del stack.hh

LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParser.cpp" -r \r "" -r \n \r\n
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParser.cpp" -r LSGShaderParser.hpp LSGShaderParser.h -r parser CShaderParser
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParser.cpp" -r m_pslLexer_yyarg _pslLexer -r m_pslpLexer_yyarg _pslpLexer -r "\"syntax error\"" "\"Syntax error.\"" -r "\""syntax error," "\""Syntax error,"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParser.h" -r "#include <string>" "" -r "#include <iostream>" "" -r parser CShaderParser
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParser.h" -r m_pslLexer_yyarg _pslLexer -r m_pslpLexer_yyarg _pslpLexer
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParser.h" -r location.hh LSGShaderParserLocation.h -r position.hh LSGShaderParserPosition.h -r stack.hh LSGShaderParserStack.h
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParser.h" -r iff if -r PARSER_HEADER_H __LSG_SHADER_PARSER_H__
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParserLocation.h" -r \r "" -r \n \r\n -r position.hh LSGShaderParserPosition.h -r BISON_LOCATION_HH __LSG_SHADERPARSERLOCATION_H__
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParserPosition.h" -r \r "" -r \n \r\n -r std::max max -r BISON_POSITION_HH __LSG_SHADERPARSERPOSITION_H__ -r "\r\nnamespace yy" "using namespace std;\r\n\r\nnamespace yy"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParserStack.h" -r \r "" -r \n \r\n -r "unsigned int*    height" "size_t height" -r BISON_STACK_HH __LSG_SHADERPARSERSTACK_H__

:: set s1=deque^<T^>^>
:: set s2=deque^<T^> ^>
:: LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParserStack.h" -r %s1% %s2%
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParserStack.h" -r "deque<T>>" "deque<T> >"


:: set s3=stack^<T^>^>
:: set s4=stack^<T^> ^>
:: LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParserStack.h" -r %s3% %s4%
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParserStack.h" -r "stack<T>>" "stack<T> >"

:: Below are just to make the code look pretty.  If this causes an error in a future version of Bison, remove it.
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParserLocation.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParserPosition.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderParserStack.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
