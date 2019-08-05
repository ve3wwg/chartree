//////////////////////////////////////////////////////////////////////
// chartree.hpp -- CharTree Class
// Date: Sun Aug  4 21:57:06 2019   (C) Warren W. Gay ve3wwg@gmail.com
///////////////////////////////////////////////////////////////////////

#ifndef CHARTREE_HPP
#define CHARTREE_HPP

#include <map>
#include <list>
#include <string>

template<typename C,typename D>
class CharTree {
public:
	typedef std::list<std::basic_string<C>> seglist_t;
	typedef void (*callback_1)(const std::basic_string<C>& path,CharTree& tree,CharTree& root,void *udata);
	typedef void (*callback_2)(const seglist_t& prefix,const std::basic_string<C>& suffix,CharTree& tree,CharTree& root,void *udata);
	typedef void (*callback_3)(D *data);

protected:

	D			*data = nullptr;
	std::map<C,CharTree*>	nodes;

	void traverse(const std::basic_string<C>& path,callback_1 callback,CharTree& root,void *udata);
	void optimize(const std::basic_string<C>& path,size_t tailcount,seglist_t& seglist,callback_2 callback,CharTree& root,void *udata);

public:	CharTree() {}
	~CharTree() {}
	
	CharTree& put(const C *path,D *data);
	D *get(const C *path) const;
	D *get() { return data; }
	CharTree& traverse(callback_1 callback,void *udata);
	CharTree& optimize(callback_2 callback,void *udata);
	CharTree& clear(callback_3 callback=nullptr);
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
	seglist_t seglist;

	optimize(path,0u,seglist,callback,*this,udata);
	return *this;
}

template<typename C,typename D>
void
CharTree<C,D>::optimize(const std::basic_string<C>& path,size_t tailcount,seglist_t& seglist,callback_2 callback,CharTree& root,void *udata) {
	seglist_t prefix(seglist);

	if ( nodes.size() > 1 && !path.empty() ) {
		std::basic_string<C> suffix;

		prefix.push_back(path.substr(path.size()-tailcount));
	}

	for ( auto& pair : nodes ) {
		const C ch = pair.first;
		CharTree<C,D> *np = pair.second;
		std::basic_string<C> tpath(path);
		tpath += ch;
		size_t tc = nodes.size() == 1 && !this->data ? tailcount + 1 : 1;

		if ( np->data ) {
			std::basic_string<C> suffix(tpath.substr(tpath.size()-tc));

			callback(prefix,suffix,*np,root,udata);
		}
		np->optimize(tpath,tc,prefix,callback,root,udata);
	}
}

template<typename C,typename D>
CharTree<C,D>&
CharTree<C,D>::clear(callback_3 callback) {
	
	if ( callback ) {
		if ( this->data ) {
			callback(this->data);
			this->data = nullptr;
		}
	}
	for ( auto& pair : nodes ) {
		pair.second->clear(callback);
		delete pair.second;
	}
	nodes.clear();
	return *this;
}

#endif // CHARTREE_HPP

// End chartree.hpp
