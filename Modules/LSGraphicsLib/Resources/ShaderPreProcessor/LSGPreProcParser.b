%{


#include "../LSGGraphicsLib.h"
#include "../PreProc/LSGPreProcContainer.h"
#include "../PreProc/LSGPreProcLexer.h"
#include <cstdio>

#pragma warning( push )

// warning C4065: switch statement contains 'default' but no 'case' labels
#pragma warning( disable : 4065 )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

// warning C4512: 'lsg::slice<T,S>' : assignment operator could not be generated
#pragma warning( disable : 4512 )



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define YYINCLUDED_STDLIB_H


// Announce to Flex the prototype we want for lexing function.
extern int yylex( /*YYSTYPE*/void * _pvNodeUnion, lsg::CPreProcLexer * _ppplLexer );


#include "../PreProc/LSGPreProcSyntaxNodes.h"


%}


%pure-parser

%parse-param														{ class CPreProcLexer * m_ppplLexer }
%parse-param														{ class CPreProcContainer * m_pppcContainer }
%lex-param															{ CPreProcLexer * m_ppplLexer }

%token LSG_PPT_DEFINED LSG_PPT_CHAR_CONSTANT LSG_PPT_FLOAT_CONSTANT LSG_PPT_DEC_CONSTANT LSG_PPT_HEX_CONSTANT LSG_PPT_OCT_CONSTANT LSG_PPT_IDENTIFIER
%token LSG_PPT_SHIFT_LEFT LSG_PPT_SHIFT_RIGHT LSG_PPT_AND LSG_PPT_OR
%token LSG_PPT_GE LSG_PPT_LE LSG_PPT_EQUALS LSG_PPT_NOT_EQUALS LSG_PPT_NOT LSG_PPT_1SCOMPLIMENT LSG_PPT_UMINUS LSG_PPT_UPLUS

%type <ndData>														identifier
%type <ndData>														basic_exp
%type <ndData>														unary_exp
%type <ndData>														multiplicative_exp
%type <ndData>														additive_exp
%type <ndData>														shift_exp
%type <ndData>														relational_exp
%type <ndData>														equality_exp
%type <ndData>														logical_and_exp
%type <ndData>														logical_xor_exp
%type <ndData>														logical_or_exp
%type <ndData>														and_exp
%type <ndData>														or_exp
%type <ndData>														conditional_exp
%type <ndData>														exp
%type <ndData>														translation_unit
%type <ui64Op>														unary_op

%start translation_unit

%left	LSG_PPT_OR
%left	LSG_PPT_AND
%left	'|'
%left	'^'
%left	'&'
%left	LSG_PPT_NOT_EQUALS  LSG_PPT_EQUALS
%left	LSG_PPT_GE  LSG_PPT_LE  '>'  '<'
%left	LSG_PPT_SHIFT_RIGHT  LSG_PPT_SHIFT_LEFT
%left   '+'  '-'
%left   '*'  '/'  '%'
%right	LSG_PPT_NOT LSG_PPT_1SCOMPLIMENT LSG_PPT_UMINUS LSG_PPT_UPLUS

%%


identifier
	: LSG_PPT_IDENTIFIER											{ m_pppcContainer->CreateIdentifier( m_ppplLexer->YYText(), $$ ); };
	;

basic_exp
	: LSG_PPT_DEC_CONSTANT											{ m_pppcContainer->CreateDecimal( m_ppplLexer->YYText(), $$ ); }
	| LSG_PPT_HEX_CONSTANT											{ m_pppcContainer->CreateHexadecimal( m_ppplLexer->YYText(), $$ ); }
	| identifier													{ $$ = $1; }
	| LSG_PPT_DEFINED '(' identifier ')'							{ m_pppcContainer->CreateDefined( $3, $$ ); }
	| LSG_PPT_DEFINED identifier									{ m_pppcContainer->CreateDefined( $2, $$ ); }
	| '(' exp ')'													{ $$ = $2; }
	;

unary_op
	: '!' %prec LSG_PPT_NOT											{ $$ = '!'; }
	| '~' %prec LSG_PPT_1SCOMPLIMENT								{ $$ = '~'; }
	| '-' %prec LSG_PPT_UMINUS										{ $$ = '-'; }
	| '+' %prec LSG_PPT_UPLUS										{ $$ = '+'; }
	;

unary_exp
	: basic_exp														{ $$ = $1; }
	| unary_op unary_exp											{ m_pppcContainer->CreateUnaryOp( static_cast<LSUINT32>($1), $2, $$ ); }
	;

multiplicative_exp
	: unary_exp														{ $$ = $1; }
	| multiplicative_exp '*' unary_exp								{ m_pppcContainer->CreateMath( $1, $3, '*', $$ ); }
	| multiplicative_exp '/' unary_exp								{ m_pppcContainer->CreateMath( $1, $3, '/', $$ ); }
	| multiplicative_exp '%' unary_exp								{ m_pppcContainer->CreateMath( $1, $3, '%', $$ ); }
	;

additive_exp
	: multiplicative_exp											{ $$ = $1; }
	| additive_exp '+' multiplicative_exp							{ m_pppcContainer->CreateMath( $1, $3, '+', $$ ); }
	| additive_exp '-' multiplicative_exp							{ m_pppcContainer->CreateMath( $1, $3, '-', $$ ); }
	;

shift_exp
	: additive_exp													{ $$ = $1; }
	| shift_exp LSG_PPT_SHIFT_LEFT additive_exp						{ m_pppcContainer->CreateMath( $1, $3, token::LSG_PPT_SHIFT_LEFT, $$ ); }
	| shift_exp LSG_PPT_SHIFT_RIGHT additive_exp					{ m_pppcContainer->CreateMath( $1, $3, token::LSG_PPT_SHIFT_RIGHT, $$ ); }
	;

relational_exp
	: shift_exp														{ $$ = $1; }
	| relational_exp '>' shift_exp									{ m_pppcContainer->CreateMath( $1, $3, '>', $$ ); }
	| relational_exp '<' shift_exp									{ m_pppcContainer->CreateMath( $1, $3, '<', $$ ); }
	| relational_exp LSG_PPT_GE shift_exp							{ m_pppcContainer->CreateMath( $1, $3, token::LSG_PPT_GE, $$ ); }
	| relational_exp LSG_PPT_LE shift_exp							{ m_pppcContainer->CreateMath( $1, $3, token::LSG_PPT_LE, $$ ); }
	;

equality_exp
	: relational_exp												{ $$ = $1; }
	| equality_exp LSG_PPT_EQUALS relational_exp					{ m_pppcContainer->CreateMath( $1, $3, token::LSG_PPT_EQUALS, $$ ); }
	| equality_exp LSG_PPT_NOT_EQUALS relational_exp				{ m_pppcContainer->CreateMath( $1, $3, token::LSG_PPT_NOT_EQUALS, $$ ); }
	;

logical_and_exp
	: equality_exp													{ $$ = $1; }
	| logical_and_exp '&' equality_exp								{ m_pppcContainer->CreateMath( $1, $3, '&', $$ ); }
	;

logical_xor_exp
	: logical_and_exp												{ $$ = $1; }
	| logical_xor_exp '^' logical_and_exp							{ m_pppcContainer->CreateMath( $1, $3, '^', $$ ); }
	;

logical_or_exp
	: logical_xor_exp												{ $$ = $1; }
	| logical_or_exp '|' logical_xor_exp							{ m_pppcContainer->CreateMath( $1, $3, '|', $$ ); }
	;

and_exp
	: logical_or_exp												{ $$ = $1; }
	| and_exp LSG_PPT_AND logical_or_exp							{ m_pppcContainer->CreateMath( $1, $3, token::LSG_PPT_AND, $$ ); }
	;

or_exp
	: and_exp 														{ $$ = $1; }
	| or_exp LSG_PPT_OR and_exp										{ m_pppcContainer->CreateMath( $1, $3, token::LSG_PPT_OR, $$ ); }
	;

conditional_exp
	: or_exp 														{ $$ = $1; }
	| or_exp '?' exp ':' conditional_exp							{ m_pppcContainer->CreateTertiary( $1, $3, $5, $$ ); }
	;
exp
	: conditional_exp												{ $$ = $1; }
	;
	
translation_unit
	: exp															{ $$ = $1; }
	| translation_unit exp											{ m_pppcContainer->CreateTranslationUnit( $1, $2, $$ ); }
	;


%%


int yylex( /*YYSTYPE*/void * _pvNodeUnion, lsg::CPreProcLexer * _ppplLexer ) {
	static_cast<void *>(_pvNodeUnion);
	return _ppplLexer->NewLex();
}


void yy::parser::error( const yy::location &_lLoc, const std::string &_strM ) {
	static_cast<const yy::location &>(_lLoc);
	static_cast<const std::string &>(_strM);
	/*std::string sError;
	std::string sFile;
	int iLine;
	m_ppplLexer->get_line( sFile, iLine );
	if ( sFile.size() ) {
		// In a file.
		sError = "ERROR: " + sFile + " line: " + CString::CreateFromUI64( iLine ).CStr() + ": " + _strM;
	}
	else {
		sError = "ERROR: line: " + std::string( CString::CreateFromUI64( iLine ).CStr() ) + ": " + _strM;
	}
	CStd::DebugPrintA( sError.c_str() );*/
}

#pragma warning( pop )