bison -ld --skeleton=lalr1.cc -o "LSGPreProcParser.cpp" "LSGPreProcParser.b"

copy LSGPreProcParser.cpp "../../Src/PreProcGen/LSGPreProcParser.cpp"
copy LSGPreProcParser.hpp "../../Src/PreProcGen/LSGPreProcParser.h"
copy location.hh "../../Src/PreProcGen/LSGPreProcParserLocation.h"
copy position.hh "../../Src/PreProcGen/LSGPreProcParserPosition.h"
copy stack.hh "../../Src/PreProcGen/LSGPreProcParserStack.h"

del location.hh
del position.hh
del stack.hh

LSUtility.exe "../../Src/PreProcGen/LSGPreProcParser.cpp" -r \r "" -r \n \r\n
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParser.cpp" -r LSGPreProcParser.hpp LSGPreProcParser.h -r parser CPreProcParser
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParser.cpp" -r m_ppplLexer_yyarg _ppplLexer -r m_pslpLexer_yyarg _pslpLexer -r "\"syntax error\"" "\"Syntax error.\"" -r "\""syntax error," "\""Syntax error,"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParser.h" -r "#include <string>" "" -r "#include <iostream>" "" -r parser CPreProcParser
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParser.h" -r m_ppplLexer_yyarg _ppplLexer -r m_pslpLexer_yyarg _pslpLexer
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParser.h" -r location.hh LSGPreProcParserLocation.h -r position.hh LSGPreProcParserPosition.h -r stack.hh LSGPreProcParserStack.h
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParser.h" -r iff if -r PARSER_HEADER_H __LSG_PREPROCPARSER_H__
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParserLocation.h" -r \r "" -r \n \r\n -r position.hh LSGPreProcParserPosition.h -r BISON_LOCATION_HH __LSG_SHADERPARSERLOCATION_H__
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParserPosition.h" -r \r "" -r \n \r\n -r std::max max -r BISON_POSITION_HH __LSG_SHADERPARSERPOSITION_H__ -r "\r\nnamespace yy" "using namespace std;\r\n\r\nnamespace yy"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParserStack.h" -r \r "" -r \n \r\n -r "unsigned int*    height" "size_t height" -r BISON_STACK_HH __LSG_SHADERPARSERSTACK_H__
set s1=deque^<T^>^>
set s2=deque^<T^> ^>
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParserStack.h" -r %s1% %s2%

set s3=stack^<T^>^>
set s4=stack^<T^> ^>
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParserStack.h" -r %s3% %s4%


:: Below are just to make the code look pretty.  If this causes an error in a future version of Bison, remove it.
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParserLocation.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParserPosition.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcParserStack.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"