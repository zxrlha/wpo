bin_PROGRAMS = wpo
BUILT_SOURCES = parser.cpp parser.hpp lexer.cpp
AM_YFLAGS=-d -Wno-yacc
wpo_CPPFLAGS = $(BOOST_CPPFLAGS)
wpo_LDADD = $(BOOST_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LIB)
wpo_CXXFLAGS = $(OPENMP_CXXFLAGS)
wpo_SOURCES = expression.hpp expression.cpp \
			  kernel.hpp kernel.cpp \
			  kcm.hpp kcm.cpp \
			  extraction.hpp extraction.cpp \
			  literal.hpp literal.cpp \
			  wpo.cpp \
			  parser.ypp \
			  lexer.lpp \
			  yyglobal.hpp yyglobal.cpp \
			  output.hpp output.cpp
