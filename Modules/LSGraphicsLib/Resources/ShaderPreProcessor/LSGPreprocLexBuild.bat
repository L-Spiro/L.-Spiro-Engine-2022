flex.exe -+ -L -Cf --nounistd -PLSGPreProcLex "LSGPreProcLex.l"
del LSGPreProcLexBase.cpp
rename lex.LSGPreProcLex.cc LSGPreProcLexBase.cpp
copy LSGPreProcLexBase.cpp "../../Src/PreProcGen"
copy FlexLexer.h "../../Src/PreProcGen/LSGPreProcLexBase.h"

LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.cpp" -r \r "" -r \n \r\n
LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.cpp" -r LSGPreProcLexFlexLexer CPreProcLexBase -r "#line * \"lex.LSGPreProcLex.cc\"" "#include \"../LSGGraphicsLib.h\"\r\n\r\nusing namespace lsg;\r\n// warning C4127: conditional expression is constant\r\n#pragma warning( disable : 4127 )"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.cpp" -r "<FlexLexer.h>" \"LSGPreProcLexBase.h\"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.cpp" -r "LexerOutput( yytext, yyleng )" "LexerOutput( yytext, static_cast<int>(yyleng) )" -r "int yyleng" "LSUINTPTR yyleng" -r "extern LSUINTPTR yyleng;" "" -r "int yyl" "LSUINTPTR yyl"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.cpp" -r "int num_to_read" "LSUINTPTR num_to_read" -r "int new_size" "LSUINTPTR new_size" -r "int num_to_alloc" "LSUINTPTR num_to_alloc"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.cpp" -r "int max_size" "LSUINTPTR max_size"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.cpp" -r "read( buf, max_size )" "read( buf, static_cast<std::streamsize>(max_size) )"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.cpp" -r "int yy_n_chars" "LSUINTPTR yy_n_chars"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.cpp" -r "int number_to_move" "LSUINTPTR number_to_move"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.cpp" -r "int offset" "LSUINTPTR offset" -r " malloc" "CMemLib::MAlloc" -r " realloc" "CMemLib::MReAlloc" -r "free" "CMemLib::Free" -r memset CStd::MemSet -r "return yyin->gcount()" "return static_cast<int>(yyin->gcount())"

LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.h" -r "int yyleng" "LSUINTPTR yyleng" -r "int YYLeng" "LSUINTPTR YYLeng" -r "int yylineno;" "int yylineno;FLEX_STD string curFile;" -r "int yy_top_state();" "int yy_top_state();void set_file( const char* buf );" -r "int yylex();" "int yylex();void get_line( FLEX_STD string &_sFileName, int &_iLine );const FLEX_STD string & get_file() const;"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.h" -r "int yy_n_chars" "LSUINTPTR yy_n_chars"
LSUtility.exe "../../Src/PreProcGen/LSGPreProcLexBase.h" -r "int max_size" "LSUINTPTR max_size" -r __FLEX_LEXER_H __LSG_PREPROCLEXBASE_H__