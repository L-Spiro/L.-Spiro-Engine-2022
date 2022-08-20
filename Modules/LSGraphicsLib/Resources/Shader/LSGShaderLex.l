%option								yylineno

D									[0-9]
O									[0-7]
L									[a-zA-Z_]
H									[a-fA-F0-9]
E									[Ee][+-]?{D}+
FS									(f|F|l|L)
IS									(u|U|l|L)*
WS									[ \t\v\r\n\f]

%x									comment0

%{
#include "../ShaderLanguage/LSGShaderSyntaxNodes.h"	// Must be included before LSGShaderParser.h.
#include "LSGShaderParser.h"
#include "String/LSTLString.h"



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define YY_NEVER_INTERACTIVE		1

%}


%%


"//".*								{}
"/*"								BEGIN( comment0 );
<comment0>[^*\n]*					{ }
<comment0>"*"+[^*/\n]*				{ }
<comment0>\n						{ }
<comment0>"*"+"/"					BEGIN( INITIAL );

^{WS}*"#"{WS}*"line"{WS}*\"(\\.|[^\\"])*\"{WS}*{D}+{IS}?			{ set_file( yytext ); }

(?i:BlendState)						{ return CShaderParser::token::LSG_SLT_BLENDSTATE; }
(?i:Bool)							{ return CShaderParser::token::LSG_SLT_BOOL; }
(?i:Break)							{ return CShaderParser::token::LSG_SLT_BREAK; }
(?i:Const)							{ return CShaderParser::token::LSG_SLT_CONST; }
(?i:Continue)						{ return CShaderParser::token::LSG_SLT_CONTINUE; }
(?i:Discard)						{ return CShaderParser::token::LSG_SLT_DISCARD; }
(?i:Do)								{ return CShaderParser::token::LSG_SLT_DO; }
(?i:Else)							{ return CShaderParser::token::LSG_SLT_ELSE; }
(?i:False)							{ return CShaderParser::token::LSG_SLT_FALSE; }
(?i:Float)							{ return CShaderParser::token::LSG_SLT_FLOAT; }
(?i:Float2)							{ return CShaderParser::token::LSG_SLT_VEC2; }
(?i:Float3)							{ return CShaderParser::token::LSG_SLT_VEC3; }
(?i:Float4)							{ return CShaderParser::token::LSG_SLT_VEC4; }
(?i:For)							{ return CShaderParser::token::LSG_SLT_FOR; }
(?i:If)								{ return CShaderParser::token::LSG_SLT_IF; }
(?i:In)								{ return CShaderParser::token::LSG_SLT_IN; }
(?i:Inout)							{ return CShaderParser::token::LSG_SLT_INOUT; }
(?i:Int)							{ return CShaderParser::token::LSG_SLT_INT; }
(?i:Matrix)							{ return CShaderParser::token::LSG_SLT_MATRIX; }
(?i:Nointerpolation)				{ return CShaderParser::token::LSG_SLT_NOINTERPOLATION; }
(?i:Out)							{ return CShaderParser::token::LSG_SLT_OUT; }
(?i:Return)							{ return CShaderParser::token::LSG_SLT_RETURN; }
(?i:Sampler)						{ return CShaderParser::token::LSG_SLT_SAMPLER; }
(?i:Smooth)							{ return CShaderParser::token::LSG_SLT_SMOOTH; }
(?i:Struct)							{ return CShaderParser::token::LSG_SLT_STRUCT; }
(?i:Switch)							{ return CShaderParser::token::LSG_SLT_SWITCH; }
(?i:Texture1d)						{ return CShaderParser::token::LSG_SLT_TEXTURE1D; }
(?i:Texture2d)						{ return CShaderParser::token::LSG_SLT_TEXTURE2D; }
(?i:Texture3d)						{ return CShaderParser::token::LSG_SLT_TEXTURE3D; }
(?i:TextureCube)					{ return CShaderParser::token::LSG_SLT_TEXTURECUBE; }
(?i:True)							{ return CShaderParser::token::LSG_SLT_TRUE; }
(?i:Uint)							{ return CShaderParser::token::LSG_SLT_UINT; }
(?i:Uniform)						{ return CShaderParser::token::LSG_SLT_UNIFORM; }
(?i:Vec2)							{ return CShaderParser::token::LSG_SLT_VEC2; }
(?i:Vec3)							{ return CShaderParser::token::LSG_SLT_VEC3; }
(?i:Vec4)							{ return CShaderParser::token::LSG_SLT_VEC4; }
(?i:Vector)							{ return CShaderParser::token::LSG_SLT_VECTOR; }
(?i:Void)							{ return CShaderParser::token::LSG_SLT_VOID; }
(?i:While)							{ return CShaderParser::token::LSG_SLT_WHILE; }

"abs"								{ return CShaderParser::token::LSG_SLT_ABS; }
"acos"								{ return CShaderParser::token::LSG_SLT_ACOS; }
"any"								{ return CShaderParser::token::LSG_SLT_ANY; }
"asin"								{ return CShaderParser::token::LSG_SLT_ASIN; }
"atan"								{ return CShaderParser::token::LSG_SLT_ATAN; }
"atan2"								{ return CShaderParser::token::LSG_SLT_ATAN2; }
"ceil"								{ return CShaderParser::token::LSG_SLT_CEIL; }
"clamp"								{ return CShaderParser::token::LSG_SLT_CLAMP; }
"cos"								{ return CShaderParser::token::LSG_SLT_COS; }
"cross"								{ return CShaderParser::token::LSG_SLT_CROSS; }
"ddx"								{ return CShaderParser::token::LSG_SLT_DDX; }
"ddy"								{ return CShaderParser::token::LSG_SLT_DDY; }
"degrees"							{ return CShaderParser::token::LSG_SLT_DEGREES; }
"determinant"						{ return CShaderParser::token::LSG_SLT_DETERMINANT; }
"distance"							{ return CShaderParser::token::LSG_SLT_DISTANCE; }
"dot"								{ return CShaderParser::token::LSG_SLT_DOT; }
"exp"								{ return CShaderParser::token::LSG_SLT_EXP; }
"exp2"								{ return CShaderParser::token::LSG_SLT_EXP2; }
"floor"								{ return CShaderParser::token::LSG_SLT_FLOOR; }
"frac"								{ return CShaderParser::token::LSG_SLT_FRAC; }
"isinf"								{ return CShaderParser::token::LSG_SLT_ISINF; }
"isnan"								{ return CShaderParser::token::LSG_SLT_ISNAN; }
"length"							{ return CShaderParser::token::LSG_SLT_LENGTH; }
"lerp"								{ return CShaderParser::token::LSG_SLT_LERP; }
"log"								{ return CShaderParser::token::LSG_SLT_LOG; }
"log2"								{ return CShaderParser::token::LSG_SLT_LOG2; }
"max"								{ return CShaderParser::token::LSG_SLT_MAX; }
"min"								{ return CShaderParser::token::LSG_SLT_MIN; }
"mix"								{ return CShaderParser::token::LSG_SLT_MIX; }
"mod"								{ return CShaderParser::token::LSG_SLT_MOD; }
"mul"								{ return CShaderParser::token::LSG_SLT_MUL; }
"normalize"							{ return CShaderParser::token::LSG_SLT_NORMALIZE; }
"pow"								{ return CShaderParser::token::LSG_SLT_POW; }
"radians"							{ return CShaderParser::token::LSG_SLT_RADIANS; }
"rcp"								{ return CShaderParser::token::LSG_SLT_RCP; }
"reflect"							{ return CShaderParser::token::LSG_SLT_REFLECT; }
"refract"							{ return CShaderParser::token::LSG_SLT_REFRACT; }
"rsqrt"								{ return CShaderParser::token::LSG_SLT_RSQRT; }
"saturate"							{ return CShaderParser::token::LSG_SLT_SATURATE; }
"sin"								{ return CShaderParser::token::LSG_SLT_SIN; }
"sincos"							{ return CShaderParser::token::LSG_SLT_SINCOS; }
"smoothstep"						{ return CShaderParser::token::LSG_SLT_SMOOTHSTEP; }
"sqrt"								{ return CShaderParser::token::LSG_SLT_SQRT; }
"step"								{ return CShaderParser::token::LSG_SLT_STEP; }
"tan"								{ return CShaderParser::token::LSG_SLT_TAN; }
"tex1d"								{ return CShaderParser::token::LSG_SLT_TEX1D; }
"tex2d"								{ return CShaderParser::token::LSG_SLT_TEX2D; }
"tex3d"								{ return CShaderParser::token::LSG_SLT_TEX3D; }
"texCube"							{ return CShaderParser::token::LSG_SLT_TEXCUBE; }
"tex2dShadow"						{ return CShaderParser::token::LSG_SLT_TEX2DSHADOW; }
"tex2dShadowCompare"				{ return CShaderParser::token::LSG_SLT_TEX2DSHADOWCOMPARE; }
"transpose"							{ return CShaderParser::token::LSG_SLT_TRANSPOSE; }

"BINORMAL"{D}*						{ return CShaderParser::token::LSG_SLT_BINORMAL; }
"BLENDINDICES"{D}*					{ return CShaderParser::token::LSG_SLT_BLENDINDICES; }
"BLENDWEIGHT"{D}*					{ return CShaderParser::token::LSG_SLT_BLENDWEIGHT; }
"COLOR"{D}*							{ return CShaderParser::token::LSG_SLT_COLOR; }
"NORMAL"{D}*						{ return CShaderParser::token::LSG_SLT_NORMAL; }
"POSITION"{D}*						{ return CShaderParser::token::LSG_SLT_POSITION; }
"POSITIONT"							{ return CShaderParser::token::LSG_SLT_POSITIONT; }
"PSIZE"{D}*							{ return CShaderParser::token::LSG_SLT_PSIZE; }
"TANGENT"{D}*						{ return CShaderParser::token::LSG_SLT_TANGENT; }
"TEXCOORD"{D}*						{ return CShaderParser::token::LSG_SLT_TEXCOORD; }
"FOG"								{ return CShaderParser::token::LSG_SLT_FOG; }
"TESSFACTOR"{D}*					{ return CShaderParser::token::LSG_SLT_TESSFACTOR; }
"ISFRONTFACE"						{ return CShaderParser::token::LSG_SLT_ISFRONTFACE; }
"VPOS"								{ return CShaderParser::token::LSG_SLT_VPOS; }
"DEPTH"{D}*							{ return CShaderParser::token::LSG_SLT_DEPTH; }
"AMBIENTLIGHT"						{ return CShaderParser::token::LSG_SLT_AMBIENTLIGHT; }
"LIGHTCOUNT"						{ return CShaderParser::token::LSG_SLT_LIGHTCOUNT; }
"DIRLIGHTCOUNT"						{ return CShaderParser::token::LSG_SLT_DIRLIGHTCOUNT; }
"POINTLIGHTCOUNT"					{ return CShaderParser::token::LSG_SLT_POINTLIGHTCOUNT; }
"SPOTLIGHTCOUNT"					{ return CShaderParser::token::LSG_SLT_SPOTLIGHTCOUNT; }
"DIRLIGHTDIRS"						{ return CShaderParser::token::LSG_SLT_DIRLIGHTDIRS; }
"DIRLIGHTDIFFUSES"					{ return CShaderParser::token::LSG_SLT_DIRLIGHTDIFFUSES; }
"POINTLIGHTPOS"						{ return CShaderParser::token::LSG_SLT_POINTLIGHTPOS; }
"POINTLIGHTDIFFUSES"				{ return CShaderParser::token::LSG_SLT_POINTLIGHTDIFFUSES; }
"POINTLIGHTSIZES"					{ return CShaderParser::token::LSG_SLT_POINTLIGHTSIZES; }
"SPOTLIGHTDIRS"						{ return CShaderParser::token::LSG_SLT_SPOTLIGHTDIRS; }
"LIGHTAMBIENT"						{ return CShaderParser::token::LSG_SLT_LIGHTAMBIENT; }
"LIGHTDIFFUSE"						{ return CShaderParser::token::LSG_SLT_LIGHTDIFFUSE; }
"LIGHTSPECULAR"						{ return CShaderParser::token::LSG_SLT_LIGHTSPECULAR; }
"LIGHTATT"							{ return CShaderParser::token::LSG_SLT_LIGHTATT; }
"SPOTLIGHTRANGE"					{ return CShaderParser::token::LSG_SLT_SPOTLIGHTRANGE; }
"SKYCOLOR"							{ return CShaderParser::token::LSG_SLT_SKYCOLOR; }
"GROUNDCOLOR"						{ return CShaderParser::token::LSG_SLT_GROUNDCOLOR; }
"MATAMBIENT"						{ return CShaderParser::token::LSG_SLT_MATAMBIENT; }
"MATDIFFUSE"						{ return CShaderParser::token::LSG_SLT_MATDIFFUSE; }
"MATEMISSIVE"						{ return CShaderParser::token::LSG_SLT_MATEMISSIVE; }
"MATSPECULAR"						{ return CShaderParser::token::LSG_SLT_MATSPECULAR; }
"MATPOWER"							{ return CShaderParser::token::LSG_SLT_MATPOWER; }
"REFLECTIVITY"						{ return CShaderParser::token::LSG_SLT_REFLECTIVITY; }
"ROUGHNESS"							{ return CShaderParser::token::LSG_SLT_ROUGHNESS; }
"ASHIKFACTORS"						{ return CShaderParser::token::LSG_SLT_ASHIKFACTORS; }
"FOGSTART"							{ return CShaderParser::token::LSG_SLT_FOGSTART; }
"FOGEND"							{ return CShaderParser::token::LSG_SLT_FOGEND; }
"FOGCOLOR"							{ return CShaderParser::token::LSG_SLT_FOGCOLOR; }
"SHADOWMAPPING"						{ return CShaderParser::token::LSG_SLT_SHADOWMAPPING; }
"NORMALMATRIX"						{ return CShaderParser::token::LSG_SLT_NORMALMATRIX; }
"TEXMATRIX"{D}*						{ return CShaderParser::token::LSG_SLT_TEXMATRIX; }
"TEXUNIT"{D}*						{ return CShaderParser::token::LSG_SLT_TEXUNIT; }
"TIMESINCELASTFRAME"				{ return CShaderParser::token::LSG_SLT_TIMESINCELASTFRAME; }
"SHADOWMAPUNIT"						{ return CShaderParser::token::LSG_SLT_SHADOWMAPUNIT; }
"SHADOWMAPMATRIX"					{ return CShaderParser::token::LSG_SLT_SHADOWMAPMATRIX; }
"SHADOWMAPUVDEPTHSCALES"			{ return CShaderParser::token::LSG_SLT_SHADOWMAPUVDEPTHSCALES; }
"SHADOWMAPCASTERSIZE"				{ return CShaderParser::token::LSG_SLT_SHADOWMAPCASTERSIZE; }
"SHADOWMAPTEXDIM"					{ return CShaderParser::token::LSG_SLT_SHADOWMAPTEXDIM; }
"CAMPOS"							{ return CShaderParser::token::LSG_SLT_CAMPOS; }
"VIEW"								{ return CShaderParser::token::LSG_SLT_VIEW; }
"WORLD"								{ return CShaderParser::token::LSG_SLT_WORLD; }
"WORLDVIEW"							{ return CShaderParser::token::LSG_SLT_WORLDVIEW; }
"WORLDVIEWPROJ"						{ return CShaderParser::token::LSG_SLT_WORLDVIEWPROJ; }

L?'(\\.|[^\\'])+'					{ return CShaderParser::token::LSG_SLT_CHAR_CONSTANT; }
\"(\\.|[^\\"])*\"					{ return CShaderParser::token::LSG_SLT_STRING_CONSTANT; }

{D}+{E}{FS}?						{ return CShaderParser::token::LSG_SLT_FLOAT_CONSTANT; }
{D}*"."{D}+({E})?{FS}?				{ return CShaderParser::token::LSG_SLT_FLOAT_CONSTANT; }
{D}+"."{D}*({E})?{FS}?				{ return CShaderParser::token::LSG_SLT_FLOAT_CONSTANT; }

0{O}+{IS}?							{ return CShaderParser::token::LSG_SLT_OCT_CONSTANT; }
{D}+{IS}?							{ return CShaderParser::token::LSG_SLT_DEC_CONSTANT; }
0[xX]{H}+{IS}?						{ return CShaderParser::token::LSG_SLT_HEX_CONSTANT; }

{L}({L}|{D})*						{ return CShaderParser::token::LSG_SLT_IDENTIFIER; }

"("									{ return '('; }
")"									{ return ')'; }

"-"									{ return '-'; }
"+"									{ return '+'; }
"*"									{ return '*'; }
"/"									{ return '/'; }
"%"									{ return '%'; }

"["									{ return '['; }
"]"									{ return ']'; }

"="									{ return '='; }
"+="								{ return CShaderParser::token::LSG_SLT_PLUS_EQUALS; }
"-="								{ return CShaderParser::token::LSG_SLT_MINUS_EQUALS; }
"*="								{ return CShaderParser::token::LSG_SLT_TIMES_EQUALS; }
"/="								{ return CShaderParser::token::LSG_SLT_DIV_EQUALS; }
"%="								{ return CShaderParser::token::LSG_SLT_MOD_EQUALS; }

"~"									{ return '~'; }
"&"									{ return '&'; }
"^"									{ return '^'; }
"|"									{ return '|'; }
"<<"								{ return CShaderParser::token::LSG_SLT_SHIFT_LEFT; }
">>"								{ return CShaderParser::token::LSG_SLT_SHIFT_RIGHT; }
"&="								{ return CShaderParser::token::LSG_SLT_AND_EQUALS; }
"^="								{ return CShaderParser::token::LSG_SLT_XOR_EQUALS; }
"|="								{ return CShaderParser::token::LSG_SLT_OR_EQUALS; }
"<<="								{ return CShaderParser::token::LSG_SLT_SHIFT_LEFT_EQUALS; }
">>="								{ return CShaderParser::token::LSG_SLT_SHIFT_RIGHT_EQUALS; }

"&&"								{ return CShaderParser::token::LSG_SLT_AND; }
"^^"								{ return CShaderParser::token::LSG_SLT_XOR; }
"||"								{ return CShaderParser::token::LSG_SLT_OR; }
":"									{ return ':'; }
"?"									{ return '?'; }

","									{ return ','; }

">"									{ return '>'; }
"<"									{ return '<'; }
">="								{ return CShaderParser::token::LSG_SLT_GE; }
"<="								{ return CShaderParser::token::LSG_SLT_LE; }
"=="								{ return CShaderParser::token::LSG_SLT_EQUALS; }
"!="								{ return CShaderParser::token::LSG_SLT_NOT_EQUALS; }

"++"								{ return CShaderParser::token::LSG_SLT_PLUS_PLUS; }
"--"								{ return CShaderParser::token::LSG_SLT_MINUS_MINUS; }

"."									{ return '.'; }

"!"									{ return '!'; }

"{"									{ return '{'; }
"}"									{ return '}'; }
";"									{ return ';'; }

.									{ }
{WS}								{ }


%%


int yyFlexLexer::yywrap() {
	return 1;
}

void yyFlexLexer::set_file( const char * _pcFile ) {
	CString sFile( _pcFile );
	// Get the line number.
	CString sLine;
	while ( CStd::IsDigit( sFile.GetLastChar() ) ) {
		char cChar = sFile.GetLastChar();
		sLine.Insert( 0UL, &cChar, 1UL );
		sFile.RemLastChar();
	}
	while ( sFile.GetLastChar() != '\"' ) {
		sFile.RemLastChar();
	}
	sFile.RemLastChar();	// Eat the trailing ".


	// Skip to the file name.
	while ( sFile.CStr()[0] != '#' ) {
		sFile.RemChar( 0UL );
	}
	while ( sFile.CStr()[0] != ' ' ) {
		sFile.RemChar( 0UL );
	}
	while ( sFile.CStr()[0] != '\"' ) {
		sFile.RemChar( 0UL );
	}
	sFile.RemChar( 0UL );	// Eat the opening ".

	// sFile now has the file name and sLine has the line number.
	curFile = sFile.CStr();
	yylineno = CStd::AtoI32( sLine.CStr() ) - 1UL;
}

void yyFlexLexer::get_line( FLEX_STD string &_sFileName, int &_iLine ) {
	_sFileName = curFile;
	_iLine = yylineno;
}

const FLEX_STD string & yyFlexLexer::get_file() const {
	return curFile;
}