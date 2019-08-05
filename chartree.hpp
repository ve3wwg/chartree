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
public:
	typedef void (*callback_1)(const std::basic_string<C>& path,CharTree& tree,CharTree& root,void *udata);
	typedef void (*callback_2)(const std::basic_string<C>& prefix,const std::basic_string<C>& suffix,CharTree& tree,CharTree& root,void *udata);

protected:
	D			*data = nullptr;
	std::map<C,CharTree*>	nodes;

	void traverse(const std::basic_string<C>& path,callback_1 callback,CharTree& root,void *udata);
	void optimize(const std::basic_string<C>& path,size_t tailcount,callback_2 callback,CharTree& root,void *udata);

public:	CharTree() {}
	~CharTree() {}
	
	CharTree& put(const C *path,D *data);
	D *get(const C *path) const;
	D *get() { return data; }
	CharTree& traverse(callback_1 callback,void *udata);
	CharTree& optimize(callback_2 callback,void *udata);
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
CharTree<C,D>::traverse(void (*callback)(const std::basic_string<C>& path,CharTree& tree,CharTree& root,void *udata),void *udata) {
	std::basic_string<C> path;

	traverse(path,callback,*this,udata);
	return *this;
}

template<typename C,typename D>
void
CharTree<C,D>::traverse(const std::basic_string<C>& path,callback_1 callback,CharTree& root,void *udata) {

	for ( auto& pair : nodes ) {
		const C ch = pair.first;
		CharTree<C,D> *np = pair.second;
		std::basic_string<C> tpath(path);
		tpath += ch;

		callback(tpath,*np,root,udata);
		np->traverse(tpath,callback,root,udata);
	}
}

template<typename C,typename D>
CharTree<C,D>&
CharTree<C,D>::optimize(callback_2 callback,void *udata) {
	std::basic_string<C> path;

	optimize(path,0u,callback,*this,udata);
	return *this;
}

template<typename C,typename D>
void
CharTree<C,D>::optimize(const std::basic_string<C>& path,size_t tailcount,callback_2 callback,CharTree& root,void *udata) {

	if ( nodes.size() > 1 ) {
		std::basic_string<C> suffix;

		callback(path,suffix,*this,root,udata);	// Call with common prefix 
	}

	for ( auto& pair : nodes ) {
		const C ch = pair.first;
		CharTree<C,D> *np = pair.second;
		std::basic_string<C> tpath(path);
		tpath += ch;
		size_t tc = nodes.size() == 1 && !this->data ? tailcount + 1 : 1;

		if ( np->data ) {
			std::basic_string prefix(tpath.substr(0,tpath.size()-tc));
			std::basic_string suffix(tpath.substr(tpath.size()-tc));

			callback(prefix,suffix,*np,root,udata);
		}
		np->optimize(tpath,tc,callback,root,udata);
	}
}

#endif // CHARTREE_HPP

// End chartree.hpp
