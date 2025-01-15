#include "common.h"

TEST(Statements, VariableDeclaration) {
  checkExpression("let x = 5", "fn main() {\nlet mut x = 5;\n}\n", false);
}

TEST(Statements, Assignment) {
  checkExpression("let x = 5", "fn main() {\nlet mut x = 5;\n}\n", false);
  checkExpression("let x = 5 x = 10",
                  "fn main() {\nlet mut x = 5;\nx = 10;\n}\n", false);
}

TEST(Statements, PrintStatements) {
  checkExpression("print 42", "fn main() {\nprint!(\"{}\", 42);\n}\n", false);

  checkExpression("println 42", "fn main() {\nprintln!(\"{}\", 42);\n}\n",
                  false);
}

TEST(Statements, ReadStatement) {
  checkExpression("read x",
                  "fn main() {\nlet mut line = String::new();\n"
                  "std::io::stdin().read_line(&mut line).unwrap();\n"
                  "let x = line.trim().parse().unwrap();\n}\n",
                  false);
}

TEST(Statements, IfStatement) {
  checkExpression("if == 1 1 { println 42 }",
                  "fn main() {\nif (1 == 1) {\nprintln!(\"{}\", 42);\n}\n}\n",
                  false);
}

TEST(Statements, IfElseStatement) {
  checkExpression("if == 1 2 { println 42 } else { println 43 }",
                  "fn main() {\nif (1 == 2) {\nprintln!(\"{}\", 42);\n}\n"
                  "else {\nprintln!(\"{}\", 43);\n}\n}\n",
                  false);
}

TEST(Statements, ComplexProgram) {
  checkExpression("let x = 5 if == x 5 { println x } else { print 0 }",
                  "fn main() {\n"
                  "let mut x = 5;\n"
                  "if (x == 5) {\n"
                  "println!(\"{}\", x);\n"
                  "}\n"
                  "else {\n"
                  "print!(\"{}\", 0);\n"
                  "}\n"
                  "}\n",
                  false);

  checkExpression("if > 2 3 print 3 if > 4 - 7 2 print + 3 4",
                  "fn main() {\n"
                  "if (2 > 3) print!(\"{}\", 3);\n"
                  "else if (4 > (7 - 2)) print!(\"{}\", (3 + 4));\n"
                  "}\n",
                  false);
}