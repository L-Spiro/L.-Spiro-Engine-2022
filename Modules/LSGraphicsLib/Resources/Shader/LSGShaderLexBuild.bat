flex.exe -+ -L -Cf --nounistd -PLSGShaderLex "LSGShaderLex.l"

del LSGShaderLexBase.cpp
rename lex.LSGShaderLex.cc LSGShaderLexBase.cpp
copy LSGShaderLexBase.cpp "../../Src/ShaderLangGen"
copy FlexLexer.h "../../Src/ShaderLangGen/LSGShaderLexBase.h"

LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.cpp" -r \r "" -r \n \r\n
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.cpp" -r LSGShaderLexFlexLexer CShaderLexBase -r "#line * \"lex.LSGShaderLex.cc\"" "#include \"../LSGGraphicsLib.h\"\r\n\r\nusing namespace lsg;\r\n// warning C4127: conditional expression is constant\r\n#pragma warning( disable : 4127 )"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.cpp" -r "<FlexLexer.h>" \"LSGShaderLexBase.h\"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.cpp" -r "LexerOutput( yytext, yyleng )" "LexerOutput( yytext, static_cast<int>(yyleng) )" -r "int yyleng" "LSUINTPTR yyleng" -r "extern LSUINTPTR yyleng;" "" -r "int yyl" "LSUINTPTR yyl"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.cpp" -r "int num_to_read" "LSUINTPTR num_to_read" -r "int new_size" "LSUINTPTR new_size" -r "int num_to_alloc" "LSUINTPTR num_to_alloc"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.cpp" -r "int max_size" "LSUINTPTR max_size"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.cpp" -r "read( buf, max_size )" "read( buf, static_cast<std::streamsize>(max_size) )"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.cpp" -r "int yy_n_chars" "LSUINTPTR yy_n_chars"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.cpp" -r "int number_to_move" "LSUINTPTR number_to_move"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.cpp" -r "int offset" "LSUINTPTR offset" -r " malloc" "CMemLib::MAlloc" -r " realloc" "CMemLib::MReAlloc" -r "free" "CMemLib::Free" -r memset CStd::MemSet -r "return yyin->gcount()" "return static_cast<int>(yyin->gcount())"

LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.h" -r "int yyleng" "LSUINTPTR yyleng" -r "int YYLeng" "LSUINTPTR YYLeng" -r "int yylineno;" "int yylineno;FLEX_STD string curFile;" -r "int yy_top_state();" "int yy_top_state();void set_file( const char* buf );" -r "int yylex();" "int yylex();void get_line( FLEX_STD string &_sFileName, int &_iLine );const FLEX_STD string & get_file() const;"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.h" -r "int yy_n_chars" "LSUINTPTR yy_n_chars"
LSUtility.exe "../../Src/ShaderLangGen/LSGShaderLexBase.h" -r "int max_size" "LSUINTPTR max_size" -r __FLEX_LEXER_H __LSG_SHADERLEXBASE_H__