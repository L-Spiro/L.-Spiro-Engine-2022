
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
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


/* First part of user declarations.  */




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






#include "LSGShaderParser.hpp"

/* User implementation prologue.  */



#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)



namespace yy {

#if YYERROR_VERBOSE

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              /* Fall through.  */
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

#endif

  /// Build a parser object.
  parser::parser (class CShaderLexer * m_pslLexer_yyarg, class CShaderLanguageParser * m_pslpContainer_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      m_pslLexer (m_pslLexer_yyarg),
      m_pslpContainer (m_pslpContainer_yyarg)
  {
  }

  parser::~parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  int
  parser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    semantic_type yylval;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[2];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yyn == yypact_ninf_)
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, m_pslLexer);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yyn == 0 || yyn == yytable_ninf_)
	goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 2:

    { (yyval.ui32StringIndex) = m_pslpContainer->CreateString( m_pslLexer->YYText() ); }
    break;

  case 3:

    { m_pslpContainer->CreateIdentifier( m_pslLexer->YYText(), (yyval.ndData) ); }
    break;

  case 4:

    {
																		(yyval.ndData).nNodeType = LSG_N_STRINGCONSTANT;
																		(yyval.ndData).nuNodeData.ui32StringIndex = (yysemantic_stack_[(1) - (1)].ui32StringIndex);
																	}
    break;

  case 5:

    { m_pslpContainer->CreateFloatingPoint( m_pslLexer->YYText(), (yyval.ndData) ); }
    break;

  case 6:

    { m_pslpContainer->CreateDecimal( m_pslLexer->YYText(), (yyval.ndData) ); }
    break;

  case 7:

    { m_pslpContainer->CreateBool( false, (yyval.ndData) ); }
    break;

  case 8:

    { m_pslpContainer->CreateBool( true, (yyval.ndData) ); }
    break;

  case 9:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 10:

    { (yyval.ndData) = (yysemantic_stack_[(3) - (2)].ndData); }
    break;

  case 11:

    {
																		if ( !m_pslpContainer->IsInteger( (yysemantic_stack_[(1) - (1)].ndData) ) ) {
																			error( yylloc, "Expression must resolve to an integer value." );
																		}
																		(yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData);
																	}
    break;

  case 12:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 13:

    { m_pslpContainer->CreateArrayAccess( (yysemantic_stack_[(4) - (1)].ndData), (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 14:

    { m_pslpContainer->CreatePostfixOrPrefixIncOrDec( (yysemantic_stack_[(2) - (1)].ndData), LSG_N_POSTFIXINC, (yyval.ndData) ); }
    break;

  case 15:

    { m_pslpContainer->CreatePostfixOrPrefixIncOrDec( (yysemantic_stack_[(2) - (1)].ndData), LSG_N_POSTFIXDEC, (yyval.ndData) ); }
    break;

  case 16:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 17:

    { m_pslpContainer->CreateFunctionCall( (yysemantic_stack_[(3) - (1)].ndData), LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 18:

    { m_pslpContainer->CreateFunctionCall( (yysemantic_stack_[(4) - (1)].ndData), (yysemantic_stack_[(4) - (3)].ndData).ui32NodeIndex, (yyval.ndData) ); }
    break;

  case 19:

    { m_pslpContainer->CreateFieldSelect( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 20:

    { m_pslpContainer->CreateConstructor( (yysemantic_stack_[(3) - (1)].ui64Op), LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 21:

    { m_pslpContainer->CreateConstructor( (yysemantic_stack_[(4) - (1)].ui64Op), (yysemantic_stack_[(4) - (3)].ndData).ui32NodeIndex, (yyval.ndData) ); }
    break;

  case 22:

    { (yyval.ui64Op) = '!'; }
    break;

  case 23:

    { (yyval.ui64Op) = '-'; }
    break;

  case 24:

    { (yyval.ui64Op) = '+'; }
    break;

  case 25:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 26:

    { m_pslpContainer->CreateUnaryOp( static_cast<LSUINT32>((yysemantic_stack_[(2) - (1)].ui64Op)), (yysemantic_stack_[(2) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 27:

    { m_pslpContainer->CreatePostfixOrPrefixIncOrDec( (yysemantic_stack_[(2) - (2)].ndData), LSG_N_PREFIXINC, (yyval.ndData) ); }
    break;

  case 28:

    { m_pslpContainer->CreatePostfixOrPrefixIncOrDec( (yysemantic_stack_[(2) - (2)].ndData), LSG_N_PREFIXDEC, (yyval.ndData) ); }
    break;

  case 29:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 30:

    { m_pslpContainer->CreateCast( (yysemantic_stack_[(4) - (2)].ui64Op), (yysemantic_stack_[(4) - (4)].ndData), (yyval.ndData) ); }
    break;

  case 31:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 32:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '*', (yyval.ndData) ); }
    break;

  case 33:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '/', (yyval.ndData) ); }
    break;

  case 34:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '%', (yyval.ndData) ); }
    break;

  case 35:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 36:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '+', (yyval.ndData) ); }
    break;

  case 37:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '-', (yyval.ndData) ); }
    break;

  case 38:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 39:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::LSG_SLT_SHIFT_LEFT, (yyval.ndData) ); }
    break;

  case 40:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::LSG_SLT_SHIFT_RIGHT, (yyval.ndData) ); }
    break;

  case 41:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 42:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '>', (yyval.ndData) ); }
    break;

  case 43:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '<', (yyval.ndData) ); }
    break;

  case 44:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::LSG_SLT_GE, (yyval.ndData) ); }
    break;

  case 45:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::LSG_SLT_LE, (yyval.ndData) ); }
    break;

  case 46:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 47:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::LSG_SLT_EQUALS, (yyval.ndData) ); }
    break;

  case 48:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::LSG_SLT_NOT_EQUALS, (yyval.ndData) ); }
    break;

  case 49:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 50:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '&', (yyval.ndData) ); }
    break;

  case 51:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 52:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '^', (yyval.ndData) ); }
    break;

  case 53:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 54:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '|', (yyval.ndData) ); }
    break;

  case 55:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 56:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::LSG_SLT_AND, (yyval.ndData) ); }
    break;

  case 57:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 58:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 59:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::LSG_SLT_OR, (yyval.ndData) ); }
    break;

  case 60:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 61:

    { m_pslpContainer->CreateTertiary( (yysemantic_stack_[(5) - (1)].ndData), (yysemantic_stack_[(5) - (3)].ndData), (yysemantic_stack_[(5) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 62:

    { (yyval.ui64Op) = '='; }
    break;

  case 63:

    { (yyval.ui64Op) = token::LSG_SLT_PLUS_EQUALS; }
    break;

  case 64:

    { (yyval.ui64Op) = token::LSG_SLT_MINUS_EQUALS; }
    break;

  case 65:

    { (yyval.ui64Op) = token::LSG_SLT_TIMES_EQUALS; }
    break;

  case 66:

    { (yyval.ui64Op) = token::LSG_SLT_DIV_EQUALS; }
    break;

  case 67:

    { (yyval.ui64Op) = token::LSG_SLT_MOD_EQUALS; }
    break;

  case 68:

    { (yyval.ui64Op) = token::LSG_SLT_SHIFT_LEFT_EQUALS; }
    break;

  case 69:

    { (yyval.ui64Op) = token::LSG_SLT_SHIFT_RIGHT_EQUALS; }
    break;

  case 70:

    { (yyval.ui64Op) = token::LSG_SLT_AND_EQUALS; }
    break;

  case 71:

    { (yyval.ui64Op) = token::LSG_SLT_XOR_EQUALS; }
    break;

  case 72:

    { (yyval.ui64Op) = token::LSG_SLT_OR_EQUALS; }
    break;

  case 73:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 74:

    { m_pslpContainer->CreateAssignment( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), static_cast<LSUINT32>((yysemantic_stack_[(3) - (2)].ui64Op)), (yyval.ndData) ); }
    break;

  case 75:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 76:

    { m_pslpContainer->CreateComma( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 77:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 78:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ABS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 79:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ACOS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 80:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ANY, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 81:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ASIN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 82:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ATAN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 83:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ATAN2, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 84:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_CEIL, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 85:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_CLAMP, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 86:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_COS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 87:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_CROSS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 88:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_DDX, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 89:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_DDY, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 90:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_DEGREES, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 91:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_DETERMINANT, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 92:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_DISTANCE, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 93:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_DOT, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 94:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_EXP, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 95:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_EXP2, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 96:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_FLOOR, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 97:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_FRAC, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 98:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ISINF, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 99:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_ISNAN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 100:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_LENGTH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 101:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_LERP, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 102:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_LOG, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 103:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_LOG2, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 104:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_MAX, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 105:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_MIN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 106:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_MIX, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 107:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_MOD, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 108:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_MUL, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 109:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_NORMALIZE, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 110:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_POW, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 111:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_RADIANS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 112:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_RCP, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 113:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_REFLECT, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 114:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_REFRACT, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 115:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_RSQRT, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 116:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_SATURATE, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 117:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_SIN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 118:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_SINCOS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 119:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_SMOOTHSTEP, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 120:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_SQRT, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 121:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_STEP, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 122:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TAN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 123:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TEX1D, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 124:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TEX2D, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 125:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TEX3D, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 126:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TEXCUBE, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 127:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TEX2DSHADOW, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 128:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TEX2DSHADOWCOMPARE, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 129:

    { m_pslpContainer->CreateIntrinsic( token::LSG_SLT_TRANSPOSE, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 130:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 131:

    { m_pslpContainer->CreateArgExpList( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 132:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_VOID, LSE_MAXU32, LSE_MAXU32 ); }
    break;

  case 133:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_SAMPLER, LSE_MAXU32, LSE_MAXU32 ); }
    break;

  case 134:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_TEXTURE1D, LSE_MAXU32, LSE_MAXU32 ); }
    break;

  case 135:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_TEXTURE2D, LSE_MAXU32, LSE_MAXU32 ); }
    break;

  case 136:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_TEXTURE3D, LSE_MAXU32, LSE_MAXU32 ); }
    break;

  case 137:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_TEXTURECUBE, LSE_MAXU32, LSE_MAXU32 ); }
    break;

  case 138:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ); }
    break;

  case 139:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 ); }
    break;

  case 140:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_UINT, LSE_MAXU32, LSE_MAXU32 ); }
    break;

  case 141:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_BOOL, LSE_MAXU32, LSE_MAXU32 ); }
    break;

  case 142:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, 2UL, LSE_MAXU32 ); }
    break;

  case 143:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, 3UL, LSE_MAXU32 ); }
    break;

  case 144:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, 4UL, LSE_MAXU32 ); }
    break;

  case 145:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, 4UL, LSE_MAXU32 ); }
    break;

  case 146:

    { (yyval.ui64Op) = CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, 4UL, 4UL ); }
    break;

  case 147:

    {
																		CShaderLanguageParser::LSG_SCALAR sValue;
																		m_pslpContainer->GetNodeValue( (yysemantic_stack_[(4) - (3)].ndData), sValue );
																		if ( sValue.ui64Value < 2ULL || sValue.ui64Value > 4ULL ) {
																			error( yyloc, "Array constant must be 2, 3, or 4." );
																		}
																		(yyval.ui64Op) = m_pslpContainer->CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, static_cast<LSUINT32>(sValue.ui64Value), LSE_MAXU32 );
																	}
    break;

  case 148:

    {
																		CShaderLanguageParser::LSG_SCALAR sValue;
																		m_pslpContainer->GetNodeValue( (yysemantic_stack_[(4) - (3)].ndData), sValue );
																		if ( sValue.ui64Value < 2ULL || sValue.ui64Value > 4ULL ) {
																			error( yyloc, "Array constant must be 2, 3, or 4." );
																		}
																		(yyval.ui64Op) = m_pslpContainer->CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_INT, static_cast<LSUINT32>(sValue.ui64Value), LSE_MAXU32 );
																	}
    break;

  case 149:

    {
																		CShaderLanguageParser::LSG_SCALAR sValue;
																		m_pslpContainer->GetNodeValue( (yysemantic_stack_[(4) - (3)].ndData), sValue );
																		if ( sValue.ui64Value < 2ULL || sValue.ui64Value > 4ULL ) {
																			error( yyloc, "Array constant must be 2, 3, or 4." );
																		}
																		(yyval.ui64Op) = m_pslpContainer->CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_UINT, static_cast<LSUINT32>(sValue.ui64Value), LSE_MAXU32 );
																	}
    break;

  case 150:

    {
																		CShaderLanguageParser::LSG_SCALAR sValue;
																		m_pslpContainer->GetNodeValue( (yysemantic_stack_[(4) - (3)].ndData), sValue );
																		if ( sValue.ui64Value < 2ULL || sValue.ui64Value > 4ULL ) {
																			error( yyloc, "Array constant must be 2, 3, or 4." );
																		}
																		(yyval.ui64Op) = m_pslpContainer->CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_BOOL, static_cast<LSUINT32>(sValue.ui64Value), LSE_MAXU32 );
																	}
    break;

  case 151:

    {
																		CShaderLanguageParser::LSG_SCALAR sLeft;
																		m_pslpContainer->GetNodeValue( (yysemantic_stack_[(7) - (3)].ndData), sLeft );
																		if ( sLeft.ui64Value < 2ULL || sLeft.ui64Value > 4ULL ) {
																			error( yyloc, "Array constant must be 2, 3, or 4." );
																		}
																		CShaderLanguageParser::LSG_SCALAR sRight;
																		m_pslpContainer->GetNodeValue( (yysemantic_stack_[(7) - (6)].ndData), sRight );
																		if ( sRight.ui64Value < 2ULL || sRight.ui64Value > 4ULL ) {
																			error( yyloc, "Array constant must be 2, 3, or 4." );
																		}
																		(yyval.ui64Op) = m_pslpContainer->CShaderLanguageParser::CombineTypesAndArrays( token::LSG_SLT_FLOAT, static_cast<LSUINT32>(sLeft.ui64Value), static_cast<LSUINT32>(sRight.ui64Value) );
																	}
    break;

  case 152:

    { (yyval.ui64Op) = m_pslpContainer->CShaderLanguageParser::CombineTypesAndArrays( m_pslpContainer->GetStructTypeId( m_pslLexer->YYText() ), LSE_MAXU32, LSE_MAXU32 ); }
    break;

  case 153:

    { m_pslpContainer->CreateTypeSpecifier( (yysemantic_stack_[(1) - (1)].ui64Op), (yyval.ndData) ); }
    break;

  case 154:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 155:

    { (yyval.ui64Op) = LSG_SQ_CONST; }
    break;

  case 156:

    { (yyval.ui64Op) = LSG_SQ_IN; }
    break;

  case 157:

    { (yyval.ui64Op) = LSG_SQ_OUT; }
    break;

  case 158:

    { (yyval.ui64Op) = LSG_SQ_UNIFORM; }
    break;

  case 159:

    { (yyval.ui64Op) = LSG_IQ_FLAT; }
    break;

  case 160:

    { (yyval.ui64Op) = (yysemantic_stack_[(1) - (1)].ui64Op); }
    break;

  case 161:

    { (yyval.ui64Op) = (yysemantic_stack_[(2) - (1)].ui64Op) | (yysemantic_stack_[(2) - (2)].ui64Op); }
    break;

  case 162:

    { m_pslpContainer->CreateFullySpecifiedType( 0ULL, (yysemantic_stack_[(1) - (1)].ndData), (yyval.ndData) ); }
    break;

  case 163:

    { m_pslpContainer->CreateFullySpecifiedType( (yysemantic_stack_[(2) - (1)].ui64Op), (yysemantic_stack_[(2) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 164:

    { m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(2) - (2)].ndData).ui32NodeIndex, (yysemantic_stack_[(2) - (1)].ndData), LSE_MAXU32, LSE_MAXU32, LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 165:

    { m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(5) - (2)].ndData).ui32NodeIndex, (yysemantic_stack_[(5) - (1)].ndData), (yysemantic_stack_[(5) - (4)].ndData).ui32NodeIndex, LSE_MAXU32, LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 166:

    { m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(4) - (2)].ndData).ui32NodeIndex, (yysemantic_stack_[(4) - (1)].ndData), LSE_MAXU32, LSE_MAXU32, (yysemantic_stack_[(4) - (4)].ndData).ui32NodeIndex, (yyval.ndData) ); }
    break;

  case 167:

    { m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(7) - (2)].ndData).ui32NodeIndex, (yysemantic_stack_[(7) - (1)].ndData), (yysemantic_stack_[(7) - (4)].ndData).ui32NodeIndex, LSE_MAXU32, (yysemantic_stack_[(7) - (7)].ndData).ui32NodeIndex, (yyval.ndData) ); }
    break;

  case 168:

    { m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(4) - (2)].ndData).ui32NodeIndex, (yysemantic_stack_[(4) - (1)].ndData), LSE_MAXU32, (yysemantic_stack_[(4) - (4)].ndData).ui32NodeIndex, LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 169:

    { m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(7) - (2)].ndData).ui32NodeIndex, (yysemantic_stack_[(7) - (1)].ndData), (yysemantic_stack_[(7) - (4)].ndData).ui32NodeIndex, (yysemantic_stack_[(7) - (7)].ndData).ui32NodeIndex, LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 170:

    { m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(6) - (2)].ndData).ui32NodeIndex, (yysemantic_stack_[(6) - (1)].ndData), LSE_MAXU32, (yysemantic_stack_[(6) - (4)].ndData).ui32NodeIndex, (yysemantic_stack_[(6) - (6)].ndData).ui32NodeIndex, (yyval.ndData) ); }
    break;

  case 171:

    { m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(9) - (2)].ndData).ui32NodeIndex, (yysemantic_stack_[(9) - (1)].ndData), (yysemantic_stack_[(9) - (4)].ndData).ui32NodeIndex, (yysemantic_stack_[(9) - (7)].ndData).ui32NodeIndex, (yysemantic_stack_[(9) - (9)].ndData).ui32NodeIndex, (yyval.ndData) ); }
    break;

  case 172:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 173:

    {
																		m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(3) - (3)].ndData).ui32NodeIndex,
																			m_pslpContainer->GetNode( (yysemantic_stack_[(3) - (1)].ndData).nuNodeData.ui32NodeIndex ), LSE_MAXU32, LSE_MAXU32, LSE_MAXU32, (yyval.ndData) );
																	}
    break;

  case 174:

    {
																		m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(6) - (3)].ndData).ui32NodeIndex,
																			m_pslpContainer->GetNode( (yysemantic_stack_[(6) - (1)].ndData).nuNodeData.ui32NodeIndex ), (yysemantic_stack_[(6) - (5)].ndData).ui32NodeIndex, LSE_MAXU32, LSE_MAXU32, (yyval.ndData) );
																	}
    break;

  case 175:

    {
																		m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(5) - (3)].ndData).ui32NodeIndex,
																			m_pslpContainer->GetNode( (yysemantic_stack_[(5) - (1)].ndData).nuNodeData.ui32NodeIndex ), LSE_MAXU32, LSE_MAXU32, (yysemantic_stack_[(5) - (5)].ndData).ui32NodeIndex, (yyval.ndData) );
																	}
    break;

  case 176:

    {
																		m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(5) - (3)].ndData).ui32NodeIndex,
																			m_pslpContainer->GetNode( (yysemantic_stack_[(5) - (1)].ndData).nuNodeData.ui32NodeIndex ), LSE_MAXU32, (yysemantic_stack_[(5) - (5)].ndData).ui32NodeIndex, LSE_MAXU32, (yyval.ndData) );
																	}
    break;

  case 177:

    {
																		m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(8) - (3)].ndData).ui32NodeIndex,
																			m_pslpContainer->GetNode( (yysemantic_stack_[(8) - (1)].ndData).nuNodeData.ui32NodeIndex ), (yysemantic_stack_[(8) - (5)].ndData).ui32NodeIndex, (yysemantic_stack_[(8) - (8)].ndData).ui32NodeIndex, LSE_MAXU32, (yyval.ndData) );
																	}
    break;

  case 178:

    {
																		m_pslpContainer->CreateSingleDeclaration( (yysemantic_stack_[(7) - (3)].ndData).ui32NodeIndex,
																			m_pslpContainer->GetNode( (yysemantic_stack_[(7) - (1)].ndData).nuNodeData.ui32NodeIndex ), LSE_MAXU32, (yysemantic_stack_[(7) - (5)].ndData).ui32NodeIndex, (yysemantic_stack_[(7) - (7)].ndData).ui32NodeIndex, (yyval.ndData) );
																	}
    break;

  case 179:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 180:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 181:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 182:

    { m_pslpContainer->CreateDeclarationStatement( (yysemantic_stack_[(2) - (1)].ndData), (yyval.ndData) ); }
    break;

  case 183:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_BINORMAL, (yyval.ndData) ); }
    break;

  case 184:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_BLENDINDICES, (yyval.ndData) ); }
    break;

  case 185:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_BLENDWEIGHT, (yyval.ndData) ); }
    break;

  case 186:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_COLOR, (yyval.ndData) ); }
    break;

  case 187:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_NORMAL, (yyval.ndData) ); }
    break;

  case 188:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_POSITION, (yyval.ndData) ); }
    break;

  case 189:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_POSITIONT, (yyval.ndData) ); }
    break;

  case 190:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_PSIZE, (yyval.ndData) ); }
    break;

  case 191:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_TANGENT, (yyval.ndData) ); }
    break;

  case 192:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_TEXCOORD, (yyval.ndData) ); }
    break;

  case 193:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_FOG, (yyval.ndData) ); }
    break;

  case 194:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_TESSFACTOR, (yyval.ndData) ); }
    break;

  case 195:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_ISFRONTFACE, (yyval.ndData) ); }
    break;

  case 196:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_VPOS, (yyval.ndData) ); }
    break;

  case 197:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_DEPTH, (yyval.ndData) ); }
    break;

  case 198:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_AMBIENTLIGHT, (yyval.ndData) ); }
    break;

  case 199:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_LIGHTCOUNT, (yyval.ndData) ); }
    break;

  case 200:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_DIRLIGHTCOUNT, (yyval.ndData) ); }
    break;

  case 201:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_POINTLIGHTCOUNT, (yyval.ndData) ); }
    break;

  case 202:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SPOTLIGHTCOUNT, (yyval.ndData) ); }
    break;

  case 203:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_DIRLIGHTDIRS, (yyval.ndData) ); }
    break;

  case 204:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_DIRLIGHTDIFFUSES, (yyval.ndData) ); }
    break;

  case 205:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_POINTLIGHTPOS, (yyval.ndData) ); }
    break;

  case 206:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_POINTLIGHTDIFFUSES, (yyval.ndData) ); }
    break;

  case 207:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_POINTLIGHTSIZES, (yyval.ndData) ); }
    break;

  case 208:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SPOTLIGHTDIRS, (yyval.ndData) ); }
    break;

  case 209:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_LIGHTAMBIENT, (yyval.ndData) ); }
    break;

  case 210:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_LIGHTDIFFUSE, (yyval.ndData) ); }
    break;

  case 211:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_LIGHTSPECULAR, (yyval.ndData) ); }
    break;

  case 212:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_LIGHTATT, (yyval.ndData) ); }
    break;

  case 213:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SPOTLIGHTRANGE, (yyval.ndData) ); }
    break;

  case 214:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SKYCOLOR, (yyval.ndData) ); }
    break;

  case 215:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_GROUNDCOLOR, (yyval.ndData) ); }
    break;

  case 216:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_MATAMBIENT, (yyval.ndData) ); }
    break;

  case 217:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_MATDIFFUSE, (yyval.ndData) ); }
    break;

  case 218:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_MATEMISSIVE, (yyval.ndData) ); }
    break;

  case 219:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_MATSPECULAR, (yyval.ndData) ); }
    break;

  case 220:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_MATPOWER, (yyval.ndData) ); }
    break;

  case 221:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_REFLECTIVITY, (yyval.ndData) ); }
    break;

  case 222:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_ROUGHNESS, (yyval.ndData) ); }
    break;

  case 223:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_ASHIKFACTORS, (yyval.ndData) ); }
    break;

  case 224:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_FOGSTART, (yyval.ndData) ); }
    break;

  case 225:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_FOGEND, (yyval.ndData) ); }
    break;

  case 226:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_FOGCOLOR, (yyval.ndData) ); }
    break;

  case 227:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SHADOWMAPPING, (yyval.ndData) ); }
    break;

  case 228:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_NORMALMATRIX, (yyval.ndData) ); }
    break;

  case 229:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_TEXMATRIX, (yyval.ndData) ); }
    break;

  case 230:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_TEXUNIT, (yyval.ndData) ); }
    break;

  case 231:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_TIMESINCELASTFRAME, (yyval.ndData) ); }
    break;

  case 232:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SHADOWMAPUNIT, (yyval.ndData) ); }
    break;

  case 233:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SHADOWMAPMATRIX, (yyval.ndData) ); }
    break;

  case 234:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SHADOWMAPUVDEPTHSCALES, (yyval.ndData) ); }
    break;

  case 235:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SHADOWMAPCASTERSIZE, (yyval.ndData) ); }
    break;

  case 236:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_SHADOWMAPTEXDIM, (yyval.ndData) ); }
    break;

  case 237:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_CAMPOS, (yyval.ndData) ); }
    break;

  case 238:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_VIEW, (yyval.ndData) ); }
    break;

  case 239:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_WORLD, (yyval.ndData) ); }
    break;

  case 240:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_WORLDVIEW, (yyval.ndData) ); }
    break;

  case 241:

    { m_pslpContainer->CreateSemantic( m_pslLexer->YYText(), token::LSG_SLT_WORLDVIEWPROJ, (yyval.ndData) ); }
    break;

  case 242:

    { (yyval.ui64Op) = LSG_SQ_IN; }
    break;

  case 243:

    { (yyval.ui64Op) = LSG_SQ_OUT; }
    break;

  case 244:

    { (yyval.ui64Op) = LSG_SQ_INOUT; }
    break;

  case 245:

    { (yyval.ui64Op) = LSG_SQ_CONST; }
    break;

  case 246:

    { m_pslpContainer->CreateParameterDeclarator( (yysemantic_stack_[(2) - (1)].ndData), (yysemantic_stack_[(2) - (2)].ndData), LSE_MAXU32, LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 247:

    { m_pslpContainer->CreateParameterDeclarator( (yysemantic_stack_[(5) - (1)].ndData), (yysemantic_stack_[(5) - (2)].ndData), (yysemantic_stack_[(5) - (4)].ndData).ui32NodeIndex, LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 248:

    { m_pslpContainer->CreateParameterDeclarator( (yysemantic_stack_[(4) - (1)].ndData), (yysemantic_stack_[(4) - (2)].ndData), LSE_MAXU32, (yysemantic_stack_[(4) - (4)].ndData).ui32NodeIndex, (yyval.ndData) ); }
    break;

  case 249:

    { m_pslpContainer->CreateParameterDeclaration( (yysemantic_stack_[(3) - (1)].ui64Op) | (yysemantic_stack_[(3) - (2)].ui64Op), (yysemantic_stack_[(3) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 250:

    { m_pslpContainer->CreateParameterDeclaration( (yysemantic_stack_[(2) - (1)].ui64Op), (yysemantic_stack_[(2) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 251:

    { m_pslpContainer->CreateParameterDeclaration( (yysemantic_stack_[(2) - (1)].ui64Op), (yysemantic_stack_[(2) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 252:

    { m_pslpContainer->CreateParameterDeclaration( 0UL, (yysemantic_stack_[(1) - (1)].ndData), (yyval.ndData) ); }
    break;

  case 253:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 254:

    { m_pslpContainer->CreateParameterDeclarationList( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 255:

    { m_pslpContainer->CreateFunctionDeclaration( (yysemantic_stack_[(5) - (1)].ndData), (yysemantic_stack_[(5) - (2)].ndData), (yysemantic_stack_[(5) - (4)].ndData).ui32NodeIndex, (yyval.ndData) ); }
    break;

  case 256:

    { m_pslpContainer->CreateFunctionDeclaration( (yysemantic_stack_[(5) - (1)].ndData), (yysemantic_stack_[(5) - (2)].ndData), LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 257:

    { m_pslpContainer->CreateFunctionDeclaration( (yysemantic_stack_[(4) - (1)].ndData), (yysemantic_stack_[(4) - (2)].ndData), LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 258:

    { m_pslpContainer->CreateFunctionDefinition( (yysemantic_stack_[(2) - (1)].ndData), (yysemantic_stack_[(2) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 259:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 260:

    { m_pslpContainer->CreateInitializerListWrap( (yysemantic_stack_[(3) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 261:

    { m_pslpContainer->CreateInitializerListWrap( (yysemantic_stack_[(4) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 262:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 263:

    { m_pslpContainer->CreateInitializerList( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 264:

    { m_pslpContainer->CreateStructDeclarator( (yysemantic_stack_[(1) - (1)].ndData), LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 265:

    { m_pslpContainer->CreateStructDeclarator( (yysemantic_stack_[(4) - (1)].ndData), (yysemantic_stack_[(4) - (3)].ndData).ui32NodeIndex, (yyval.ndData) ); }
    break;

  case 266:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 267:

    { m_pslpContainer->CreateStructDeclaratorList( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 268:

    { m_pslpContainer->CreateStructDeclaration( 0UL, (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 269:

    { m_pslpContainer->CreateStructDeclaration( (yysemantic_stack_[(4) - (1)].ui64Op), (yysemantic_stack_[(4) - (2)].ndData), (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 270:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 271:

    { m_pslpContainer->CreateStructDeclarationList( (yysemantic_stack_[(2) - (1)].ndData), (yysemantic_stack_[(2) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 272:

    { m_pslpContainer->CreateStruct( (yysemantic_stack_[(5) - (2)].ndData).ui32NodeIndex, (yysemantic_stack_[(5) - (4)].ndData), (yyval.ndData) ); }
    break;

  case 273:

    { m_pslpContainer->CreateStruct( LSE_MAXU32, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 274:

    { m_pslpContainer->CreateSelectionStatement( (yysemantic_stack_[(5) - (3)].ndData), (yysemantic_stack_[(5) - (5)].ndData), LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 275:

    { m_pslpContainer->CreateSelectionStatement( (yysemantic_stack_[(7) - (3)].ndData), (yysemantic_stack_[(7) - (5)].ndData), (yysemantic_stack_[(7) - (7)].ndData).ui32NodeIndex, (yyval.ndData) ); }
    break;

  case 276:

    { m_pslpContainer->CreateForInitDeclStatement( (yysemantic_stack_[(2) - (1)].ndData), (yyval.ndData) ); }
    break;

  case 277:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 278:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 279:

    { m_pslpContainer->CreateWhileLoop( (yysemantic_stack_[(5) - (3)].ndData), (yysemantic_stack_[(5) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 280:

    { m_pslpContainer->CreateDoWhileLoop( (yysemantic_stack_[(7) - (5)].ndData), (yysemantic_stack_[(7) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 281:

    { m_pslpContainer->CreateForLoop( (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (4)].ndData), LSE_MAXU32, (yysemantic_stack_[(6) - (6)].ndData), (yyval.ndData) ); }
    break;

  case 282:

    { m_pslpContainer->CreateForLoop( (yysemantic_stack_[(7) - (3)].ndData), (yysemantic_stack_[(7) - (4)].ndData), (yysemantic_stack_[(7) - (5)].ndData).ui32NodeIndex, (yysemantic_stack_[(7) - (7)].ndData), (yyval.ndData) ); }
    break;

  case 283:

    { m_pslpContainer->CreateJump( LSG_N_CONTINUE, LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 284:

    { m_pslpContainer->CreateJump( LSG_N_BREAK, LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 285:

    { m_pslpContainer->CreateJump( LSG_N_RETURN, LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 286:

    { m_pslpContainer->CreateJump( LSG_N_RETURN, (yysemantic_stack_[(3) - (2)].ndData).ui32NodeIndex, (yyval.ndData) ); }
    break;

  case 287:

    { m_pslpContainer->CreateJump( LSG_N_DISCARD, LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 288:

    { m_pslpContainer->CreateExpressionStatement( LSE_MAXU32, (yyval.ndData) ); }
    break;

  case 289:

    { m_pslpContainer->CreateExpressionStatement( (yysemantic_stack_[(2) - (1)].ndData).ui32NodeIndex, (yyval.ndData) ); }
    break;

  case 290:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 291:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 292:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 293:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 294:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 295:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 296:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 297:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 298:

    { m_pslpContainer->CreateStatementList( (yysemantic_stack_[(2) - (1)].ndData), (yysemantic_stack_[(2) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 299:

    { m_pslpContainer->CreateBlankStatement( (yyval.ndData) ); }
    break;

  case 300:

    { m_pslpContainer->CreateNewScopedStatement( (yysemantic_stack_[(3) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 301:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 302:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 303:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 304:

    { m_pslpContainer->CreateExternalDeclarationList( (yysemantic_stack_[(2) - (1)].ndData), (yysemantic_stack_[(2) - (2)].ndData), (yyval.ndData) ); }
    break;



	default:
          break;
      }
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	error (yylloc, yysyntax_error_ (yystate));
      }

    yyerror_range[0] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[0] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (yyn != yypact_ninf_)
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[0] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[1] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (int yystate)
  {
    std::string res;
    YYUSE (yystate);
#if YYERROR_VERBOSE
    int yyn = yypact_[yystate];
    if (yypact_ninf_ < yyn && yyn <= yylast_)
      {
	/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
	int yyxbegin = yyn < 0 ? -yyn : 0;

	/* Stay within bounds of both yycheck and yytname.  */
	int yychecklim = yylast_ - yyn + 1;
	int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
	int count = 0;
	for (int x = yyxbegin; x < yyxend; ++x)
	  if (yycheck_[x + yyn] == x && x != yyterror_)
	    ++count;

	// FIXME: This method of building the message is not compatible
	// with internationalization.  It should work like yacc.c does it.
	// That is, first build a string that looks like this:
	// "syntax error, unexpected %s or %s or %s"
	// Then, invoke YY_ on this string.
	// Finally, use the string as a format to output
	// yytname_[tok], etc.
	// Until this gets fixed, this message appears in English only.
	res = "syntax error, unexpected ";
	res += yytnamerr_ (yytname_[tok]);
	if (count < 5)
	  {
	    count = 0;
	    for (int x = yyxbegin; x < yyxend; ++x)
	      if (yycheck_[x + yyn] == x && x != yyterror_)
		{
		  res += (!count++) ? ", expecting " : " or ";
		  res += yytnamerr_ (yytname_[x]);
		}
	  }
      }
    else
#endif
      res = YY_("syntax error");
    return res;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const short int parser::yypact_ninf_ = -359;
  const short int
  parser::yypact_[] =
  {
      2730,  -359,  -166,  -359,  -149,  -359,  -146,  -359,  -359,  -359,
    -359,     6,  -359,  -359,  -359,  -359,  -121,  -359,  -359,  -359,
    -359,  -359,  -359,  -359,  -359,  -359,  -359,    67,  1317,    68,
    -359,  -114,  -109,  -359,  -102,  -359,  -359,  -359,   439,  2229,
    2229,  2229,  -359,  2765,   -97,  2229,  -359,  -359,  -139,    68,
    -359,   558,  -359,  -359,  -359,  -359,  -359,  -359,  2371,  2371,
    -359,  -359,   -77,   -56,    -3,    10,    12,    18,    53,    59,
      96,   111,   131,   196,   201,   202,   204,   205,   206,   207,
     208,   209,   210,   212,   213,   214,   215,   216,   219,   220,
     221,   222,   223,   224,   225,   226,   228,   229,   230,   231,
     233,   235,   236,   237,   239,   240,   246,   247,   248,   249,
     250,   252,   253,   254,  -359,  -359,  2229,  -359,  -359,  -359,
    -359,   122,    15,  2371,   114,  -359,   -51,  -123,   166,     2,
     165,   102,   162,    42,   302,  -359,    -6,  -359,  -359,   157,
    -359,   256,   255,   257,    68,  1317,  -359,    32,  2765,   258,
    1200,  2484,  2229,  2557,  -143,   259,   260,   261,  1052,   262,
     263,  1507,   264,  -359,  -359,   -81,   256,  -359,  -359,  -359,
    -359,  -359,  -359,  -359,  -359,   731,  -359,  2229,  -359,  -359,
    2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,
    2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,
    2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,
    2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,
    2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,
    2229,  2229,  -130,    81,  -359,  -359,  -359,  1803,  2229,    68,
    -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,
    -359,  -359,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,
    2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,  2229,
    2229,  2229,  2229,  1945,   269,  -359,   271,  -359,   155,    68,
    -359,  -359,   316,  -359,  1200,  -359,  -359,  -359,  -359,  -359,
    -359,   265,  -359,    68,  1317,  2835,  -359,  -359,  -115,  -359,
    -359,   272,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,
    -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,
    -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,
    -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,
    -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,
    -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,
    -359,   270,  1200,  2229,  2557,  -359,  -359,  -359,   389,  1359,
    2229,  -359,   159,  2229,  -359,  -359,  -359,  -359,   -92,   -91,
     -89,   -87,   -43,   -40,   -34,   -32,   -30,   -29,   -28,   -26,
     -24,   -18,   -16,   -15,   -14,   -10,     3,    13,    23,    25,
      26,    27,    28,    29,    37,    40,    41,    43,    46,    77,
      78,    79,    90,    91,    94,   104,   105,   106,   107,   115,
     116,   119,   120,   121,   125,   134,   135,   138,   139,   143,
    -359,  2229,  -359,   148,   274,  -359,  -359,  -359,  -359,  -359,
     -51,   -51,  -123,  -123,   166,   166,   166,   166,     2,     2,
     165,   102,   162,    42,  -359,   171,  -359,  -359,   149,  2229,
    2229,    68,  -359,   184,  -359,  -359,  -141,  -359,  -155,  -359,
    1317,  -359,  -359,  2800,  -138,  1200,  -359,   275,   285,   280,
      68,   273,  -359,  1655,  -359,   151,  -359,   153,  -359,  2229,
    -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,
    -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,
    -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,
    -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,
    -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,  -359,
    -359,  -359,  -359,  -359,  2229,  -359,   281,   283,  -359,  -359,
     879,  -359,  2229,  2557,  -359,  -359,  1200,  2557,  -359,   286,
    1200,  2229,  -127,  -359,  2087,  1052,  1052,  -359,  -359,  -359,
    -359,  -359,  -359,   284,  -359,  -359,   301,  2557,  -359,   154,
    1052,   176,   450,  -359,  -359,  1200,  -359,   291,  -359,  1052,
    1052,  -359,  -359,  -359,  -359
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned short int
  parser::yydefact_[] =
  {
         0,   152,   141,   155,   138,   156,   139,   146,   159,   157,
     133,     0,   134,   135,   136,   137,   140,   158,   142,   143,
     144,   145,   132,   153,   154,   162,   160,     0,     0,     0,
     172,   179,     0,   302,   181,   301,   180,   303,     0,     0,
       0,     0,     3,     0,     0,     0,   161,   163,   164,     0,
     182,     0,   258,     1,   304,     2,     5,     6,     0,     0,
       7,     8,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    24,    23,     0,    22,     4,     9,
      12,     0,    25,     0,    29,    31,    35,    38,    41,    46,
      49,    51,    53,    55,    57,    58,    60,    73,    75,    11,
      16,     0,     0,     0,     0,     0,   270,     0,     0,     0,
       0,     0,     0,     0,   173,     0,     0,     0,     0,     0,
       0,     0,     0,   288,   299,     0,   153,   291,   181,   292,
     293,   294,   290,   295,   297,     0,   296,     0,    27,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   150,    14,    15,     0,     0,     0,
      26,    63,    64,    65,    66,    67,    70,    71,    72,    68,
      69,    62,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   147,   148,   264,   266,     0,     0,
     273,   271,     0,   149,     0,   259,   166,   245,   242,   244,
     243,   132,   257,     0,     0,     0,   252,   253,     0,    29,
      77,     0,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   223,   216,   217,   218,   219,
     220,   221,   222,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   236,   234,   235,   237,   238,   239,   240,
     241,   168,     0,     0,     0,   284,   283,   287,     0,     0,
       0,   285,     0,     0,   289,   300,   298,   130,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      10,     0,    17,     0,     0,    19,    74,    32,    33,    34,
      36,    37,    39,    40,    44,    45,    42,    43,    47,    48,
      50,    52,    54,    56,    59,     0,    76,    20,     0,     0,
       0,     0,   268,     0,   272,   262,     0,   256,   246,   250,
       0,   251,   255,     0,   165,     0,   175,     0,   176,     0,
       0,     0,   278,     0,   277,     0,   286,     0,    78,     0,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   127,   128,   125,   126,
     129,    30,    18,    13,     0,    21,     0,     0,   267,   269,
       0,   260,     0,     0,   249,   254,     0,     0,   170,   174,
       0,     0,   164,   276,     0,     0,     0,   131,    61,   151,
     265,   261,   263,     0,   248,   167,   169,     0,   178,     0,
       0,     0,   274,   279,   247,     0,   177,     0,   281,     0,
       0,   171,   280,   282,   275
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  parser::yypgoto_[] =
  {
      -359,  -359,     5,  -359,   -21,  -359,  -359,     1,  -228,   132,
     133,   -11,   129,   243,   234,   242,   227,  -359,   241,  -359,
    -147,  -359,   -38,   -31,  -351,  -359,  2390,     0,  -359,     9,
    -359,  -359,    -4,   144,   145,  -359,  -359,    33,  -358,   244,
    -359,  -277,    44,  -359,    35,  -359,  -273,  -359,    54,   245,
    -118,   368,  -359,  -359,  -359,  -359,  -359,  -359,  -333,  -359,
    -154,  -359,   485,   482,  -359
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  parser::yydefgoto_[] =
  {
        -1,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   252,   377,   165,   301,   140,   378,   141,    24,    25,
      26,    27,    28,    29,    30,    31,    32,   167,   361,   294,
     295,   296,   297,   298,   168,    35,   286,   466,   277,   278,
     146,   147,    36,   169,   482,   483,   170,   171,   172,   173,
     174,   175,   176,    37,    38
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char parser::yytable_ninf_ = -1;
  const unsigned short int
  parser::yytable_[] =
  {
        23,   138,   138,   138,   368,   300,   478,   138,   139,   139,
     139,   465,   477,   138,   139,    42,    44,   469,   471,   142,
     143,   376,   270,    46,   149,   437,   438,   439,    23,   281,
      39,   260,   261,    33,    48,    34,   484,    47,    23,   145,
     362,   552,     1,    23,   150,   556,   553,    40,   235,   236,
      41,   166,   144,   363,   154,   151,   150,   152,   364,   178,
     179,   550,   153,   557,   551,   430,   256,   257,     2,   152,
       3,    33,   272,    34,   153,    45,     4,    42,   138,     5,
     472,     6,     7,     8,     9,   232,    10,   473,    49,   476,
      12,    13,    14,    15,    50,    16,    17,    18,    19,    20,
      21,    22,    51,   488,   490,     3,   491,   148,   492,   547,
     489,   489,   285,   489,     5,   489,   233,   180,     8,     9,
     138,   272,   374,   138,   240,   241,   242,   243,   244,   245,
     372,    17,   246,   247,   248,   249,   250,   138,   181,   138,
     253,   254,   255,   145,   145,    23,   232,    23,    23,   276,
     564,    23,   493,   299,   279,   494,   144,   144,   166,   489,
     293,   495,   489,   496,   281,   497,   498,   499,   489,   500,
     489,   501,   489,   489,   489,   166,   489,   502,   489,   503,
     504,   505,   258,   259,   489,   506,   489,   489,   489,   262,
     263,   182,   489,   554,   271,   574,   264,   265,   507,   576,
     138,   573,   558,   541,   183,   489,   184,   139,   508,   237,
      43,   238,   185,   239,   436,   489,   300,   434,   509,   586,
     510,   511,   512,   513,   514,   489,   268,   489,   489,   489,
     489,   489,   515,   138,   456,   516,   517,   280,   518,   489,
     455,   519,   489,   489,   435,   489,   285,   186,   489,   444,
     445,   446,   447,   187,   299,   299,   299,   299,   299,   299,
     299,   299,   299,   299,   299,   299,   299,   299,   299,   299,
     299,   299,   520,   521,   522,   273,   431,   572,   145,   489,
     489,   489,    23,   575,   276,   523,   524,   578,   266,   525,
     188,   144,   489,   489,    23,    23,   489,   251,   468,   526,
     527,   528,   529,   293,   293,   189,   489,   489,   489,   489,
     530,   531,   591,   300,   532,   533,   534,   489,   489,   234,
     535,   489,   489,   489,   285,   190,     1,   489,   269,   536,
     537,   138,   138,   538,   539,   138,   489,   489,   540,   485,
     489,   489,   487,   542,   545,   489,   565,   267,   566,   587,
     489,   489,     2,   272,     3,   272,   272,   461,   462,   272,
       4,   272,   486,     5,   299,     6,     7,     8,     9,   166,
      10,   589,   544,   272,    12,    13,    14,    15,   272,    16,
      17,    18,    19,    20,    21,    22,   461,   549,   440,   441,
     191,   442,   443,   448,   449,   192,   193,   568,   194,   195,
     196,   197,   198,   199,   200,   300,   201,   202,   203,   204,
     205,   582,   583,   206,   207,   208,   209,   210,   211,   212,
     213,   138,   214,   215,   216,   217,   588,   218,   139,   219,
     220,   221,   299,   222,   223,   593,   594,   285,   546,    53,
     224,   225,   226,   227,   228,   138,   229,   230,   231,     1,
     273,   567,   274,   475,   275,   283,   369,   370,   373,   479,
     467,   299,   365,   366,   367,   459,   276,   460,   560,   474,
      23,   543,   559,    23,   561,     2,   563,     3,   569,   293,
     570,   584,   293,     4,   585,   562,     5,   577,     6,     7,
       8,     9,   590,    10,   592,    11,   453,    12,    13,    14,
      15,   451,    16,    17,    18,    19,    20,    21,    22,   450,
     452,   454,   285,   480,   481,   548,   282,   555,   285,    52,
      54,   464,   285,   138,   463,     0,   138,   138,   138,     0,
     579,     0,     0,   581,     0,     0,     0,     0,     0,   470,
       0,     0,   138,     0,     0,   299,     0,   285,     0,     0,
       0,   138,   138,   299,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    56,    57,   166,   166,    42,     1,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     166,     0,     0,     0,     0,     0,     0,     0,     0,   166,
     166,    58,    59,     0,     2,   155,     3,   156,   157,   158,
       0,    60,     4,   159,   160,     5,     0,     6,     7,     8,
       9,   161,    10,     0,    11,     0,    12,    13,    14,    15,
      61,    16,    17,    18,    19,    20,    21,    22,   162,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    55,    56,    57,     0,     0,
      42,     1,     0,     0,     0,     0,     0,   114,   115,     0,
       0,     0,   116,     0,     0,     0,     0,   117,     0,     0,
       0,   163,    51,   164,    58,    59,     0,     2,   155,     3,
     156,   157,   158,     0,    60,     4,   159,   160,     5,     0,
       6,     7,     8,     9,   161,    10,     0,    11,     0,    12,
      13,    14,    15,    61,    16,    17,    18,    19,    20,    21,
      22,   162,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    55,    56,    57,     0,     0,    42,     1,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    58,    59,     0,     2,     0,     0,     0,     0,
     114,   115,    60,     4,     0,   116,     0,     0,     6,     7,
     117,     0,     0,    10,   163,    51,   375,    12,    13,    14,
      15,    61,    16,     0,    18,    19,    20,    21,    22,     0,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    55,    56,    57,     0,
       0,    42,     1,     0,     0,     0,     0,     0,   114,   115,
       0,     0,     0,   116,     0,     0,     0,     0,   117,     0,
       0,     0,     0,   284,   571,    58,    59,     0,     2,   155,
       3,   156,   157,   158,     0,    60,     4,   159,   160,     5,
       0,     6,     7,     8,     9,   161,    10,     0,    11,     0,
      12,    13,    14,    15,    61,    16,    17,    18,    19,    20,
      21,    22,   162,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    55,    56,    57,     0,     0,    42,
       1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    58,    59,     0,     2,     0,     0,     0,
       0,   114,   115,    60,     4,     0,   116,     0,     0,     6,
       7,   117,     0,     0,    10,   163,    51,     0,    12,    13,
      14,    15,    61,    16,     0,    18,    19,    20,    21,    22,
       0,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,     0,     0,     0,     0,     1,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     2,     0,     0,     0,     0,     0,     0,
       0,     4,     0,    55,    56,    57,     6,     7,    42,     1,
       0,    10,     0,     0,     0,    12,    13,    14,    15,     0,
      16,     0,    18,    19,    20,    21,    22,     0,     0,   114,
     115,     0,    58,    59,   116,     2,     0,     3,     0,   117,
       0,     0,    60,     4,   284,     0,     5,     0,     6,     7,
       8,     9,     0,    10,     0,     0,     0,    12,    13,    14,
      15,    61,    16,    17,    18,    19,    20,    21,    22,     0,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    55,    56,    57,     0,     0,    42,     1,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      58,    59,     0,     2,     0,     0,     0,     0,   114,   115,
      60,     4,     0,   116,     0,     0,     6,     7,   117,     0,
       0,    10,   163,     0,     0,    12,    13,    14,    15,    61,
      16,     0,    18,    19,    20,    21,    22,     0,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    55,
      56,    57,     0,     0,    42,     1,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    58,    59,
       0,     2,     0,     0,     0,     0,   114,   115,    60,     4,
       0,   116,     0,     0,     6,     7,   117,     0,     0,    10,
     371,     0,     0,    12,    13,    14,    15,    61,    16,     0,
      18,    19,    20,    21,    22,     0,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    55,    56,    57,
       0,     0,    42,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    58,    59,     0,     2,
       0,     0,     0,     0,   114,   115,    60,     4,     0,   116,
       0,     0,     6,     7,   117,     0,     0,    10,   163,     0,
       0,    12,    13,    14,    15,    61,    16,     0,    18,    19,
      20,    21,    22,     0,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    55,
      56,    57,     0,     0,    42,     1,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    58,    59,
       0,     2,     0,     0,     0,     0,     0,     0,    60,     4,
       0,     0,   114,   115,     6,     7,     0,   116,   432,    10,
       0,     0,   117,    12,    13,    14,    15,    61,    16,     0,
      18,    19,    20,    21,    22,     0,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    55,    56,    57,     0,     0,    42,     1,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      58,    59,     0,     2,     0,     0,     0,     0,     0,     0,
      60,     4,     0,     0,   114,   115,     6,     7,     0,   116,
     457,    10,     0,     0,   117,    12,    13,    14,    15,    61,
      16,     0,    18,    19,    20,    21,    22,     0,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    55,    56,    57,     0,     0,    42,     1,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    58,    59,     0,     2,     0,     0,     0,     0,
       0,     0,    60,     4,     0,     0,   114,   115,     6,     7,
       0,   116,   580,    10,     0,     0,   117,    12,    13,    14,
      15,    61,    16,     0,    18,    19,    20,    21,    22,     0,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    55,    56,    57,     0,     0,
      42,     1,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    58,    59,     0,     2,     0,     0,
       0,     0,     0,     0,    60,     4,     0,     0,   114,   115,
       6,     7,     0,   116,     0,    10,     0,     0,   117,    12,
      13,    14,    15,    61,    16,     0,    18,    19,    20,    21,
      22,     0,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,     1,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       2,     0,   287,     0,     0,     0,     0,     0,     4,     0,
       0,   288,   289,     6,     7,     0,   290,     0,    10,     0,
       0,     0,    12,    13,    14,    15,     0,    16,     0,    18,
      19,    20,    21,   291,     0,     0,     0,     0,     0,     0,
     114,   115,     0,     0,     0,   177,     0,     0,     0,     0,
     117,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,   389,   390,   391,   392,   393,   394,   395,   396,   397,
     398,   399,   400,   401,   402,   403,   404,   405,   406,   407,
     408,   409,   410,   411,   412,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   422,   423,   424,   425,   426,   427,
     428,   429,     0,     0,     0,     0,     0,   433,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   458,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   292,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
       0,   332,   333,   334,   335,   336,   337,   338,   339,   340,
     341,   342,   343,   344,   345,   346,   347,   348,   349,   350,
     351,   352,   353,   354,   355,   356,   357,   358,   359,   360,
       1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     2,     0,     3,     0,
       0,     0,     0,     0,     4,     1,     0,     5,     0,     6,
       7,     8,     9,     0,    10,     0,    11,     0,    12,    13,
      14,    15,     0,    16,    17,    18,    19,    20,    21,    22,
       0,     2,     0,     3,     0,     0,     0,     0,     0,     4,
       1,     0,     5,     0,     6,     7,     8,     9,     0,    10,
       0,     0,     0,    12,    13,    14,    15,     0,    16,    17,
      18,    19,    20,    21,    22,     0,     2,     0,   287,     0,
       0,     0,     0,     0,     4,     1,     0,   288,   289,     6,
       7,     0,   290,     0,    10,     0,     0,     0,    12,    13,
      14,    15,     0,    16,     0,    18,    19,    20,    21,    22,
       0,     2,     0,     0,     0,     0,     0,     0,     0,     4,
       0,     0,   288,   289,     6,     7,     0,   290,     0,    10,
       0,     0,     0,    12,    13,    14,    15,     0,    16,     0,
      18,    19,    20,    21,    22
  };

  /* YYCHECK.  */
  const short int
  parser::yycheck_[] =
  {
         0,    39,    40,    41,   158,   152,   364,    45,    39,    40,
      41,   284,   363,    51,    45,     9,    11,   294,   295,    40,
      41,   175,    28,    27,    45,   253,   254,   255,    28,   147,
     196,    29,    30,     0,    29,     0,   369,    28,    38,    43,
     183,   196,    10,    43,   183,   183,   201,   196,    33,    34,
     196,    51,    43,   196,    49,   194,   183,   196,   201,    58,
      59,   202,   201,   201,   205,   195,   189,   190,    36,   196,
      38,    38,   202,    38,   201,   196,    44,     9,   116,    47,
     195,    49,    50,    51,    52,   116,    54,   202,   202,   362,
      58,    59,    60,    61,   203,    63,    64,    65,    66,    67,
      68,    69,   204,   195,   195,    38,   195,   204,   195,   460,
     202,   202,   150,   202,    47,   202,   116,   194,    51,    52,
     158,   202,   203,   161,   123,    11,    12,    13,    14,    15,
     161,    64,    18,    19,    20,    21,    22,   175,   194,   177,
     191,   192,   193,   147,   148,   145,   177,   147,   148,   144,
     483,   151,   195,   152,   145,   195,   147,   148,   158,   202,
     151,   195,   202,   195,   282,   195,   195,   195,   202,   195,
     202,   195,   202,   202,   202,   175,   202,   195,   202,   195,
     195,   195,    16,    17,   202,   195,   202,   202,   202,   187,
     188,   194,   202,   470,   200,   553,    31,    32,   195,   557,
     238,   552,   475,   431,   194,   202,   194,   238,   195,   194,
     204,   196,   194,   198,   252,   202,   363,   238,   195,   577,
     195,   195,   195,   195,   195,   202,   184,   202,   202,   202,
     202,   202,   195,   271,   272,   195,   195,   205,   195,   202,
     271,   195,   202,   202,   239,   202,   284,   194,   202,   260,
     261,   262,   263,   194,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   195,   195,   195,   194,   195,   550,   282,   202,
     202,   202,   282,   556,   279,   195,   195,   560,   186,   195,
     194,   282,   202,   202,   294,   295,   202,   183,   293,   195,
     195,   195,   195,   294,   295,   194,   202,   202,   202,   202,
     195,   195,   585,   460,   195,   195,   195,   202,   202,   197,
     195,   202,   202,   202,   362,   194,    10,   202,    26,   195,
     195,   369,   370,   195,   195,   373,   202,   202,   195,   370,
     202,   202,   373,   195,   195,   202,   195,   185,   195,   195,
     202,   202,    36,   202,    38,   202,   202,   202,   203,   202,
      44,   202,   203,    47,   363,    49,    50,    51,    52,   369,
      54,   195,   201,   202,    58,    59,    60,    61,   202,    63,
      64,    65,    66,    67,    68,    69,   202,   203,   256,   257,
     194,   258,   259,   264,   265,   194,   194,   544,   194,   194,
     194,   194,   194,   194,   194,   552,   194,   194,   194,   194,
     194,   565,   566,   194,   194,   194,   194,   194,   194,   194,
     194,   459,   194,   194,   194,   194,   580,   194,   459,   194,
     194,   194,   431,   194,   194,   589,   590,   475,   459,     0,
     194,   194,   194,   194,   194,   483,   194,   194,   194,    10,
     194,   489,   197,   183,   197,   197,   194,   194,   194,    70,
     195,   460,   203,   203,   203,   196,   461,   196,   183,   197,
     470,   197,   197,   473,   194,    36,   203,    38,   197,   470,
     197,   197,   473,    44,   183,   480,    47,   201,    49,    50,
      51,    52,    42,    54,   203,    56,   269,    58,    59,    60,
      61,   267,    63,    64,    65,    66,    67,    68,    69,   266,
     268,   270,   550,   369,   369,   461,   148,   473,   556,    34,
      38,   205,   560,   561,   279,    -1,   564,   565,   566,    -1,
     561,    -1,    -1,   564,    -1,    -1,    -1,    -1,    -1,   295,
      -1,    -1,   580,    -1,    -1,   544,    -1,   585,    -1,    -1,
      -1,   589,   590,   552,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     4,     5,     6,   565,   566,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     580,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   589,
     590,    33,    34,    -1,    36,    37,    38,    39,    40,    41,
      -1,    43,    44,    45,    46,    47,    -1,    49,    50,    51,
      52,    53,    54,    -1,    56,    -1,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     4,     5,     6,    -1,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,   189,   190,    -1,
      -1,    -1,   194,    -1,    -1,    -1,    -1,   199,    -1,    -1,
      -1,   203,   204,   205,    33,    34,    -1,    36,    37,    38,
      39,    40,    41,    -1,    43,    44,    45,    46,    47,    -1,
      49,    50,    51,    52,    53,    54,    -1,    56,    -1,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     4,     5,     6,    -1,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    34,    -1,    36,    -1,    -1,    -1,    -1,
     189,   190,    43,    44,    -1,   194,    -1,    -1,    49,    50,
     199,    -1,    -1,    54,   203,   204,   205,    58,    59,    60,
      61,    62,    63,    -1,    65,    66,    67,    68,    69,    -1,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,    -1,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,   189,   190,
      -1,    -1,    -1,   194,    -1,    -1,    -1,    -1,   199,    -1,
      -1,    -1,    -1,   204,   205,    33,    34,    -1,    36,    37,
      38,    39,    40,    41,    -1,    43,    44,    45,    46,    47,
      -1,    49,    50,    51,    52,    53,    54,    -1,    56,    -1,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     4,     5,     6,    -1,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    34,    -1,    36,    -1,    -1,    -1,
      -1,   189,   190,    43,    44,    -1,   194,    -1,    -1,    49,
      50,   199,    -1,    -1,    54,   203,   204,    -1,    58,    59,
      60,    61,    62,    63,    -1,    65,    66,    67,    68,    69,
      -1,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,    -1,    -1,    -1,    -1,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    44,    -1,     4,     5,     6,    49,    50,     9,    10,
      -1,    54,    -1,    -1,    -1,    58,    59,    60,    61,    -1,
      63,    -1,    65,    66,    67,    68,    69,    -1,    -1,   189,
     190,    -1,    33,    34,   194,    36,    -1,    38,    -1,   199,
      -1,    -1,    43,    44,   204,    -1,    47,    -1,    49,    50,
      51,    52,    -1,    54,    -1,    -1,    -1,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    -1,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     4,     5,     6,    -1,    -1,     9,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    34,    -1,    36,    -1,    -1,    -1,    -1,   189,   190,
      43,    44,    -1,   194,    -1,    -1,    49,    50,   199,    -1,
      -1,    54,   203,    -1,    -1,    58,    59,    60,    61,    62,
      63,    -1,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,
       5,     6,    -1,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    34,
      -1,    36,    -1,    -1,    -1,    -1,   189,   190,    43,    44,
      -1,   194,    -1,    -1,    49,    50,   199,    -1,    -1,    54,
     203,    -1,    -1,    58,    59,    60,    61,    62,    63,    -1,
      65,    66,    67,    68,    69,    -1,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,
      -1,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    34,    -1,    36,
      -1,    -1,    -1,    -1,   189,   190,    43,    44,    -1,   194,
      -1,    -1,    49,    50,   199,    -1,    -1,    54,   203,    -1,
      -1,    58,    59,    60,    61,    62,    63,    -1,    65,    66,
      67,    68,    69,    -1,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,
       5,     6,    -1,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    34,
      -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,
      -1,    -1,   189,   190,    49,    50,    -1,   194,   195,    54,
      -1,    -1,   199,    58,    59,    60,    61,    62,    63,    -1,
      65,    66,    67,    68,    69,    -1,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     4,     5,     6,    -1,    -1,     9,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    34,    -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    -1,    -1,   189,   190,    49,    50,    -1,   194,
     195,    54,    -1,    -1,   199,    58,    59,    60,    61,    62,
      63,    -1,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     4,     5,     6,    -1,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    34,    -1,    36,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    44,    -1,    -1,   189,   190,    49,    50,
      -1,   194,   195,    54,    -1,    -1,   199,    58,    59,    60,
      61,    62,    63,    -1,    65,    66,    67,    68,    69,    -1,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     4,     5,     6,    -1,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    33,    34,    -1,    36,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    44,    -1,    -1,   189,   190,
      49,    50,    -1,   194,    -1,    54,    -1,    -1,   199,    58,
      59,    60,    61,    62,    63,    -1,    65,    66,    67,    68,
      69,    -1,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      36,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    -1,
      -1,    47,    48,    49,    50,    -1,    52,    -1,    54,    -1,
      -1,    -1,    58,    59,    60,    61,    -1,    63,    -1,    65,
      66,    67,    68,    69,    -1,    -1,    -1,    -1,    -1,    -1,
     189,   190,    -1,    -1,    -1,   194,    -1,    -1,    -1,    -1,
     199,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,    -1,    -1,    -1,    -1,    -1,   237,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   273,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   195,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
      -1,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    36,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    44,    10,    -1,    47,    -1,    49,
      50,    51,    52,    -1,    54,    -1,    56,    -1,    58,    59,
      60,    61,    -1,    63,    64,    65,    66,    67,    68,    69,
      -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,
      10,    -1,    47,    -1,    49,    50,    51,    52,    -1,    54,
      -1,    -1,    -1,    58,    59,    60,    61,    -1,    63,    64,
      65,    66,    67,    68,    69,    -1,    36,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    44,    10,    -1,    47,    48,    49,
      50,    -1,    52,    -1,    54,    -1,    -1,    -1,    58,    59,
      60,    61,    -1,    63,    -1,    65,    66,    67,    68,    69,
      -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      -1,    -1,    47,    48,    49,    50,    -1,    52,    -1,    54,
      -1,    -1,    -1,    58,    59,    60,    61,    -1,    63,    -1,
      65,    66,    67,    68,    69
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned short int
  parser::yystos_[] =
  {
         0,    10,    36,    38,    44,    47,    49,    50,    51,    52,
      54,    56,    58,    59,    60,    61,    63,    64,    65,    66,
      67,    68,    69,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   250,   251,   258,   269,   270,   196,
     196,   196,     9,   204,   208,   196,   238,   235,   208,   202,
     203,   204,   268,     0,   269,     4,     5,     6,    33,    34,
      43,    62,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   189,   190,   194,   199,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   228,   229,
     231,   233,   210,   210,   235,   238,   256,   257,   204,   210,
     183,   194,   196,   201,   208,    37,    39,    40,    41,    45,
      46,    53,    70,   203,   205,   229,   233,   243,   250,   259,
     262,   263,   264,   265,   266,   267,   268,   194,   213,   213,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   229,   233,   197,    33,    34,   194,   196,   198,
     213,    11,    12,    13,    14,    15,    18,    19,    20,    21,
      22,   183,   227,   191,   192,   193,   189,   190,    16,    17,
      29,    30,   187,   188,    31,    32,   186,   185,   184,    26,
      28,   200,   202,   194,   197,   197,   208,   254,   255,   235,
     205,   256,   257,   197,   204,   228,   252,    38,    47,    48,
      52,    69,   195,   235,   245,   246,   247,   248,   249,   213,
     226,   230,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   244,   183,   196,   201,   203,   203,   203,   266,   194,
     194,   203,   229,   194,   203,   205,   266,   228,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     195,   195,   195,   232,   210,   208,   228,   214,   214,   214,
     215,   215,   216,   216,   217,   217,   217,   217,   218,   218,
     219,   220,   221,   222,   224,   229,   228,   195,   232,   196,
     196,   202,   203,   255,   205,   252,   253,   195,   208,   247,
     245,   247,   195,   202,   197,   183,   252,   230,   244,    70,
     239,   240,   260,   261,   264,   229,   203,   229,   195,   202,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   214,   195,   197,   201,   195,   210,   230,   254,   203,
     202,   205,   196,   201,   247,   248,   183,   201,   252,   197,
     183,   194,   208,   203,   264,   195,   195,   228,   226,   197,
     197,   205,   252,   230,   244,   252,   244,   201,   252,   229,
     195,   229,   266,   266,   197,   183,   244,   195,   266,   195,
      42,   252,   203,   266,   266
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,    61,   124,    94,    38,    62,    60,    43,
      45,    42,    47,    37,    40,    41,    91,    93,    46,    33,
      63,    58,    44,    59,   123,   125
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned short int
  parser::yyr1_[] =
  {
         0,   206,   207,   208,   209,   209,   209,   209,   209,   209,
     209,   210,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   212,   212,   212,   213,   213,   213,   213,   214,
     214,   215,   215,   215,   215,   216,   216,   216,   217,   217,
     217,   218,   218,   218,   218,   218,   219,   219,   219,   220,
     220,   221,   221,   222,   222,   223,   223,   224,   225,   225,
     226,   226,   227,   227,   227,   227,   227,   227,   227,   227,
     227,   227,   227,   228,   228,   229,   229,   230,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     232,   232,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   234,   235,   236,   236,   236,   236,   237,
     238,   238,   239,   239,   240,   240,   240,   240,   240,   240,
     240,   240,   241,   241,   241,   241,   241,   241,   241,   242,
     242,   242,   243,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   245,   245,   245,   246,   247,   247,   247,   248,
     248,   248,   248,   249,   249,   250,   250,   250,   251,   252,
     252,   252,   253,   253,   254,   254,   255,   255,   256,   256,
     257,   257,   258,   258,   259,   259,   260,   261,   261,   262,
     262,   262,   262,   263,   263,   263,   263,   263,   264,   264,
     265,   265,   265,   265,   265,   266,   266,   267,   267,   268,
     268,   269,   269,   270,   270
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  parser::yyr2_[] =
  {
         0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     4,     2,     2,     1,     3,     4,     3,
       3,     4,     1,     1,     1,     1,     2,     2,     2,     1,
       4,     1,     3,     3,     3,     1,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     1,     3,
       1,     5,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     3,     1,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     4,     4,
       4,     7,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     2,     2,     5,     4,     7,     4,     7,
       6,     9,     1,     3,     6,     5,     5,     8,     7,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     5,     4,     3,
       2,     2,     1,     1,     3,     5,     5,     4,     2,     1,
       3,     4,     1,     3,     1,     4,     1,     3,     3,     4,
       1,     2,     5,     4,     5,     7,     2,     1,     1,     5,
       7,     6,     7,     2,     2,     2,     3,     2,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       3,     1,     1,     1,     2
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const parser::yytname_[] =
  {
    "$end", "error", "$undefined", "LSG_SLT_CHAR_CONSTANT",
  "LSG_SLT_STRING_CONSTANT", "LSG_SLT_FLOAT_CONSTANT",
  "LSG_SLT_DEC_CONSTANT", "LSG_SLT_HEX_CONSTANT", "LSG_SLT_OCT_CONSTANT",
  "LSG_SLT_IDENTIFIER", "LSG_SLT_TYPENAME", "LSG_SLT_PLUS_EQUALS",
  "LSG_SLT_MINUS_EQUALS", "LSG_SLT_TIMES_EQUALS", "LSG_SLT_DIV_EQUALS",
  "LSG_SLT_MOD_EQUALS", "LSG_SLT_SHIFT_LEFT", "LSG_SLT_SHIFT_RIGHT",
  "LSG_SLT_AND_EQUALS", "LSG_SLT_XOR_EQUALS", "LSG_SLT_OR_EQUALS",
  "LSG_SLT_SHIFT_LEFT_EQUALS", "LSG_SLT_SHIFT_RIGHT_EQUALS", "LSG_SLT_NOT",
  "LSG_SLT_UMINUS", "LSG_SLT_UPLUS", "LSG_SLT_AND", "LSG_SLT_XOR",
  "LSG_SLT_OR", "LSG_SLT_GE", "LSG_SLT_LE", "LSG_SLT_EQUALS",
  "LSG_SLT_NOT_EQUALS", "LSG_SLT_PLUS_PLUS", "LSG_SLT_MINUS_MINUS",
  "LSG_SLT_BLENDSTATE", "LSG_SLT_BOOL", "LSG_SLT_BREAK", "LSG_SLT_CONST",
  "LSG_SLT_CONTINUE", "LSG_SLT_DISCARD", "LSG_SLT_DO", "LSG_SLT_ELSE",
  "LSG_SLT_FALSE", "LSG_SLT_FLOAT", "LSG_SLT_FOR", "LSG_SLT_IF",
  "LSG_SLT_IN", "LSG_SLT_INOUT", "LSG_SLT_INT", "LSG_SLT_MATRIX",
  "LSG_SLT_NOINTERPOLATION", "LSG_SLT_OUT", "LSG_SLT_RETURN",
  "LSG_SLT_SAMPLER", "LSG_SLT_SMOOTH", "LSG_SLT_STRUCT", "LSG_SLT_SWITCH",
  "LSG_SLT_TEXTURE1D", "LSG_SLT_TEXTURE2D", "LSG_SLT_TEXTURE3D",
  "LSG_SLT_TEXTURECUBE", "LSG_SLT_TRUE", "LSG_SLT_UINT", "LSG_SLT_UNIFORM",
  "LSG_SLT_VEC2", "LSG_SLT_VEC3", "LSG_SLT_VEC4", "LSG_SLT_VECTOR",
  "LSG_SLT_VOID", "LSG_SLT_WHILE", "LSG_SLT_ABS", "LSG_SLT_ACOS",
  "LSG_SLT_ANY", "LSG_SLT_ASIN", "LSG_SLT_ATAN", "LSG_SLT_ATAN2",
  "LSG_SLT_CEIL", "LSG_SLT_CLAMP", "LSG_SLT_COS", "LSG_SLT_CROSS",
  "LSG_SLT_DDX", "LSG_SLT_DDY", "LSG_SLT_DEGREES", "LSG_SLT_DETERMINANT",
  "LSG_SLT_DISTANCE", "LSG_SLT_DOT", "LSG_SLT_EXP", "LSG_SLT_EXP2",
  "LSG_SLT_FLOOR", "LSG_SLT_FRAC", "LSG_SLT_ISINF", "LSG_SLT_ISNAN",
  "LSG_SLT_LENGTH", "LSG_SLT_LERP", "LSG_SLT_LOG", "LSG_SLT_LOG2",
  "LSG_SLT_MAX", "LSG_SLT_MIN", "LSG_SLT_MIX", "LSG_SLT_MOD",
  "LSG_SLT_MUL", "LSG_SLT_NORMALIZE", "LSG_SLT_POW", "LSG_SLT_RADIANS",
  "LSG_SLT_RCP", "LSG_SLT_REFLECT", "LSG_SLT_REFRACT", "LSG_SLT_RSQRT",
  "LSG_SLT_SATURATE", "LSG_SLT_SIN", "LSG_SLT_SINCOS",
  "LSG_SLT_SMOOTHSTEP", "LSG_SLT_SQRT", "LSG_SLT_STEP", "LSG_SLT_TAN",
  "LSG_SLT_TEX1D", "LSG_SLT_TEX2D", "LSG_SLT_TEX2DSHADOW",
  "LSG_SLT_TEX2DSHADOWCOMPARE", "LSG_SLT_TEX3D", "LSG_SLT_TEXCUBE",
  "LSG_SLT_TRANSPOSE", "LSG_SLT_BINORMAL", "LSG_SLT_BLENDINDICES",
  "LSG_SLT_BLENDWEIGHT", "LSG_SLT_COLOR", "LSG_SLT_NORMAL",
  "LSG_SLT_POSITION", "LSG_SLT_POSITIONT", "LSG_SLT_PSIZE",
  "LSG_SLT_TANGENT", "LSG_SLT_TEXCOORD", "LSG_SLT_FOG",
  "LSG_SLT_TESSFACTOR", "LSG_SLT_ISFRONTFACE", "LSG_SLT_VPOS",
  "LSG_SLT_DEPTH", "LSG_SLT_AMBIENTLIGHT", "LSG_SLT_LIGHTCOUNT",
  "LSG_SLT_DIRLIGHTCOUNT", "LSG_SLT_POINTLIGHTCOUNT",
  "LSG_SLT_SPOTLIGHTCOUNT", "LSG_SLT_DIRLIGHTDIRS",
  "LSG_SLT_DIRLIGHTDIFFUSES", "LSG_SLT_POINTLIGHTPOS",
  "LSG_SLT_POINTLIGHTDIFFUSES", "LSG_SLT_POINTLIGHTSIZES",
  "LSG_SLT_SPOTLIGHTDIRS", "LSG_SLT_LIGHTAMBIENT", "LSG_SLT_LIGHTDIFFUSE",
  "LSG_SLT_LIGHTSPECULAR", "LSG_SLT_LIGHTATT", "LSG_SLT_SPOTLIGHTEXP",
  "LSG_SLT_SPOTLIGHTRANGE", "LSG_SLT_SKYCOLOR", "LSG_SLT_GROUNDCOLOR",
  "LSG_SLT_ASHIKFACTORS", "LSG_SLT_MATAMBIENT", "LSG_SLT_MATDIFFUSE",
  "LSG_SLT_MATEMISSIVE", "LSG_SLT_MATSPECULAR", "LSG_SLT_MATPOWER",
  "LSG_SLT_REFLECTIVITY", "LSG_SLT_ROUGHNESS", "LSG_SLT_FOGSTART",
  "LSG_SLT_FOGEND", "LSG_SLT_FOGCOLOR", "LSG_SLT_SHADOWMAPPING",
  "LSG_SLT_NORMALMATRIX", "LSG_SLT_TEXMATRIX", "LSG_SLT_TEXUNIT",
  "LSG_SLT_TIMESINCELASTFRAME", "LSG_SLT_SHADOWMAPUNIT",
  "LSG_SLT_SHADOWMAPMATRIX", "LSG_SLT_SHADOWMAPTEXDIM",
  "LSG_SLT_SHADOWMAPUVDEPTHSCALES", "LSG_SLT_SHADOWMAPCASTERSIZE",
  "LSG_SLT_CAMPOS", "LSG_SLT_VIEW", "LSG_SLT_WORLD", "LSG_SLT_WORLDVIEW",
  "LSG_SLT_WORLDVIEWPROJ", "'='", "'|'", "'^'", "'&'", "'>'", "'<'", "'+'",
  "'-'", "'*'", "'/'", "'%'", "'('", "')'", "'['", "']'", "'.'", "'!'",
  "'?'", "':'", "','", "';'", "'{'", "'}'", "$accept", "string",
  "identifier", "basic_exp", "integer_exp", "postfix_exp", "unary_op",
  "unary_exp", "cast_exp", "multiplicative_exp", "additive_exp",
  "shift_exp", "relational_exp", "equality_exp", "logical_and_exp",
  "logical_xor_exp", "logical_or_exp", "and_exp", "xor_exp", "or_exp",
  "conditional_exp", "assignment_ops", "assignment_exp", "exp",
  "constant_exp", "intrinsic", "argument_exp_list",
  "type_specifier_nonarray", "type_specifier_no_prec", "type_specifier",
  "storage_qualifier", "interpolation_qualifier", "type_qualifier",
  "fully_specified_type", "single_decl", "init_decl_list", "declaration",
  "declaration_statement", "parameter_semantic", "parameter_qualifier",
  "parameter_type_qualifier", "parameter_declarator",
  "parameter_declaration", "parameter_declaration_list",
  "function_declaration", "function_definition", "initializer",
  "initializer_list", "struct_declarator", "struct_declarator_list",
  "struct_declaration", "struct_declaration_list", "struct_specifier",
  "selection_statement", "for_init_statement_decl", "for_init_statement",
  "iteration_statement", "jump_statement", "expression_statement",
  "simple_statement", "statement", "statement_list", "compound_statement",
  "external_decl", "translation_unit", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const parser::rhs_number_type
  parser::yyrhs_[] =
  {
       270,     0,    -1,     4,    -1,     9,    -1,   207,    -1,     5,
      -1,     6,    -1,    43,    -1,    62,    -1,   208,    -1,   194,
     229,   195,    -1,   229,    -1,   209,    -1,   211,   196,   210,
     197,    -1,   211,    33,    -1,   211,    34,    -1,   231,    -1,
     211,   194,   195,    -1,   211,   194,   232,   195,    -1,   211,
     198,   208,    -1,   233,   194,   195,    -1,   233,   194,   232,
     195,    -1,   199,    -1,   190,    -1,   189,    -1,   211,    -1,
     212,   213,    -1,    33,   213,    -1,    34,   213,    -1,   213,
      -1,   194,   233,   195,   214,    -1,   214,    -1,   215,   191,
     214,    -1,   215,   192,   214,    -1,   215,   193,   214,    -1,
     215,    -1,   216,   189,   215,    -1,   216,   190,   215,    -1,
     216,    -1,   217,    16,   216,    -1,   217,    17,   216,    -1,
     217,    -1,   218,   187,   217,    -1,   218,   188,   217,    -1,
     218,    29,   217,    -1,   218,    30,   217,    -1,   218,    -1,
     219,    31,   218,    -1,   219,    32,   218,    -1,   219,    -1,
     220,   186,   219,    -1,   220,    -1,   221,   185,   220,    -1,
     221,    -1,   222,   184,   221,    -1,   222,    -1,   223,    26,
     222,    -1,   223,    -1,   224,    -1,   225,    28,   224,    -1,
     225,    -1,   225,   200,   229,   201,   226,    -1,   183,    -1,
      11,    -1,    12,    -1,    13,    -1,    14,    -1,    15,    -1,
      21,    -1,    22,    -1,    18,    -1,    19,    -1,    20,    -1,
     226,    -1,   213,   227,   228,    -1,   228,    -1,   229,   202,
     228,    -1,   226,    -1,    71,   194,   232,   195,    -1,    72,
     194,   232,   195,    -1,    73,   194,   232,   195,    -1,    74,
     194,   232,   195,    -1,    75,   194,   232,   195,    -1,    76,
     194,   232,   195,    -1,    77,   194,   232,   195,    -1,    78,
     194,   232,   195,    -1,    79,   194,   232,   195,    -1,    80,
     194,   232,   195,    -1,    81,   194,   232,   195,    -1,    82,
     194,   232,   195,    -1,    83,   194,   232,   195,    -1,    84,
     194,   232,   195,    -1,    85,   194,   232,   195,    -1,    86,
     194,   232,   195,    -1,    87,   194,   232,   195,    -1,    88,
     194,   232,   195,    -1,    89,   194,   232,   195,    -1,    90,
     194,   232,   195,    -1,    91,   194,   232,   195,    -1,    92,
     194,   232,   195,    -1,    93,   194,   232,   195,    -1,    94,
     194,   232,   195,    -1,    95,   194,   232,   195,    -1,    96,
     194,   232,   195,    -1,    97,   194,   232,   195,    -1,    98,
     194,   232,   195,    -1,    99,   194,   232,   195,    -1,   100,
     194,   232,   195,    -1,   101,   194,   232,   195,    -1,   102,
     194,   232,   195,    -1,   103,   194,   232,   195,    -1,   104,
     194,   232,   195,    -1,   105,   194,   232,   195,    -1,   106,
     194,   232,   195,    -1,   107,   194,   232,   195,    -1,   108,
     194,   232,   195,    -1,   109,   194,   232,   195,    -1,   110,
     194,   232,   195,    -1,   111,   194,   232,   195,    -1,   112,
     194,   232,   195,    -1,   113,   194,   232,   195,    -1,   114,
     194,   232,   195,    -1,   115,   194,   232,   195,    -1,   116,
     194,   232,   195,    -1,   117,   194,   232,   195,    -1,   120,
     194,   232,   195,    -1,   121,   194,   232,   195,    -1,   118,
     194,   232,   195,    -1,   119,   194,   232,   195,    -1,   122,
     194,   232,   195,    -1,   228,    -1,   232,   202,   228,    -1,
      69,    -1,    54,    -1,    58,    -1,    59,    -1,    60,    -1,
      61,    -1,    44,    -1,    49,    -1,    63,    -1,    36,    -1,
      65,    -1,    66,    -1,    67,    -1,    68,    -1,    50,    -1,
      44,   196,   210,   197,    -1,    49,   196,   210,   197,    -1,
      63,   196,   210,   197,    -1,    36,   196,   210,   197,    -1,
      44,   196,   210,   197,   196,   210,   197,    -1,    10,    -1,
     233,    -1,   234,    -1,    38,    -1,    47,    -1,    52,    -1,
      64,    -1,    51,    -1,   236,    -1,   237,   238,    -1,   235,
      -1,   238,   235,    -1,   239,   208,    -1,   239,   208,   196,
     230,   197,    -1,   239,   208,   183,   252,    -1,   239,   208,
     196,   230,   197,   183,   252,    -1,   239,   208,   201,   244,
      -1,   239,   208,   196,   230,   197,   201,   244,    -1,   239,
     208,   201,   244,   183,   252,    -1,   239,   208,   196,   230,
     197,   201,   244,   183,   252,    -1,   240,    -1,   241,   202,
     208,    -1,   241,   202,   208,   196,   230,   197,    -1,   241,
     202,   208,   183,   252,    -1,   241,   202,   208,   201,   244,
      -1,   241,   202,   208,   196,   230,   197,   201,   244,    -1,
     241,   202,   208,   201,   244,   183,   252,    -1,   241,    -1,
     258,    -1,   250,    -1,   242,   203,    -1,   123,    -1,   124,
      -1,   125,    -1,   126,    -1,   127,    -1,   128,    -1,   129,
      -1,   130,    -1,   131,    -1,   132,    -1,   133,    -1,   134,
      -1,   135,    -1,   136,    -1,   137,    -1,   138,    -1,   139,
      -1,   140,    -1,   141,    -1,   142,    -1,   143,    -1,   144,
      -1,   145,    -1,   146,    -1,   147,    -1,   148,    -1,   149,
      -1,   150,    -1,   151,    -1,   152,    -1,   154,    -1,   155,
      -1,   156,    -1,   158,    -1,   159,    -1,   160,    -1,   161,
      -1,   162,    -1,   163,    -1,   164,    -1,   157,    -1,   165,
      -1,   166,    -1,   167,    -1,   168,    -1,   169,    -1,   170,
      -1,   171,    -1,   172,    -1,   173,    -1,   174,    -1,   176,
      -1,   177,    -1,   175,    -1,   178,    -1,   179,    -1,   180,
      -1,   181,    -1,   182,    -1,    47,    -1,    52,    -1,    48,
      -1,    38,    -1,   235,   208,    -1,   235,   208,   196,   230,
     197,    -1,   235,   208,   201,   244,    -1,   246,   245,   247,
      -1,   245,   247,    -1,   246,   247,    -1,   247,    -1,   248,
      -1,   249,   202,   248,    -1,   239,   208,   194,   249,   195,
      -1,   239,   208,   194,    69,   195,    -1,   239,   208,   194,
     195,    -1,   250,   268,    -1,   228,    -1,   204,   253,   205,
      -1,   204,   253,   202,   205,    -1,   252,    -1,   253,   202,
     252,    -1,   208,    -1,   208,   196,   230,   197,    -1,   254,
      -1,   255,   202,   254,    -1,   235,   255,   203,    -1,   238,
     235,   255,   203,    -1,   256,    -1,   257,   256,    -1,    56,
     208,   204,   257,   205,    -1,    56,   204,   257,   205,    -1,
      46,   194,   229,   195,   266,    -1,    46,   194,   229,   195,
     266,    42,   266,    -1,   240,   203,    -1,   264,    -1,   260,
      -1,    70,   194,   229,   195,   266,    -1,    41,   266,    70,
     194,   229,   195,   203,    -1,    45,   194,   261,   264,   195,
     266,    -1,    45,   194,   261,   264,   229,   195,   266,    -1,
      39,   203,    -1,    37,   203,    -1,    53,   203,    -1,    53,
     229,   203,    -1,    40,   203,    -1,   203,    -1,   229,   203,
      -1,   264,    -1,   243,    -1,   259,    -1,   262,    -1,   263,
      -1,   265,    -1,   268,    -1,   266,    -1,   267,   266,    -1,
     204,   205,    -1,   204,   267,   205,    -1,   251,    -1,   243,
      -1,   269,    -1,   270,   269,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  parser::yyprhs_[] =
  {
         0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      19,    23,    25,    27,    32,    35,    38,    40,    44,    49,
      53,    57,    62,    64,    66,    68,    70,    73,    76,    79,
      81,    86,    88,    92,    96,   100,   102,   106,   110,   112,
     116,   120,   122,   126,   130,   134,   138,   140,   144,   148,
     150,   154,   156,   160,   162,   166,   168,   172,   174,   176,
     180,   182,   188,   190,   192,   194,   196,   198,   200,   202,
     204,   206,   208,   210,   212,   216,   218,   222,   224,   229,
     234,   239,   244,   249,   254,   259,   264,   269,   274,   279,
     284,   289,   294,   299,   304,   309,   314,   319,   324,   329,
     334,   339,   344,   349,   354,   359,   364,   369,   374,   379,
     384,   389,   394,   399,   404,   409,   414,   419,   424,   429,
     434,   439,   444,   449,   454,   459,   464,   469,   474,   479,
     484,   486,   490,   492,   494,   496,   498,   500,   502,   504,
     506,   508,   510,   512,   514,   516,   518,   520,   525,   530,
     535,   540,   548,   550,   552,   554,   556,   558,   560,   562,
     564,   566,   569,   571,   574,   577,   583,   588,   596,   601,
     609,   616,   626,   628,   632,   639,   645,   651,   660,   668,
     670,   672,   674,   677,   679,   681,   683,   685,   687,   689,
     691,   693,   695,   697,   699,   701,   703,   705,   707,   709,
     711,   713,   715,   717,   719,   721,   723,   725,   727,   729,
     731,   733,   735,   737,   739,   741,   743,   745,   747,   749,
     751,   753,   755,   757,   759,   761,   763,   765,   767,   769,
     771,   773,   775,   777,   779,   781,   783,   785,   787,   789,
     791,   793,   795,   797,   799,   801,   803,   806,   812,   817,
     821,   824,   827,   829,   831,   835,   841,   847,   852,   855,
     857,   861,   866,   868,   872,   874,   879,   881,   885,   889,
     894,   896,   899,   905,   910,   916,   924,   927,   929,   931,
     937,   945,   952,   960,   963,   966,   969,   973,   976,   978,
     981,   983,   985,   987,   989,   991,   993,   995,   997,  1000,
    1003,  1007,  1009,  1011,  1013
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  parser::yyrline_[] =
  {
         0,   176,   176,   180,   184,   188,   189,   190,   191,   192,
     193,   197,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   219,   221,   222,   226,   227,   228,   229,   233,
     234,   238,   239,   240,   241,   245,   246,   247,   251,   252,
     253,   257,   258,   259,   260,   261,   265,   266,   267,   271,
     272,   276,   277,   281,   282,   286,   287,   291,   296,   297,
     301,   302,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   320,   321,   325,   326,   330,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360,   361,   362,   363,   364,   365,
     366,   367,   368,   369,   370,   371,   372,   373,   374,   375,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     389,   390,   394,   395,   396,   397,   398,   399,   400,   401,
     402,   403,   404,   405,   406,   407,   408,   409,   417,   425,
     433,   441,   454,   458,   462,   466,   467,   468,   469,   473,
     477,   478,   482,   483,   487,   488,   489,   490,   492,   493,
     495,   497,   502,   503,   507,   511,   515,   519,   524,   532,
     533,   534,   538,   542,   543,   544,   545,   546,   547,   548,
     549,   550,   551,   552,   553,   554,   555,   556,   557,   558,
     559,   560,   561,   562,   563,   564,   565,   566,   567,   568,
     569,   570,   571,   572,   573,   574,   575,   576,   577,   578,
     579,   580,   581,   582,   583,   584,   585,   586,   587,   588,
     589,   590,   591,   592,   593,   594,   595,   596,   597,   598,
     599,   600,   604,   605,   606,   610,   614,   615,   616,   620,
     622,   623,   625,   629,   630,   634,   636,   637,   641,   645,
     646,   647,   651,   652,   656,   657,   661,   662,   666,   667,
     671,   672,   676,   677,   681,   682,   686,   690,   691,   695,
     696,   697,   699,   704,   705,   706,   707,   708,   712,   713,
     717,   718,   719,   720,   721,   725,   726,   730,   731,   735,
     736,   740,   741,   745,   746
  };

  // Print the state stack on the debug stream.
  void
  parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  parser::token_number_type
  parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   199,     2,     2,     2,   193,   186,     2,
     194,   195,   191,   189,   202,   190,   198,   192,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   201,   203,
     188,   183,   187,   200,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   196,     2,   197,   185,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   204,   184,   205,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int parser::yyeof_ = 0;
  const int parser::yylast_ = 2904;
  const int parser::yynnts_ = 65;
  const int parser::yyempty_ = -2;
  const int parser::yyfinal_ = 53;
  const int parser::yyterror_ = 1;
  const int parser::yyerrcode_ = 256;
  const int parser::yyntokens_ = 206;

  const unsigned int parser::yyuser_token_number_max_ = 437;
  const parser::token_number_type parser::yyundef_token_ = 2;



} // yy






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
