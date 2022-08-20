
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C++ LALR(1) parser skeleton written by Akim Demaille.  */

#ifndef PARSER_HEADER_H
# define PARSER_HEADER_H



#include <string>
#include <iostream>
#include "stack.hh"



namespace yy {

  class position;
  class location;


} // yy


#include "location.hh"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
do {							\
  if (N)						\
    {							\
      (Current).begin = (Rhs)[1].begin;			\
      (Current).end   = (Rhs)[N].end;			\
    }							\
  else							\
    {							\
      (Current).begin = (Current).end = (Rhs)[0].end;	\
    }							\
} while (false)
#endif



namespace yy {


  /// A Bison parser.
  class parser
  {
  public:
    /// Symbol semantic values.
#ifndef YYSTYPE
    typedef YYSTYPE semantic_type;
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;
    /// Tokens.
    struct token
    {
      /* Tokens.  */
   enum yytokentype {
     LSG_SLT_CHAR_CONSTANT = 258,
     LSG_SLT_STRING_CONSTANT = 259,
     LSG_SLT_FLOAT_CONSTANT = 260,
     LSG_SLT_DEC_CONSTANT = 261,
     LSG_SLT_HEX_CONSTANT = 262,
     LSG_SLT_OCT_CONSTANT = 263,
     LSG_SLT_IDENTIFIER = 264,
     LSG_SLT_TYPENAME = 265,
     LSG_SLT_PLUS_EQUALS = 266,
     LSG_SLT_MINUS_EQUALS = 267,
     LSG_SLT_TIMES_EQUALS = 268,
     LSG_SLT_DIV_EQUALS = 269,
     LSG_SLT_MOD_EQUALS = 270,
     LSG_SLT_SHIFT_LEFT = 271,
     LSG_SLT_SHIFT_RIGHT = 272,
     LSG_SLT_AND_EQUALS = 273,
     LSG_SLT_XOR_EQUALS = 274,
     LSG_SLT_OR_EQUALS = 275,
     LSG_SLT_SHIFT_LEFT_EQUALS = 276,
     LSG_SLT_SHIFT_RIGHT_EQUALS = 277,
     LSG_SLT_NOT = 278,
     LSG_SLT_UMINUS = 279,
     LSG_SLT_UPLUS = 280,
     LSG_SLT_AND = 281,
     LSG_SLT_XOR = 282,
     LSG_SLT_OR = 283,
     LSG_SLT_GE = 284,
     LSG_SLT_LE = 285,
     LSG_SLT_EQUALS = 286,
     LSG_SLT_NOT_EQUALS = 287,
     LSG_SLT_PLUS_PLUS = 288,
     LSG_SLT_MINUS_MINUS = 289,
     LSG_SLT_BLENDSTATE = 290,
     LSG_SLT_BOOL = 291,
     LSG_SLT_BREAK = 292,
     LSG_SLT_CONST = 293,
     LSG_SLT_CONTINUE = 294,
     LSG_SLT_DISCARD = 295,
     LSG_SLT_DO = 296,
     LSG_SLT_ELSE = 297,
     LSG_SLT_FALSE = 298,
     LSG_SLT_FLOAT = 299,
     LSG_SLT_FOR = 300,
     LSG_SLT_IF = 301,
     LSG_SLT_IN = 302,
     LSG_SLT_INOUT = 303,
     LSG_SLT_INT = 304,
     LSG_SLT_MATRIX = 305,
     LSG_SLT_NOINTERPOLATION = 306,
     LSG_SLT_OUT = 307,
     LSG_SLT_RETURN = 308,
     LSG_SLT_SAMPLER = 309,
     LSG_SLT_SMOOTH = 310,
     LSG_SLT_STRUCT = 311,
     LSG_SLT_SWITCH = 312,
     LSG_SLT_TEXTURE1D = 313,
     LSG_SLT_TEXTURE2D = 314,
     LSG_SLT_TEXTURE3D = 315,
     LSG_SLT_TEXTURECUBE = 316,
     LSG_SLT_TRUE = 317,
     LSG_SLT_UINT = 318,
     LSG_SLT_UNIFORM = 319,
     LSG_SLT_VEC2 = 320,
     LSG_SLT_VEC3 = 321,
     LSG_SLT_VEC4 = 322,
     LSG_SLT_VECTOR = 323,
     LSG_SLT_VOID = 324,
     LSG_SLT_WHILE = 325,
     LSG_SLT_ABS = 326,
     LSG_SLT_ACOS = 327,
     LSG_SLT_ANY = 328,
     LSG_SLT_ASIN = 329,
     LSG_SLT_ATAN = 330,
     LSG_SLT_ATAN2 = 331,
     LSG_SLT_CEIL = 332,
     LSG_SLT_CLAMP = 333,
     LSG_SLT_COS = 334,
     LSG_SLT_CROSS = 335,
     LSG_SLT_DDX = 336,
     LSG_SLT_DDY = 337,
     LSG_SLT_DEGREES = 338,
     LSG_SLT_DETERMINANT = 339,
     LSG_SLT_DISTANCE = 340,
     LSG_SLT_DOT = 341,
     LSG_SLT_EXP = 342,
     LSG_SLT_EXP2 = 343,
     LSG_SLT_FLOOR = 344,
     LSG_SLT_FRAC = 345,
     LSG_SLT_ISINF = 346,
     LSG_SLT_ISNAN = 347,
     LSG_SLT_LENGTH = 348,
     LSG_SLT_LERP = 349,
     LSG_SLT_LOG = 350,
     LSG_SLT_LOG2 = 351,
     LSG_SLT_MAX = 352,
     LSG_SLT_MIN = 353,
     LSG_SLT_MIX = 354,
     LSG_SLT_MOD = 355,
     LSG_SLT_MUL = 356,
     LSG_SLT_NORMALIZE = 357,
     LSG_SLT_POW = 358,
     LSG_SLT_RADIANS = 359,
     LSG_SLT_RCP = 360,
     LSG_SLT_REFLECT = 361,
     LSG_SLT_REFRACT = 362,
     LSG_SLT_RSQRT = 363,
     LSG_SLT_SATURATE = 364,
     LSG_SLT_SIN = 365,
     LSG_SLT_SINCOS = 366,
     LSG_SLT_SMOOTHSTEP = 367,
     LSG_SLT_SQRT = 368,
     LSG_SLT_STEP = 369,
     LSG_SLT_TAN = 370,
     LSG_SLT_TEX1D = 371,
     LSG_SLT_TEX2D = 372,
     LSG_SLT_TEX2DSHADOW = 373,
     LSG_SLT_TEX2DSHADOWCOMPARE = 374,
     LSG_SLT_TEX3D = 375,
     LSG_SLT_TEXCUBE = 376,
     LSG_SLT_TRANSPOSE = 377,
     LSG_SLT_BINORMAL = 378,
     LSG_SLT_BLENDINDICES = 379,
     LSG_SLT_BLENDWEIGHT = 380,
     LSG_SLT_COLOR = 381,
     LSG_SLT_NORMAL = 382,
     LSG_SLT_POSITION = 383,
     LSG_SLT_POSITIONT = 384,
     LSG_SLT_PSIZE = 385,
     LSG_SLT_TANGENT = 386,
     LSG_SLT_TEXCOORD = 387,
     LSG_SLT_FOG = 388,
     LSG_SLT_TESSFACTOR = 389,
     LSG_SLT_ISFRONTFACE = 390,
     LSG_SLT_VPOS = 391,
     LSG_SLT_DEPTH = 392,
     LSG_SLT_AMBIENTLIGHT = 393,
     LSG_SLT_LIGHTCOUNT = 394,
     LSG_SLT_DIRLIGHTCOUNT = 395,
     LSG_SLT_POINTLIGHTCOUNT = 396,
     LSG_SLT_SPOTLIGHTCOUNT = 397,
     LSG_SLT_DIRLIGHTDIRS = 398,
     LSG_SLT_DIRLIGHTDIFFUSES = 399,
     LSG_SLT_POINTLIGHTPOS = 400,
     LSG_SLT_POINTLIGHTDIFFUSES = 401,
     LSG_SLT_POINTLIGHTSIZES = 402,
     LSG_SLT_SPOTLIGHTDIRS = 403,
     LSG_SLT_LIGHTAMBIENT = 404,
     LSG_SLT_LIGHTDIFFUSE = 405,
     LSG_SLT_LIGHTSPECULAR = 406,
     LSG_SLT_LIGHTATT = 407,
     LSG_SLT_SPOTLIGHTEXP = 408,
     LSG_SLT_SPOTLIGHTRANGE = 409,
     LSG_SLT_SKYCOLOR = 410,
     LSG_SLT_GROUNDCOLOR = 411,
     LSG_SLT_ASHIKFACTORS = 412,
     LSG_SLT_MATAMBIENT = 413,
     LSG_SLT_MATDIFFUSE = 414,
     LSG_SLT_MATEMISSIVE = 415,
     LSG_SLT_MATSPECULAR = 416,
     LSG_SLT_MATPOWER = 417,
     LSG_SLT_REFLECTIVITY = 418,
     LSG_SLT_ROUGHNESS = 419,
     LSG_SLT_FOGSTART = 420,
     LSG_SLT_FOGEND = 421,
     LSG_SLT_FOGCOLOR = 422,
     LSG_SLT_SHADOWMAPPING = 423,
     LSG_SLT_NORMALMATRIX = 424,
     LSG_SLT_TEXMATRIX = 425,
     LSG_SLT_TEXUNIT = 426,
     LSG_SLT_TIMESINCELASTFRAME = 427,
     LSG_SLT_SHADOWMAPUNIT = 428,
     LSG_SLT_SHADOWMAPMATRIX = 429,
     LSG_SLT_SHADOWMAPTEXDIM = 430,
     LSG_SLT_SHADOWMAPUVDEPTHSCALES = 431,
     LSG_SLT_SHADOWMAPCASTERSIZE = 432,
     LSG_SLT_CAMPOS = 433,
     LSG_SLT_VIEW = 434,
     LSG_SLT_WORLD = 435,
     LSG_SLT_WORLDVIEW = 436,
     LSG_SLT_WORLDVIEWPROJ = 437
   };

    };
    /// Token type.
    typedef token::yytokentype token_type;

    /// Build a parser object.
    parser (class CShaderLexer * m_pslLexer_yyarg, class CShaderLanguageParser * m_pslpContainer_yyarg);
    virtual ~parser ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

  private:
    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Generate an error message.
    /// \param state   the state where the error occurred.
    /// \param tok     the lookahead token.
    virtual std::string yysyntax_error_ (int yystate);

#if YYDEBUG
    /// \brief Report a symbol value on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_value_print_ (int yytype,
					 const semantic_type* yyvaluep,
					 const location_type* yylocationp);
    /// \brief Report a symbol on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_print_ (int yytype,
				   const semantic_type* yyvaluep,
				   const location_type* yylocationp);
#endif


    /// State numbers.
    typedef int state_type;
    /// State stack type.
    typedef stack<state_type>    state_stack_type;
    /// Semantic value stack type.
    typedef stack<semantic_type> semantic_stack_type;
    /// location stack type.
    typedef stack<location_type> location_stack_type;

    /// The state stack.
    state_stack_type yystate_stack_;
    /// The semantic value stack.
    semantic_stack_type yysemantic_stack_;
    /// The location stack.
    location_stack_type yylocation_stack_;

    /// Internal symbol numbers.
    typedef unsigned char token_number_type;
    /* Tables.  */
    /// For a state, the index in \a yytable_ of its portion.
    static const short int yypact_[];
    static const short int yypact_ninf_;

    /// For a state, default rule to reduce.
    /// Unless\a  yytable_ specifies something else to do.
    /// Zero means the default is an error.
    static const unsigned short int yydefact_[];

    static const short int yypgoto_[];
    static const short int yydefgoto_[];

    /// What to do in a state.
    /// \a yytable_[yypact_[s]]: what to do in state \a s.
    /// - if positive, shift that token.
    /// - if negative, reduce the rule which number is the opposite.
    /// - if zero, do what YYDEFACT says.
    static const unsigned short int yytable_[];
    static const signed char yytable_ninf_;

    static const short int yycheck_[];

    /// For a state, its accessing symbol.
    static const unsigned short int yystos_[];

    /// For a rule, its LHS.
    static const unsigned short int yyr1_[];
    /// For a rule, its RHS length.
    static const unsigned char yyr2_[];

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#endif

#if YYERROR_VERBOSE
    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    virtual std::string yytnamerr_ (const char *n);
#endif

#if YYDEBUG
    /// A type to store symbol numbers and -1.
    typedef short int rhs_number_type;
    /// A `-1'-separated list of the rules' RHS.
    static const rhs_number_type yyrhs_[];
    /// For each rule, the index of the first RHS symbol in \a yyrhs_.
    static const unsigned short int yyprhs_[];
    /// For each rule, its source line number.
    static const unsigned short int yyrline_[];
    /// For each scanner token number, its symbol number.
    static const unsigned short int yytoken_number_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    /* Debugging.  */
    int yydebug_;
    std::ostream* yycdebug_;
#endif

    /// Convert a scanner token number \a t to a symbol number.
    token_number_type yytranslate_ (int t);

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg        Why this token is reclaimed.
    /// \param yytype       The symbol type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    inline void yydestruct_ (const char* yymsg,
			     int yytype,
			     semantic_type* yyvaluep,
			     location_type* yylocationp);

    /// Pop \a n symbols the three stacks.
    inline void yypop_ (unsigned int n = 1);

    /* Constants.  */
    static const int yyeof_;
    /* LAST_ -- Last index in TABLE_.  */
    static const int yylast_;
    static const int yynnts_;
    static const int yyempty_;
    static const int yyfinal_;
    static const int yyterror_;
    static const int yyerrcode_;
    static const int yyntokens_;
    static const unsigned int yyuser_token_number_max_;
    static const token_number_type yyundef_token_;

    /* User arguments.  */
    class CShaderLexer * m_pslLexer;
    class CShaderLanguageParser * m_pslpContainer;
  };


} // yy




#endif /* ! defined PARSER_HEADER_H */
