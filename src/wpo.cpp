#include "kernel.hpp"
#include <iostream>
#include "extraction.hpp"
#include "kcm.hpp"
#include "literal.hpp"
#include "yyglobal.hpp"
#include "parser.hpp"

int main()
{
	yyparse();
	find_kernel_intersections(vP);
	find_cube_intersections(vP);
	for (int i = 0; i < vP.size(); ++i)
	{
		std::cout<<vP[i]<<std::endl;
	}

	return 0;
}
