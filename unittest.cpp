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
	CharTree<char,const char> tree;
	char buf[4096];
	std::string temp;
	std::vector<std::string> svec;
	
	while ( !std::cin.eof() ) {
		std::cin.getline(buf,sizeof buf);
		// printf("Got line '%s'\n",buf);
		std::string line(buf);
		tree.put(line.c_str(),strdup(line.c_str()));
		temp = line;

		if ( line.empty() )
			continue;

		svec.push_back(buf);
		const char *vp = tree.get(line.c_str());
		assert(!strcmp(vp,line.c_str()));
	}			

	for ( auto& path : svec ) {
		const char *vp = tree.get(path.c_str());
		assert(!strcmp(vp,path.c_str()));
	}

	auto dumpit = [](const char *path,CharTree<char,const char>& tree,CharTree<char,const char>& root,void *udata) {
		const char *datap = tree.get();

		if ( datap != nullptr )
			printf("path = '%s' -> '%s'\n",path,datap ? datap : "<empty>");
	};

	tree.traverse(dumpit,nullptr);
	
	return 0;
}

// End main.cpp
