%{
#pragma warning(disable: 4005)
#include <string>

#include "comments/grammar.hpp"

#define YY_USER_ACTION loc.columns(yyleng);

#define YY_DECL yy_comments::parser::symbol_type yylex(yyscan_t yyscanner, yy_comments::location &loc)

#define MAKE(x) yy_comments::parser::make_ ## x(loc)
#define MAKE_VALUE(x, v) yy_comments::parser::make_ ## x((v), loc)

std::string comment;
%}

%option nounistd
%option yylineno
%option nounput
%option batch
%option never-interactive
%option reentrant
%option noyywrap
%option prefix="ll_comments"

%x COMMENT

%%

%{
    // Code run each time yylex is called.
    loc.step();
%}

#.*/\n                  ; // ignore comments

[ \t]+                  loc.step();
\r                      loc.step();
\n                      {
                            loc.lines(yyleng);
                            loc.step();
                        }

\/\/.*                  ;

<INITIAL>"/*"           { BEGIN(COMMENT); comment.clear(); }
<COMMENT>[^*\n]*        |
<COMMENT>"*"+[^*/\n]*   |
<COMMENT>\n             comment += yytext;
<COMMENT>"*"+"/"        { BEGIN(INITIAL); return MAKE_VALUE(STRING, comment);}

.                       { /*driver.error(loc, "invalid character");*/ return MAKE(ERROR_SYMBOL); }
<<EOF>>                 return MAKE(EOQ);

%%
