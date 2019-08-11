//////////////////////////////////////////////////////////////////////
// main.cpp -- CharTree test program
// Date: Sun Aug  4 21:56:46 2019   (C) ve3wwg@gmail.com
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "chartree.hpp"

#include <iostream>
#include <string>
#include <vector>

int
main(int argc,char **argv) {
	CharTree<char> tree([](char *udata){free(udata);});
	char buf[4096];
	std::string temp;
	std::vector<std::string> svec;
	
	//////////////////////////////////////////////////////////////
	// Load tree from test data
	//////////////////////////////////////////////////////////////

	while ( !std::cin.eof() ) {
		std::cin.getline(buf,sizeof buf);

		if ( !*buf )
			continue;

		std::string line(buf);
		tree.put(line.c_str(),strdup(line.c_str()));
		temp = line;

		svec.push_back(buf);
		const char *vp = tree.get(line.c_str());
		assert(!strcmp(vp,line.c_str()));
	}			

	//////////////////////////////////////////////////////////////
	// Validate loaded tree:
	//////////////////////////////////////////////////////////////

	for ( auto& path : svec ) {
		const char *vp = tree.get(path.c_str());
		assert(!strcmp(vp,path.c_str()));
	}

	//////////////////////////////////////////////////////////////
	// Traversal test:
	//////////////////////////////////////////////////////////////

	auto dumpit = [](const std::string& path,CharTree<char>& tree,CharTree<char>& root,void *udata) {
		const char *datap = tree.get();

		if ( datap != nullptr )
			printf("path = '%s' -> '%s'\n",path.c_str(),datap ? datap : "<empty>");
	};

	puts("TRAVERSE:");
	tree.traverse(dumpit,nullptr);

	//////////////////////////////////////////////////////////////
	// Optimized traversal test:
	//////////////////////////////////////////////////////////////

	auto dump2 = [](const CharTree<char>::seglist_t& prefix,const std::string& suffix,CharTree<char>& tree,CharTree<char>& root,void *udata) {
		const char *datap = tree.get();

		for ( auto& comp : prefix )
			printf(" '%s'",comp.c_str());

		printf(" . '%s' -> '%s'\n",suffix.c_str(),datap ? datap : "<empty>");
	};

	puts("OPTIMIZED:");
	tree.optimize(dump2,nullptr);

	return 0;
}

// End main.cpp
