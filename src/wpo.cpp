#include "kernel.hpp"
#include <iostream>
#include "extraction.hpp"
#include "kcm.hpp"
#include "literal.hpp"
#include "yyglobal.hpp"
#include "parser.hpp"
#include "output.hpp"

using std::cout;
using std::cerr;
using std::endl;

int main()
{
	yyparse();
	cerr<<"//Parse finished"<<endl;
	//return 0;
	find_kernel_intersections(vP);
	cerr<<"//kernel intersection finished"<<endl;
	find_cube_intersections(vP);
	cerr<<"//cube intersection finished"<<endl;

	reorder(vP);
	cerr<<"//reorder intersection finished"<<endl;
	rename(vP);
	cerr<<"//rename intersection finished"<<endl;
	for (int i = 0; i < vP.size(); ++i)
	{
		std::cout<<vP[i]<<line_suffix<<std::endl;
	}
	return 0;
}
