%header{
#include<iostream>
}

%rules

"ss" {
  std::cout << "ss";
  std::cout << "ss";
  std::cout << "ss";
}
"ss" {std::cout << "ss";}
/[0-9]+/ {std::cout << "num";}
