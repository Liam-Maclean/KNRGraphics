
%{
  #include <iostream.h>
  #include <cctype>
  #include <cstring>
  #include <vector>
  #include <stack>

  #include "ast.h"

  // Bring the standard library into the
  // global namespace
  using namespace std;

  // Prototypes to keep the compiler happy
  void yyerror (const char *error);
  
  // Variables
  int vars ['Z'- 'A' + 1];

  // stack class that takes care of all the nodes that were allocated
  stack <Expression *> nodes;
%}

// we need to provid the following functions
int main ()
{
  memset (vars, 0, sizeof (vars));
  return yyparse ();
}

void yyerror (const char *error)
{
  cout << error << endl;
}