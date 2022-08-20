%{


#include "../LSGGraphicsLib.h"
#include "../ShaderLanguage/LSGShaderLanguageParser.h"
#include "../ShaderLanguage/LSGShaderLexer.h"
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
extern int yylex( /*YYSTYPE*/void * _pvNodeUnion, lsg::CShaderLexer * _pslLexer );


#include "../ShaderLanguage/LSGShaderSyntaxNodes.h"


%}


%pure-parser

%parse-param														{ class CShaderLexer * m_pslLexer }
%parse-param														{ class CShaderLanguageParser * m_pslpContainer }
%lex-param															{ CShaderLexer * m_pslLexer }

%token LSG_SLT_CHAR_CONSTANT LSG_SLT_STRING_CONSTANT LSG_SLT_FLOAT_CONSTANT LSG_SLT_DEC_CONSTANT LSG_SLT_HEX_CONSTANT LSG_SLT_OCT_CONSTANT LSG_SLT_IDENTIFIER LSG_SLT_TYPENAME
%token LSG_SLT_PLUS_EQUALS LSG_SLT_MINUS_EQUALS LSG_SLT_TIMES_EQUALS LSG_SLT_DIV_EQUALS LSG_SLT_MOD_EQUALS
%token LSG_SLT_SHIFT_LEFT LSG_SLT_SHIFT_RIGHT LSG_SLT_AND_EQUALS LSG_SLT_XOR_EQUALS LSG_SLT_OR_EQUALS LSG_SLT_SHIFT_LEFT_EQUALS LSG_SLT_SHIFT_RIGHT_EQUALS LSG_SLT_NOT LSG_SLT_UMINUS LSG_SLT_UPLUS
%token LSG_SLT_AND LSG_SLT_XOR LSG_SLT_OR LSG_SLT_GE LSG_SLT_LE LSG_SLT_EQUALS LSG_SLT_NOT_EQUALS
%token LSG_SLT_PLUS_PLUS LSG_SLT_MINUS_MINUS
%token LSG_SLT_BLENDSTATE LSG_SLT_BOOL LSG_SLT_BREAK
%token LSG_SLT_CONST LSG_SLT_CONTINUE
%token LSG_SLT_DISCARD LSG_SLT_DO
%token LSG_SLT_ELSE
%token LSG_SLT_FALSE LSG_SLT_FLOAT LSG_SLT_FOR
%token LSG_SLT_IF LSG_SLT_IN LSG_SLT_INOUT LSG_SLT_INT
%token LSG_SLT_MATRIX
%token LSG_SLT_NOINTERPOLATION
%token LSG_SLT_OUT
%token LSG_SLT_RETURN
%token LSG_SLT_SAMPLER LSG_SLT_SMOOTH LSG_SLT_STRUCT LSG_SLT_SWITCH
%token LSG_SLT_TEXTURE1D LSG_SLT_TEXTURE2D LSG_SLT_TEXTURE3D LSG_SLT_TEXTURECUBE LSG_SLT_TRUE
%token LSG_SLT_UINT LSG_SLT_UNIFORM
%token LSG_SLT_VEC2 LSG_SLT_VEC3 LSG_SLT_VEC4 LSG_SLT_VECTOR LSG_SLT_VOID
%token LSG_SLT_WHILE

%token LSG_SLT_ABS LSG_SLT_ACOS LSG_SLT_ANY LSG_SLT_ASIN LSG_SLT_ATAN LSG_SLT_ATAN2
%token LSG_SLT_CEIL LSG_SLT_CLAMP LSG_SLT_COS LSG_SLT_CROSS
%token LSG_SLT_DDX LSG_SLT_DDY LSG_SLT_DEGREES LSG_SLT_DETERMINANT LSG_SLT_DISTANCE LSG_SLT_DOT
%token LSG_SLT_EXP LSG_SLT_EXP2
%token LSG_SLT_FLOOR LSG_SLT_FRAC
%token LSG_SLT_ISINF LSG_SLT_ISNAN
%token LSG_SLT_LENGTH LSG_SLT_LERP LSG_SLT_LOG LSG_SLT_LOG2
%token LSG_SLT_MAX LSG_SLT_MIN LSG_SLT_MIX LSG_SLT_MOD LSG_SLT_MUL
%token LSG_SLT_NORMALIZE
%token LSG_SLT_POW
%token LSG_SLT_RADIANS LSG_SLT_RCP LSG_SLT_REFLECT LSG_SLT_REFRACT LSG_SLT_RSQRT
%token LSG_SLT_SATURATE LSG_SLT_SIN LSG_SLT_SINCOS LSG_SLT_SMOOTHSTEP LSG_SLT_SQRT LSG_SLT_STEP
%token LSG_SLT_TAN LSG_SLT_TEX1D LSG_SLT_TEX2D LSG_SLT_TEX2DSHADOW LSG_SLT_TEX2DSHADOWCOMPARE LSG_SLT_TEX3D LSG_SLT_TEXCUBE LSG_SLT_TRANSPOSE

%token LSG_SLT_BINORMAL LSG_SLT_BLENDINDICES LSG_SLT_BLENDWEIGHT LSG_SLT_COLOR LSG_SLT_NORMAL LSG_SLT_POSITION LSG_SLT_POSITIONT LSG_SLT_PSIZE LSG_SLT_TANGENT LSG_SLT_TEXCOORD
%token LSG_SLT_FOG LSG_SLT_TESSFACTOR
%token LSG_SLT_ISFRONTFACE LSG_SLT_VPOS LSG_SLT_DEPTH

%token LSG_SLT_AMBIENTLIGHT LSG_SLT_LIGHTCOUNT LSG_SLT_DIRLIGHTCOUNT LSG_SLT_POINTLIGHTCOUNT LSG_SLT_SPOTLIGHTCOUNT LSG_SLT_DIRLIGHTDIRS LSG_SLT_DIRLIGHTDIFFUSES LSG_SLT_POINTLIGHTPOS LSG_SLT_POINTLIGHTDIFFUSES LSG_SLT_POINTLIGHTSIZES LSG_SLT_SPOTLIGHTDIRS LSG_SLT_LIGHTAMBIENT LSG_SLT_LIGHTDIFFUSE LSG_SLT_LIGHTSPECULAR LSG_SLT_LIGHTATT LSG_SLT_SPOTLIGHTEXP LSG_SLT_SPOTLIGHTRANGE LSG_SLT_SKYCOLOR LSG_SLT_GROUNDCOLOR

%token LSG_SLT_ASHIKFACTORS LSG_SLT_MATAMBIENT LSG_SLT_MATDIFFUSE LSG_SLT_MATEMISSIVE LSG_SLT_MATSPECULAR LSG_SLT_MATPOWER LSG_SLT_REFLECTIVITY LSG_SLT_ROUGHNESS

%token LSG_SLT_FOGSTART LSG_SLT_FOGEND LSG_SLT_FOGCOLOR

%token LSG_SLT_SHADOWMAPPING

%token LSG_SLT_NORMALMATRIX LSG_SLT_TEXMATRIX LSG_SLT_TEXUNIT LSG_SLT_TIMESINCELASTFRAME LSG_SLT_SHADOWMAPUNIT LSG_SLT_SHADOWMAPMATRIX LSG_SLT_SHADOWMAPTEXDIM LSG_SLT_SHADOWMAPUVDEPTHSCALES LSG_SLT_SHADOWMAPCASTERSIZE LSG_SLT_CAMPOS LSG_SLT_VIEW LSG_SLT_WORLD LSG_SLT_WORLDVIEW LSG_SLT_WORLDVIEWPROJ

%type <ndData>														identifier
%type <ndData>														basic_exp
%type <ndData>														integer_exp
%type <ndData>														postfix_exp
%type <ndData>														unary_exp
%type <ndData>														cast_exp
%type <ndData>														multiplicative_exp
%type <ndData>														additive_exp
%type <ndData>														shift_exp
%type <ndData>														relational_exp
%type <ndData>														equality_exp
%type <ndData>														logical_and_exp
%type <ndData>														logical_xor_exp
%type <ndData>														logical_or_exp
%type <ndData>														and_exp
%type <ndData>														xor_exp
%type <ndData>														or_exp
%type <ndData>														conditional_exp
%type <ndData>														assignment_exp
%type <ndData>														exp
%type <ndData>														constant_exp
%type <ndData>														intrinsic
%type <ndData>														argument_exp_list
%type <ndData>														type_specifier_no_prec
%type <ndData>														type_specifier
%type <ndData>														fully_specified_type
%type <ndData>														single_decl
%type <ndData>														init_decl_list
%type <ndData>														declaration
%type <ndData>														declaration_statement
%type <ndData>														parameter_semantic
%type <ndData>														parameter_declarator
%type <ndData>														parameter_declaration
%type <ndData>														parameter_declaration_list
%type <ndData>														function_declaration
%type <ndData>														function_definition
%type <ndData>														initializer
%type <ndData>														initializer_list
%type <ndData>														struct_declarator
%type <ndData>														struct_declarator_list
%type <ndData>														struct_declaration
%type <ndData>														struct_declaration_list
%type <ndData>														struct_specifier
%type <ndData>														selection_statement
%type <ndData>														for_init_statement_decl
%type <ndData>														for_init_statement
%type <ndData>														iteration_statement
%type <ndData>														jump_statement
%type <ndData>														expression_statement
%type <ndData>														simple_statement
%type <ndData>														statement
%type <ndData>														statement_list
%type <ndData>														compound_statement
%type <ndData>														external_decl
%type <ndData>														translation_unit
%type <ui32StringIndex>												string
%type <ui64Op>														unary_op
%type <ui64Op>														assignment_ops
%type <ui64Op>														type_specifier_nonarray
%type <ui64Op>														storage_qualifier
%type <ui64Op>														interpolation_qualifier
%type <ui64Op>														type_qualifier
%type <ui64Op>														parameter_qualifier
%type <ui64Op>														parameter_type_qualifier

%start translation_unit

%right	LSG_SLT_XOR_EQUALS LSG_SLT_OR_EQUALS LSG_SLT_AND_EQUALS LSG_SLT_SHIFT_RIGHT_EQUALS LSG_SLT_SHIFT_LEFT_EQUALS LSG_SLT_MINUS_EQUALS LSG_SLT_PLUS_EQUALS LSG_SLT_MOD_EQUALS LSG_SLT_DIV_EQUALS LSG_SLT_TIMES_EQUALS '='
%left	LSG_SLT_OR
%left	LSG_SLT_XOR
%left	LSG_SLT_AND
%left	'|'
%left	'^'
%left	'&'
%left	LSG_SLT_NOT_EQUALS  LSG_SLT_EQUALS
%left	LSG_SLT_GE  LSG_SLT_LE  '>'  '<'
%left	LSG_SLT_SHIFT_RIGHT  LSG_SLT_SHIFT_LEFT
%left   '+'  '-'
%left   '*'  '/'  '%'
%right	LSG_SLT_NOT LSG_SLT_UMINUS LSG_SLT_UPLUS

%%


string
	: LSG_SLT_STRING_CONSTANT										{ $$ = m_pslpContainer->CreateString( m_pslLexer->YYText() ); }
	;

identifier
	: LSG_SLT_IDENTIFIER											{ m_pslpContainer->CreateIdentifier( m_pslLexer->YYText(), $$ ); };
	;

basic_exp
	: string														{
																		$$.nNodeType = LSG_N_STRINGCONSTANT;
																		$$.nuNodeData.ui32StringIndex = $1;
																	}
	| LSG_SLT_FLOAT_CONSTANT										{ m_pslpContainer->CreateFloatingPoint( m_pslLexer->YYText(), $$ ); }
	| LSG_SLT_DEC_CONSTANT											{ m_pslpContainer->CreateDecimal( m_pslLexer->YYText(), $$ ); }
	| LSG_SLT_FALSE													{ m_pslpContainer->CreateBool( false, $$ ); }
	| LSG_SLT_TRUE													{ m_pslpContainer->CreateBool( true, $$ ); }
	| identifier													{ $$ = $1; }
	| '(' exp ')'													{ $$ = $2; }
	;

integer_exp
	: exp															{
																		if ( !m_pslpContainer->IsInteger( $1 ) ) {
																			error( yylloc, "Expression must resolve to an integer value." );
																		}
																		$$ = $1;
																	}
	;

postfix_exp
	: basic_exp														{ $$ = $1; }
	| postfix_exp '[' integer_exp ']'								{ m_pslpContainer->CreateArrayAccess( $1, $3, $$ ); }
	| postfix_exp LSG_SLT_PLUS_PLUS									{ m_pslpContainer->CreatePostfixOrPrefixIncOrDec( $1, LSG_N_POSTFIXINC, $$ ); }
	| postfix_exp LSG_SLT_MINUS_MINUS								{ m_pslpContainer->CreatePostfixOrPrefixIncOrDec( $1, LSG_N_POSTFIXDEC, $$ ); }
	| intrinsic														{ $$ = $1; }
	| postfix_exp '(' ')'											{ m_pslpContainer->CreateFunctionCall( $1, LSE_MAXU32, $$ ); }
	| postfix_exp '(' argument_exp_list ')'							{ m_pslpContainer->CreateFunctionCall( $1, $3.ui32NodeIndex, $$ ); }
	| postfix_exp '.' identifier									{ m_pslpContainer->CreateFieldSelect( $1, $3, $$ ); }
	| type_specifier_nonarray '(' ')'								{ m_pslpContainer->CreateConstructor( $1, LSE_MAXU32, $$ ); }
	| type_specifier_nonarray '(' argument_exp_list ')'				{ m_pslpContainer->CreateConstructor( $1, $3.ui32NodeIndex, $$ ); }
	;

unary_op
	: '!' %prec LSG_SLT_NOT											{ $$ = '!'; }
	//| '~' %prec LSG_SLT_1SCOMPLIMENT								{ $$ = '~'; }
	| '-' %prec LSG_SLT_UMINUS										{ $$ = '-'; }
	| '+' %prec LSG_SLT_UPLUS										{ $$ = '+'; }
	;

unary_exp
	: postfix_exp													{ $$ = $1; }
	| unary_op unary_exp											{ m_pslpContainer->CreateUnaryOp( static_cast<LSUINT32>($1), $2, $$ ); }
	| LSG_SLT_PLUS_PLUS unary_exp									{ m_pslpContainer->CreatePostfixOrPrefixIncOrDec( $2, LSG_N_PREFIXINC, $$ ); }
	| LSG_SLT_MINUS_MINUS unary_exp									{ m_pslpContainer->CreatePostfixOrPrefixIncOrDec( $2, LSG_N_PREFIXDEC, $$ ); }
	;

cast_exp
	: unary_exp														{ $$ = $1; }
	| '(' type_specifier_nonarray ')' cast_exp						{ m_pslpContainer->CreateCast( $2, $4, $$ ); }
	;

multiplicative_exp
	: cast_exp														{ $$ = $1; }
	| multiplicative_exp '*' cast_exp								{ m_pslpContainer->CreateMath( $1, $3, '*', $$ ); }
	| multiplicative_exp '/' cast_exp								{ m_pslpContainer->CreateMath( $1, $3, '/', $$ ); }
	| multiplicative_exp '%' cast_exp								{ m_pslpContainer->CreateMath( $1, $3, '%', $$ ); }
	;

additive_exp
	: multiplicative_exp											{ $$ = $1; }
	| additive_exp '+' multiplicative_exp							{ m_pslpContainer->CreateMath( $1, $3, '+', $$ ); }
	| additive_exp '-' multiplicative_exp							{ m_pslpContainer->CreateMath( $1, $3, '-', $$ ); }
	;

shift_exp
	: additive_exp													{ $$ = $1; }
	| shift_exp LSG_SLT_SHIFT_LEFT additive_exp						{ m_pslpContainer->CreateMath( $1, $3, token::LSG_SLT_SHIFT_LEFT, $$ ); }
	| shift_exp LSG_SLT_SHIFT_RIGHT additive_exp					{ m_pslpContainer->CreateMath( $1, $3, token::LSG_SLT_SHIFT_RIGHT, $$ ); }
	;

relational_exp
	: shift_exp														{ $$ = $1; }
	| relational_exp '>' shift_exp									{ m_pslpContainer->CreateMath( $1, $3, '>', $$ ); }
	| relational_exp '<' shift_exp									{ m_pslpContainer->CreateMath( $1, $3, '<', $$ ); }
	| relational_exp LSG_SLT_GE shift_exp							{ m_pslpContainer->CreateMath( $1, $3, token::LSG_SLT_GE, $$ ); }
	| relational_exp LSG_SLT_LE shift_exp							{ m_pslpContainer->CreateMath( $1, $3, token::LSG_SLT_LE, $$ ); }
	;

equality_exp
	: relational_exp												{ $$ = $1; }
	| equality_exp LSG_SLT_EQUALS relational_exp					{ m_pslpContainer->CreateMath( $1, $3, token::LSG_SLT_EQUALS, $$ ); }
	| equality_exp LSG_SLT_NOT_EQUALS relational_exp				{ m_pslpContainer->CreateMath( $1, $3, token::LSG_SLT_NOT_EQUALS, $$ ); }
	;

logical_and_exp
	: equality_exp													{ $$ = $1; }
	| logical_and_exp '&' equality_exp								{ m_pslpContainer->CreateMath( $1, $3, '&', $$ ); }
	;

logical_xor_exp
	: logical_and_exp												{ $$ = $1; }
	| logical_xor_exp '^' logical_and_exp							{ m_pslpContainer->CreateMath( $1, $3, '^', $$ ); }
	;

logical_or_exp
	: logical_xor_exp												{ $$ = $1; }
	| logical_or_exp '|' logical_xor_exp							{ m_pslpContainer->CreateMath( $1, $3, '|', $$ ); }
	;

and_exp
	: logical_or_exp												{ $$ = $1; }
	| and_exp LSG_SLT_AND logical_or_exp							{ m_pslpContainer->CreateMath( $1, $3, token::LSG_SLT_AND, $$ ); }
	;

xor_exp
	: and_exp 														{ $$ = $1; }
	//| xor_exp LSG_SLT_XOR and_exp									{ m_pslpContainer->CreateMath( $1, $3, token::LSG_SLT_XOR, $$ ); }
	;

or_exp
	: xor_exp 														{ $$ = $1; }
	| or_exp LSG_SLT_OR xor_exp										{ m_pslpContainer->CreateMath( $1, $3, token::LSG_SLT_OR, $$ ); }
	;

conditional_exp
	: or_exp 														{ $$ = $1; }
	| or_exp '?' exp ':' conditional_exp							{ m_pslpContainer->CreateTertiary( $1, $3, $5, $$ ); }
	;

assignment_ops
	: '='															{ $$ = '='; }
	| LSG_SLT_PLUS_EQUALS											{ $$ = token::LSG_SLT_PLUS_EQUALS; }
	| LSG_SLT_MINUS_EQUALS											{ $$ = token::LSG_SLT_MINUS_EQUALS; }
	| LSG_SLT_TIMES_EQUALS											{ $$ = token::LSG_SLT_TIMES_EQUALS; }
	| LSG_SLT_DIV_EQUALS											{ $$ = token::LSG_SLT_DIV_EQUALS; }
	| LSG_SLT_MOD_EQUALS											{ $$ = token::LSG_SLT_MOD_EQUALS; }
	| LSG_SLT_SHIFT_LEFT_EQUALS										{ $$ = token::LSG_SLT_SHIFT_LEFT_EQUALS; }
	| LSG_SLT_SHIFT_RIGHT_EQUALS									{ $$ = token::LSG_SLT_SHIFT_RIGHT_EQUALS; }
	| LSG_SLT_AND_EQUALS											{ $$ = token::LSG_SLT_AND_EQUALS; }
	| LSG_SLT_XOR_EQUALS											{ $$ = token::LSG_SLT_XOR_EQUALS; }
	| LSG_SLT_OR_EQUALS												{ $$ = token::LSG_SLT_OR_EQUALS; }
	;

assignment_exp
	: conditional_exp												{ $$ = $1; }
	| unary_exp assignment_ops assignment_exp						{ m_pslpContainer->CreateAssignment( $1, $3, static_cast<LSUINT32>($2), $$ ); }
	;

exp
	: assignment_exp												{ $$ = $1; }
	| exp ',' assignment_exp										{ m_pslpContainer->CreateComma( $1, $3, $$ ); }
	;

constant_exp
	: conditional_exp												{ $$ = $1; }
	;
	
intrinsic
	: LSG_SLT_ABS '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ABS, $3, $$ ); }
	| LSG_SLT_ACOS '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ACOS, $3, $$ ); }
	| LSG_SLT_ANY '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ANY, $3, $$ ); }
	| LSG_SLT_ASIN '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ASIN, $3, $$ ); }
	| LSG_SLT_ATAN '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ATAN, $3, $$ ); }
	| LSG_SLT_ATAN2 '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ATAN2, $3, $$ ); }
	| LSG_SLT_CEIL '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_CEIL, $3, $$ ); }
	| LSG_SLT_CLAMP '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_CLAMP, $3, $$ ); }
	| LSG_SLT_COS '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_COS, $3, $$ ); }
	| LSG_SLT_CROSS '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_CROSS, $3, $$ ); }
	| LSG_SLT_DDX '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_DDX, $3, $$ ); }
	| LSG_SLT_DDY '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_DDY, $3, $$ ); }
	| LSG_SLT_DEGREES '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_DEGREES, $3, $$ ); }
	| LSG_SLT_DETERMINANT '(' argument_exp_list ')'					{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_DETERMINANT, $3, $$ ); }
	| LSG_SLT_DISTANCE '(' argument_exp_list ')'					{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_DISTANCE, $3, $$ ); }
	| LSG_SLT_DOT '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_DOT, $3, $$ ); }
	| LSG_SLT_EXP '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_EXP, $3, $$ ); }
	| LSG_SLT_EXP2 '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_EXP2, $3, $$ ); }
	| LSG_SLT_FLOOR '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_FLOOR, $3, $$ ); }
	| LSG_SLT_FRAC '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_FRAC, $3, $$ ); }
	| LSG_SLT_ISINF '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ISINF, $3, $$ ); }
	| LSG_SLT_ISNAN '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ISNAN, $3, $$ ); }
	| LSG_SLT_LENGTH '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_LENGTH, $3, $$ ); }
	| LSG_SLT_LERP '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_LERP, $3, $$ ); }
	| LSG_SLT_LOG '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_LOG, $3, $$ ); }
	| LSG_SLT_LOG2 '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_LOG2, $3, $$ ); }
	| LSG_SLT_MAX '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_MAX, $3, $$ ); }
	| LSG_SLT_MIN '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_MIN, $3, $$ ); }
	| LSG_SLT_MIX '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_MIX, $3, $$ ); }
	| LSG_SLT_MOD '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_MOD, $3, $$ ); }
	| LSG_SLT_MUL '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_MUL, $3, $$ ); }
	| LSG_SLT_NORMALIZE '(' argument_exp_list ')'					{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_NORMALIZE, $3, $$ ); }
	| LSG_SLT_POW '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_POW, $3, $$ ); }
	| LSG_SLT_RADIANS '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_RADIANS, $3, $$ ); }
	| LSG_SLT_RCP '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_RCP, $3, $$ ); }
	| LSG_SLT_REFLECT '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_REFLECT, $3, $$ ); }
	| LSG_SLT_REFRACT '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_REFRACT, $3, $$ ); }
	| LSG_SLT_RSQRT '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_RSQRT, $3, $$ ); }
	| LSG_SLT_SATURATE '(' argument_exp_list ')'					{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_SATURATE, $3, $$ ); }
	| LSG_SLT_SIN '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_SIN, $3, $$ ); }
	| LSG_SLT_SINCOS '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_SINCOS, $3, $$ ); }
	| LSG_SLT_SMOOTHSTEP '(' argument_exp_list ')'					{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_SMOOTHSTEP, $3, $$ ); }
	| LSG_SLT_SQRT '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_SQRT, $3, $$ ); }
	| LSG_SLT_STEP '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_STEP, $3, $$ ); }
	| LSG_SLT_TAN '(' argument_exp_list ')'							{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TAN, $3, $$ ); }
	| LSG_SLT_TEX1D '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TEX1D, $3, $$ ); }
	| LSG_SLT_TEX2D '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TEX2D, $3, $$ ); }
	| LSG_SLT_TEX3D '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TEX3D, $3, $$ ); }
	| LSG_SLT_TEXCUBE '(' argument_exp_list ')'						{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TEXCUBE, $3, $$ ); }
	| LSG_SLT_TEX2DSHADOW '(' argument_exp_list ')'					{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TEX2DSHADOW, $3, $$ ); }
	| LSG_SLT_TEX2DSHADOWCOMPARE '(' argument_exp_list ')'			{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TEX2DSHADOWCOMPARE, $3, $$ ); }
	| LSG_SLT_TRANSPOSE '(' argument_exp_list ')'					{ m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TRANSPOSE, $3, $$ ); }
	;

argument_exp_list
	: assignment_exp												{ $$ = $1; }
	| argument_exp_list ',' assignment_exp							{ m_pslpContainer->CreateArgExpList( $1, $3, $$ ); }
	;

type_specifier_nonarray
	: LSG_SLT_VOID													{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_VOID, LSE_MAXU32, LSE_MAXU32 ); }
	| LSG_SLT_SAMPLER												{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_SAMPLER, LSE_MAXU32, LSE_MAXU32 ); }
	| LSG_SLT_TEXTURE1D												{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_TEXTURE1D, LSE_MAXU32, LSE_MAXU32 ); }
	| LSG_SLT_TEXTURE2D												{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_TEXTURE2D, LSE_MAXU32, LSE_MAXU32 ); }
	| LSG_SLT_TEXTURE3D												{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_TEXTURE3D, LSE_MAXU32, LSE_MAXU32 ); }
	| LSG_SLT_TEXTURECUBE											{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_TEXTURECUBE, LSE_MAXU32, LSE_MAXU32 ); }
	| LSG_SLT_FLOAT													{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ); }
	| LSG_SLT_INT													{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 ); }
	| LSG_SLT_UINT													{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_UINT, LSE_MAXU32, LSE_MAXU32 ); }
	| LSG_SLT_BOOL													{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_BOOL, LSE_MAXU32, LSE_MAXU32 ); }
	| LSG_SLT_VEC2													{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, 2UL, LSE_MAXU32 ); }
	| LSG_SLT_VEC3													{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, 3UL, LSE_MAXU32 ); }
	| LSG_SLT_VEC4													{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, 4UL, LSE_MAXU32 ); }
	| LSG_SLT_VECTOR												{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, 4UL, LSE_MAXU32 ); }
	| LSG_SLT_MATRIX												{ $$ = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, 4UL, 4UL ); }
	| LSG_SLT_FLOAT '[' integer_exp ']'								{
																		CShaderLanguageParser::LSG_SCALAR sValue;
																		m_pslpContainer->GetNodeValue( $3, sValue );
																		if ( sValue.ui64Value < 2ULL || sValue.ui64Value > 4ULL ) {
																			error( yyloc, "Array constant must be 2, 3, or 4." );
																		}
																		$$ = m_pslpContainer->CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, static_cast<LSUINT32>(sValue.ui64Value), LSE_MAXU32 );
																	}
	| LSG_SLT_INT '[' integer_exp ']'								{
																		CShaderLanguageParser::LSG_SCALAR sValue;
																		m_pslpContainer->GetNodeValue( $3, sValue );
																		if ( sValue.ui64Value < 2ULL || sValue.ui64Value > 4ULL ) {
																			error( yyloc, "Array constant must be 2, 3, or 4." );
																		}
																		$$ = m_pslpContainer->CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_INT, static_cast<LSUINT32>(sValue.ui64Value), LSE_MAXU32 );
																	}
	| LSG_SLT_UINT '[' integer_exp ']'								{
																		CShaderLanguageParser::LSG_SCALAR sValue;
																		m_pslpContainer->GetNodeValue( $3, sValue );
																		if ( sValue.ui64Value < 2ULL || sValue.ui64Value > 4ULL ) {
																			error( yyloc, "Array constant must be 2, 3, or 4." );
																		}
																		$$ = m_pslpContainer->CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_UINT, static_cast<LSUINT32>(sValue.ui64Value), LSE_MAXU32 );
																	}
	| LSG_SLT_BOOL '[' integer_exp ']'								{
																		CShaderLanguageParser::LSG_SCALAR sValue;
																		m_pslpContainer->GetNodeValue( $3, sValue );
																		if ( sValue.ui64Value < 2ULL || sValue.ui64Value > 4ULL ) {
																			error( yyloc, "Array constant must be 2, 3, or 4." );
																		}
																		$$ = m_pslpContainer->CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_BOOL, static_cast<LSUINT32>(sValue.ui64Value), LSE_MAXU32 );
																	}
	| LSG_SLT_FLOAT '[' integer_exp ']' '[' integer_exp ']'			{
																		CShaderLanguageParser::LSG_SCALAR sLeft;
																		m_pslpContainer->GetNodeValue( $3, sLeft );
																		if ( sLeft.ui64Value < 2ULL || sLeft.ui64Value > 4ULL ) {
																			error( yyloc, "Array constant must be 2, 3, or 4." );
																		}
																		CShaderLanguageParser::LSG_SCALAR sRight;
																		m_pslpContainer->GetNodeValue( $6, sRight );
																		if ( sRight.ui64Value < 2ULL || sRight.ui64Value > 4ULL ) {
																			error( yyloc, "Array constant must be 2, 3, or 4." );
																		}
																		$$ = m_pslpContainer->CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, static_cast<LSUINT32>(sLeft.ui64Value), static_cast<LSUINT32>(sRight.ui64Value) );
																	}
	| LSG_SLT_TYPENAME												{ $$ = m_pslpContainer->CShaderLanguageParser::CombineTypesAndArrays( m_pslpContainer->GetStructTypeId( m_pslLexer->YYText() ), LSE_MAXU32, LSE_MAXU32 ); }
	;

type_specifier_no_prec
	: type_specifier_nonarray										{ m_pslpContainer->CreateTypeSpecifier( $1, $$ ); }
	;

type_specifier
	: type_specifier_no_prec										{ $$ = $1; }
	;

storage_qualifier
	: LSG_SLT_CONST													{ $$ = LSG_SQ_CONST; }
	| LSG_SLT_IN													{ $$ = LSG_SQ_IN; }
	| LSG_SLT_OUT													{ $$ = LSG_SQ_OUT; }
	| LSG_SLT_UNIFORM												{ $$ = LSG_SQ_UNIFORM; }
	;

interpolation_qualifier
	: LSG_SLT_NOINTERPOLATION										{ $$ = LSG_IQ_FLAT; }
	;

type_qualifier
	: storage_qualifier												{ $$ = $1; }
	| interpolation_qualifier type_qualifier						{ $$ = $1 | $2; }
	;

fully_specified_type
	: type_specifier												{ m_pslpContainer->CreateFullySpecifiedType( 0ULL, $1, $$ ); }
	| type_qualifier type_specifier									{ m_pslpContainer->CreateFullySpecifiedType( $1, $2, $$ ); }
	;

single_decl
	: fully_specified_type identifier								{ m_pslpContainer->CreateSingleDeclaration( $2.ui32NodeIndex, $1, LSE_MAXU32, LSE_MAXU32, LSE_MAXU32, $$ ); }
	| fully_specified_type identifier '[' constant_exp ']'			{ m_pslpContainer->CreateSingleDeclaration( $2.ui32NodeIndex, $1, $4.ui32NodeIndex, LSE_MAXU32, LSE_MAXU32, $$ ); }
	| fully_specified_type identifier '=' initializer				{ m_pslpContainer->CreateSingleDeclaration( $2.ui32NodeIndex, $1, LSE_MAXU32, LSE_MAXU32, $4.ui32NodeIndex, $$ ); }
	| fully_specified_type identifier '[' constant_exp ']'
		'=' initializer												{ m_pslpContainer->CreateSingleDeclaration( $2.ui32NodeIndex, $1, $4.ui32NodeIndex, LSE_MAXU32, $7.ui32NodeIndex, $$ ); }
	| fully_specified_type identifier ':' parameter_semantic		{ m_pslpContainer->CreateSingleDeclaration( $2.ui32NodeIndex, $1, LSE_MAXU32, $4.ui32NodeIndex, LSE_MAXU32, $$ ); }
	| fully_specified_type identifier '[' constant_exp ']' ':' parameter_semantic
																	{ m_pslpContainer->CreateSingleDeclaration( $2.ui32NodeIndex, $1, $4.ui32NodeIndex, $7.ui32NodeIndex, LSE_MAXU32, $$ ); }
	| fully_specified_type identifier ':' parameter_semantic '=' initializer
																	{ m_pslpContainer->CreateSingleDeclaration( $2.ui32NodeIndex, $1, LSE_MAXU32, $4.ui32NodeIndex, $6.ui32NodeIndex, $$ ); }
	| fully_specified_type identifier '[' constant_exp ']' ':' parameter_semantic '=' initializer
																	{ m_pslpContainer->CreateSingleDeclaration( $2.ui32NodeIndex, $1, $4.ui32NodeIndex, $7.ui32NodeIndex, $9.ui32NodeIndex, $$ ); }
	;

init_decl_list
	: single_decl													{ $$ = $1; }
	| init_decl_list ',' identifier									{
																		m_pslpContainer->CreateSingleDeclaration( $3.ui32NodeIndex,
																			m_pslpContainer->GetNode( $1.nuNodeData.ui32NodeIndex ), LSE_MAXU32, LSE_MAXU32, LSE_MAXU32, $$ );
																	}
	| init_decl_list ',' identifier '[' constant_exp ']'			{
																		m_pslpContainer->CreateSingleDeclaration( $3.ui32NodeIndex,
																			m_pslpContainer->GetNode( $1.nuNodeData.ui32NodeIndex ), $5.ui32NodeIndex, LSE_MAXU32, LSE_MAXU32, $$ );
																	}
	| init_decl_list ',' identifier '=' initializer					{
																		m_pslpContainer->CreateSingleDeclaration( $3.ui32NodeIndex,
																			m_pslpContainer->GetNode( $1.nuNodeData.ui32NodeIndex ), LSE_MAXU32, LSE_MAXU32, $5.ui32NodeIndex, $$ );
																	}
	| init_decl_list ',' identifier ':' parameter_semantic			{
																		m_pslpContainer->CreateSingleDeclaration( $3.ui32NodeIndex,
																			m_pslpContainer->GetNode( $1.nuNodeData.ui32NodeIndex ), LSE_MAXU32, $5.ui32NodeIndex, LSE_MAXU32, $$ );
																	}
	| init_decl_list ',' identifier '[' constant_exp ']' ':' parameter_semantic
																	{
																		m_pslpContainer->CreateSingleDeclaration( $3.ui32NodeIndex,
																			m_pslpContainer->GetNode( $1.nuNodeData.ui32NodeIndex ), $5.ui32NodeIndex, $8.ui32NodeIndex, LSE_MAXU32, $$ );
																	}
	| init_decl_list ',' identifier ':' parameter_semantic '=' initializer
																	{
																		m_pslpContainer->CreateSingleDeclaration( $3.ui32NodeIndex,
																			m_pslpContainer->GetNode( $1.nuNodeData.ui32NodeIndex ), LSE_MAXU32, $5.ui32NodeIndex, $7.ui32NodeIndex, $$ );
																	}
	;

declaration
	: init_decl_list												{ $$ = $1; }
	| struct_specifier												{ $$ = $1; }
	| function_declaration											{ $$ = $1; }
	;

declaration_statement
	: declaration ';'												{ m_pslpContainer->CreateDeclarationStatement( $1, $$ ); }
	;

parameter_semantic
	: LSG_SLT_BINORMAL												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_BINORMAL, $$ ); }
	| LSG_SLT_BLENDINDICES											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_BLENDINDICES, $$ ); }
	| LSG_SLT_BLENDWEIGHT											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_BLENDWEIGHT, $$ ); }
	| LSG_SLT_COLOR													{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_COLOR, $$ ); }
	| LSG_SLT_NORMAL												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_NORMAL, $$ ); }
	| LSG_SLT_POSITION												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_POSITION, $$ ); }
	| LSG_SLT_POSITIONT												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_POSITIONT, $$ ); }
	| LSG_SLT_PSIZE													{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_PSIZE, $$ ); }
	| LSG_SLT_TANGENT												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_TANGENT, $$ ); }
	| LSG_SLT_TEXCOORD												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_TEXCOORD, $$ ); }
	| LSG_SLT_FOG													{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_FOG, $$ ); }
	| LSG_SLT_TESSFACTOR											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_TESSFACTOR, $$ ); }
	| LSG_SLT_ISFRONTFACE											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_ISFRONTFACE, $$ ); }
	| LSG_SLT_VPOS													{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_VPOS, $$ ); }
	| LSG_SLT_DEPTH													{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_DEPTH, $$ ); }
	| LSG_SLT_AMBIENTLIGHT											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_AMBIENTLIGHT, $$ ); }
	| LSG_SLT_LIGHTCOUNT											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_LIGHTCOUNT, $$ ); }
	| LSG_SLT_DIRLIGHTCOUNT											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_DIRLIGHTCOUNT, $$ ); }
	| LSG_SLT_POINTLIGHTCOUNT										{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_POINTLIGHTCOUNT, $$ ); }
	| LSG_SLT_SPOTLIGHTCOUNT										{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SPOTLIGHTCOUNT, $$ ); }
	| LSG_SLT_DIRLIGHTDIRS											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_DIRLIGHTDIRS, $$ ); }
	| LSG_SLT_DIRLIGHTDIFFUSES										{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_DIRLIGHTDIFFUSES, $$ ); }
	| LSG_SLT_POINTLIGHTPOS											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_POINTLIGHTPOS, $$ ); }
	| LSG_SLT_POINTLIGHTDIFFUSES									{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_POINTLIGHTDIFFUSES, $$ ); }
	| LSG_SLT_POINTLIGHTSIZES										{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_POINTLIGHTSIZES, $$ ); }
	| LSG_SLT_SPOTLIGHTDIRS											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SPOTLIGHTDIRS, $$ ); }
	| LSG_SLT_LIGHTAMBIENT											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_LIGHTAMBIENT, $$ ); }
	| LSG_SLT_LIGHTDIFFUSE											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_LIGHTDIFFUSE, $$ ); }
	| LSG_SLT_LIGHTSPECULAR											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_LIGHTSPECULAR, $$ ); }
	| LSG_SLT_LIGHTATT												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_LIGHTATT, $$ ); }
	| LSG_SLT_SPOTLIGHTRANGE										{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SPOTLIGHTRANGE, $$ ); }
	| LSG_SLT_SKYCOLOR												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SKYCOLOR, $$ ); }
	| LSG_SLT_GROUNDCOLOR											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_GROUNDCOLOR, $$ ); }
	| LSG_SLT_MATAMBIENT											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_MATAMBIENT, $$ ); }
	| LSG_SLT_MATDIFFUSE											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_MATDIFFUSE, $$ ); }
	| LSG_SLT_MATEMISSIVE											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_MATEMISSIVE, $$ ); }
	| LSG_SLT_MATSPECULAR											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_MATSPECULAR, $$ ); }
	| LSG_SLT_MATPOWER												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_MATPOWER, $$ ); }
	| LSG_SLT_REFLECTIVITY											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_REFLECTIVITY, $$ ); }
	| LSG_SLT_ROUGHNESS												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_ROUGHNESS, $$ ); }
	| LSG_SLT_ASHIKFACTORS											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_ASHIKFACTORS, $$ ); }
	| LSG_SLT_FOGSTART												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_FOGSTART, $$ ); }
	| LSG_SLT_FOGEND												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_FOGEND, $$ ); }
	| LSG_SLT_FOGCOLOR												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_FOGCOLOR, $$ ); }
	| LSG_SLT_SHADOWMAPPING											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SHADOWMAPPING, $$ ); }
	| LSG_SLT_NORMALMATRIX											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_NORMALMATRIX, $$ ); }
	| LSG_SLT_TEXMATRIX												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_TEXMATRIX, $$ ); }
	| LSG_SLT_TEXUNIT												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_TEXUNIT, $$ ); }
	| LSG_SLT_TIMESINCELASTFRAME									{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_TIMESINCELASTFRAME, $$ ); }
	| LSG_SLT_SHADOWMAPUNIT											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SHADOWMAPUNIT, $$ ); }
	| LSG_SLT_SHADOWMAPMATRIX										{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SHADOWMAPMATRIX, $$ ); }
	| LSG_SLT_SHADOWMAPUVDEPTHSCALES								{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SHADOWMAPUVDEPTHSCALES, $$ ); }
	| LSG_SLT_SHADOWMAPCASTERSIZE									{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SHADOWMAPCASTERSIZE, $$ ); }
	| LSG_SLT_SHADOWMAPTEXDIM										{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SHADOWMAPTEXDIM, $$ ); }
	| LSG_SLT_CAMPOS												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_CAMPOS, $$ ); }
	| LSG_SLT_VIEW													{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_VIEW, $$ ); }
	| LSG_SLT_WORLD													{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_WORLD, $$ ); }
	| LSG_SLT_WORLDVIEW												{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_WORLDVIEW, $$ ); }
	| LSG_SLT_WORLDVIEWPROJ											{ m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_WORLDVIEWPROJ, $$ ); }
	;

parameter_qualifier
	: LSG_SLT_IN													{ $$ = LSG_SQ_IN; }
	| LSG_SLT_OUT													{ $$ = LSG_SQ_OUT; }
	| LSG_SLT_INOUT													{ $$ = LSG_SQ_INOUT; }
	;

parameter_type_qualifier
	: LSG_SLT_CONST													{ $$ = LSG_SQ_CONST; }
	;

parameter_declarator
	: type_specifier identifier										{ m_pslpContainer->CreateParameterDeclarator( $1, $2, LSE_MAXU32, LSE_MAXU32, $$ ); }
	| type_specifier identifier	'[' constant_exp ']'				{ m_pslpContainer->CreateParameterDeclarator( $1, $2, $4.ui32NodeIndex, LSE_MAXU32, $$ ); }
	| type_specifier identifier ':' parameter_semantic				{ m_pslpContainer->CreateParameterDeclarator( $1, $2, LSE_MAXU32, $4.ui32NodeIndex, $$ ); }
	;

parameter_declaration
	: parameter_type_qualifier parameter_qualifier parameter_declarator
																	{ m_pslpContainer->CreateParameterDeclaration( $1 | $2, $3, $$ ); }
	| parameter_qualifier parameter_declarator						{ m_pslpContainer->CreateParameterDeclaration( $1, $2, $$ ); }
	| parameter_type_qualifier parameter_declarator
																	{ m_pslpContainer->CreateParameterDeclaration( $1, $2, $$ ); }
	| parameter_declarator											{ m_pslpContainer->CreateParameterDeclaration( 0UL, $1, $$ ); }
	;

parameter_declaration_list
	: parameter_declaration											{ $$ = $1; }
	| parameter_declaration_list ',' parameter_declaration			{ m_pslpContainer->CreateParameterDeclarationList( $1, $3, $$ ); }
	;

function_declaration
	: fully_specified_type identifier '(' parameter_declaration_list ')'
																	{ m_pslpContainer->CreateFunctionDeclaration( $1, $2, $4.ui32NodeIndex, $$ ); }
	| fully_specified_type identifier '(' LSG_SLT_VOID ')'			{ m_pslpContainer->CreateFunctionDeclaration( $1, $2, LSE_MAXU32, $$ ); }
	| fully_specified_type identifier '(' ')'						{ m_pslpContainer->CreateFunctionDeclaration( $1, $2, LSE_MAXU32, $$ ); }
	;

function_definition
	: function_declaration compound_statement						{ m_pslpContainer->CreateFunctionDefinition( $1, $2, $$ ); }
	;

initializer
	: assignment_exp												{ $$ = $1; }
	| '{' initializer_list '}'										{ m_pslpContainer->CreateInitializerListWrap( $2, $$ ); }
	| '{' initializer_list ',' '}'									{ m_pslpContainer->CreateInitializerListWrap( $2, $$ ); }
	;

initializer_list
	: initializer													{ $$ = $1; }
	| initializer_list ',' initializer								{ m_pslpContainer->CreateInitializerList( $1, $3, $$ ); }
	;

struct_declarator
	: identifier													{ m_pslpContainer->CreateStructDeclarator( $1, LSE_MAXU32, $$ ); }
	| identifier '[' constant_exp ']'								{ m_pslpContainer->CreateStructDeclarator( $1, $3.ui32NodeIndex, $$ ); }
	;

struct_declarator_list
	: struct_declarator												{ $$ = $1; }
	| struct_declarator_list ',' struct_declarator					{ m_pslpContainer->CreateStructDeclaratorList( $1, $3, $$ ); }
	;

struct_declaration
	: type_specifier struct_declarator_list ';'						{ m_pslpContainer->CreateStructDeclaration( 0UL, $1, $2, $$ ); }
	| type_qualifier type_specifier struct_declarator_list ';'		{ m_pslpContainer->CreateStructDeclaration( $1, $2, $3, $$ ); }
	;

struct_declaration_list
	: struct_declaration											{ $$ = $1; }
	| struct_declaration_list struct_declaration					{ m_pslpContainer->CreateStructDeclarationList( $1, $2, $$ ); }
	;

struct_specifier
	: LSG_SLT_STRUCT identifier '{' struct_declaration_list '}'		{ m_pslpContainer->CreateStruct( $2.ui32NodeIndex, $4, $$ ); }
	| LSG_SLT_STRUCT '{' struct_declaration_list '}'				{ m_pslpContainer->CreateStruct( LSE_MAXU32, $3, $$ ); }
	;

selection_statement
	: LSG_SLT_IF '(' exp ')' statement								{ m_pslpContainer->CreateSelectionStatement( $3, $5, LSE_MAXU32, $$ ); }
	| LSG_SLT_IF '(' exp ')' statement LSG_SLT_ELSE statement		{ m_pslpContainer->CreateSelectionStatement( $3, $5, $7.ui32NodeIndex, $$ ); }
	;

for_init_statement_decl
	: single_decl ';'												{ m_pslpContainer->CreateForInitDeclStatement( $1, $$ ); }
	;

for_init_statement
	: expression_statement											{ $$ = $1; }
	| for_init_statement_decl										{ $$ = $1; }
	;

iteration_statement
	: LSG_SLT_WHILE '(' exp ')' statement							{ m_pslpContainer->CreateWhileLoop( $3, $5, $$ ); }
	| LSG_SLT_DO statement LSG_SLT_WHILE '(' exp ')' ';'			{ m_pslpContainer->CreateDoWhileLoop( $5, $2, $$ ); }
	| LSG_SLT_FOR '(' for_init_statement expression_statement ')' statement
																	{ m_pslpContainer->CreateForLoop( $3, $4, LSE_MAXU32, $6, $$ ); }
	| LSG_SLT_FOR '(' for_init_statement expression_statement exp ')' statement
																	{ m_pslpContainer->CreateForLoop( $3, $4, $5.ui32NodeIndex, $7, $$ ); }
	;

jump_statement
	: LSG_SLT_CONTINUE ';'											{ m_pslpContainer->CreateJump( LSG_N_CONTINUE, LSE_MAXU32, $$ ); }
	| LSG_SLT_BREAK ';'												{ m_pslpContainer->CreateJump( LSG_N_BREAK, LSE_MAXU32, $$ ); }
	| LSG_SLT_RETURN ';'											{ m_pslpContainer->CreateJump( LSG_N_RETURN, LSE_MAXU32, $$ ); }
	| LSG_SLT_RETURN exp ';'										{ m_pslpContainer->CreateJump( LSG_N_RETURN, $2.ui32NodeIndex, $$ ); }
	| LSG_SLT_DISCARD ';'											{ m_pslpContainer->CreateJump( LSG_N_DISCARD, LSE_MAXU32, $$ ); }
	;

expression_statement
	: ';'															{ m_pslpContainer->CreateExpressionStatement( LSE_MAXU32, $$ ); }
	| exp ';'														{ m_pslpContainer->CreateExpressionStatement( $1.ui32NodeIndex, $$ ); }
	;

simple_statement
	: expression_statement											{ $$ = $1; }
	| declaration_statement											{ $$ = $1; }
	| selection_statement											{ $$ = $1; }
	| iteration_statement											{ $$ = $1; }
	| jump_statement												{ $$ = $1; }
	;

statement
	: simple_statement												{ $$ = $1; }
	| compound_statement											{ $$ = $1; }
	;

statement_list
	: statement														{ $$ = $1; }
	| statement_list statement										{ m_pslpContainer->CreateStatementList( $1, $2, $$ ); }
	;

compound_statement
	: '{' '}'														{ m_pslpContainer->CreateBlankStatement( $$ ); }
	| '{' statement_list '}'										{ m_pslpContainer->CreateNewScopedStatement( $2, $$ ); }
	;

external_decl
	: function_definition											{ $$ = $1; }
	| declaration_statement											{ $$ = $1; }
	;

translation_unit
	: external_decl													{ $$ = $1; }
	| translation_unit external_decl								{ m_pslpContainer->CreateExternalDeclarationList( $1, $2, $$ ); }
	;


%%


int yylex( /*YYSTYPE*/void * /*_pvNodeUnion*/, lsg::CShaderLexer * _pslLexer ) {
	return _pslLexer->NewLex();
}


void yy::parser::error( const yy::location &/*_lLoc*/, const std::string &_strM ) {
	std::string sError;
	std::string sFile;
	int iLine;
	m_pslLexer->get_line( sFile, iLine );
	if ( sFile.size() ) {
		// In a file.
		sError = "ERROR: " + sFile + " line: " + CString::CreateFromUI64( iLine ).CStr() + ": " + _strM;
	}
	else {
		sError = "ERROR: line: " + std::string( CString::CreateFromUI64( iLine ).CStr() ) + ": " + _strM;
	}
	CStd::DebugPrintA( sError.c_str() );
}

#pragma warning( pop )