#include "kernel.hpp"
#include <iostream>
#include "extraction.hpp"
#include "kcm.hpp"
#include "literal.hpp"
#include "yyglobal.hpp"
#include "parser.hpp"
#include "output.hpp"

int main()
{
	yyparse();
	find_kernel_intersections(vP);
	find_cube_intersections(vP);

	reorder(vP);
	for (int i = 0; i < vP.size(); ++i)
	{
		//std::cout<<vP[i]<<std::endl;
	}
	rename(vP);
	for (int i = 0; i < vP.size(); ++i)
	{
		std::cout<<vP[i]<<line_suffix<<std::endl;
	}
	return 0;
}
