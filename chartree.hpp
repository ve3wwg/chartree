//////////////////////////////////////////////////////////////////////
// chartree.hpp -- CharTree Class
// Date: Sun Aug  4 21:57:06 2019   (C) Warren W. Gay ve3wwg@gmail.com
///////////////////////////////////////////////////////////////////////

#ifndef CHARTREE_HPP
#define CHARTREE_HPP

#include <map>
#include <string>

template<typename C,typename D>
class CharTree {
	D			*data = nullptr;
	std::map<C,CharTree*>	nodes;

	void traverse(const std::basic_string<C>& path,void (*func)(const C *path,CharTree& tree,CharTree& root),CharTree& root);

public:	CharTree() {}
	~CharTree() {}
	
	CharTree& put(const C *path,D *data);
	D *get(const C *path) const;
	D *get() { return data; }
	CharTree& traverse(void (*func)(const C *path,CharTree& tree,CharTree& root));
};

template<typename C,typename D>
CharTree<C,D>&
CharTree<C,D>::put(const C *path,D *datap) {
	C ch;
	CharTree *np = this;
	
	if ( !path || !*path )
		return *this;

	while ( (ch = *path++) != C(0) ) {
		auto it = np->nodes.find(ch);
		if ( it == np->nodes.end() ) {
			CharTree *p = new CharTree();
			np->nodes[ch] = p;
			np = p;
		} else	{
			np = it->second;
		}
	}
	np->data = datap;
	return *np;
}

template<typename C,typename D>
D *
CharTree<C,D>::get(const C *path) const {
	C ch;
	const CharTree *np = this;
	
	if ( !path || !*path )
		return this->data;

	while ( (ch = *path++) != C(0) ) {
		auto it = np->nodes.find(ch);
		if ( it == np->nodes.end() )
			return nullptr;
		np = it->second;
	}
	return np->data;
}

template<typename C,typename D>
CharTree<C,D>&
CharTree<C,D>::traverse(void (*func)(const C *path,CharTree& tree,CharTree& root)) {
	std::basic_string<C> path;

	traverse(path,func,*this);
	return *this;
}

template<typename C,typename D>
void
CharTree<C,D>::traverse(const std::basic_string<C>& path,void (*func)(const C *path,CharTree& tree,CharTree& root),CharTree& root) {

	for ( auto& pair : nodes ) {
		const C ch = pair.first;
		CharTree<C,D> *np = pair.second;
		std::basic_string<C> tpath(path);
		tpath += ch;

		func(tpath.c_str(),*np,root);
		np->traverse(tpath,func,root);
	}
}

#endif // CHARTREE_HPP

// End chartree.hpp
