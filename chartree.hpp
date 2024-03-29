//////////////////////////////////////////////////////////////////////
// chartree.hpp -- CharTree Class
// Date: Sun Aug  4 21:57:06 2019   (C) Warren W. Gay ve3wwg@gmail.com
///////////////////////////////////////////////////////////////////////

#ifndef CHARTREE_HPP
#define CHARTREE_HPP

#include <map>
#include <list>
#include <string>

//////////////////////////////////////////////////////////////////////
// Typical Usage:
//
//	See bottom of this file for CharTree<typename D> which
//	assumes the Key type is 'char'.
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Tree Class:
//
//	typename C	Key type (char or wide char type)
//	typename D	Data type (data is allocated as D*)
//
//////////////////////////////////////////////////////////////////////

template<typename C,typename D>
class BasicCharTree {
public:
	typedef std::list<std::basic_string<C>> seglist_t;
	typedef void (*callback_1)(const std::basic_string<C>& path,BasicCharTree& tree,BasicCharTree& root,void *udata);
	typedef void (*callback_2)(const seglist_t& prefix,const std::basic_string<C>& suffix,BasicCharTree& tree,BasicCharTree& root,void *udata);
	typedef void (*callback_3)(D *data);

protected:
	D			*data = nullptr;
	std::map<C,BasicCharTree*>	nodes;
	callback_3		udata_free;

	void traverse(const std::basic_string<C>& path,callback_1 callback,BasicCharTree& root,void *udata);
	void optimize(const std::basic_string<C>& path,size_t tailcount,seglist_t& seglist,callback_2 callback,BasicCharTree& root,void *udata);

public:	BasicCharTree(callback_3 udata_free) : udata_free(udata_free) {}
	~BasicCharTree() { clear(); }
	
	BasicCharTree& put(const C *path,D *data);
	D *get(const C *path) const;
	D *get() { return data; }
	BasicCharTree& traverse(callback_1 callback,void *udata);
	BasicCharTree& optimize(callback_2 callback,void *udata);
	BasicCharTree& clear();
};

template<typename C,typename D>
BasicCharTree<C,D>&
BasicCharTree<C,D>::put(const C *path,D *datap) {
	C ch;
	BasicCharTree *np = this;
	
	if ( !path || !*path )
		return *this;

	while ( (ch = *path++) != C(0) ) {
		auto it = np->nodes.find(ch);
		if ( it == np->nodes.end() ) {
			BasicCharTree *p = new BasicCharTree(udata_free);
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
BasicCharTree<C,D>::get(const C *path) const {
	C ch;
	const BasicCharTree *np = this;
	
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
BasicCharTree<C,D>&
BasicCharTree<C,D>::traverse(void (*callback)(const std::basic_string<C>& path,BasicCharTree& tree,BasicCharTree& root,void *udata),void *udata) {
	std::basic_string<C> path;

	traverse(path,callback,*this,udata);
	return *this;
}

template<typename C,typename D>
void
BasicCharTree<C,D>::traverse(const std::basic_string<C>& path,callback_1 callback,BasicCharTree& root,void *udata) {

	for ( auto& pair : nodes ) {
		const C ch = pair.first;
		BasicCharTree<C,D> *np = pair.second;
		std::basic_string<C> tpath(path);
		tpath += ch;

		callback(tpath,*np,root,udata);
		np->traverse(tpath,callback,root,udata);
	}
}

template<typename C,typename D>
BasicCharTree<C,D>&
BasicCharTree<C,D>::optimize(callback_2 callback,void *udata) {
	std::basic_string<C> path;
	seglist_t seglist;

	optimize(path,0u,seglist,callback,*this,udata);
	return *this;
}

template<typename C,typename D>
void
BasicCharTree<C,D>::optimize(const std::basic_string<C>& path,size_t tailcount,seglist_t& seglist,callback_2 callback,BasicCharTree& root,void *udata) {
	seglist_t prefix(seglist);

	if ( nodes.size() > 1 && !path.empty() ) {
		std::basic_string<C> suffix;

		prefix.push_back(path.substr(path.size()-tailcount));
	}

	for ( auto& pair : nodes ) {
		const C ch = pair.first;
		BasicCharTree<C,D> *np = pair.second;
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
BasicCharTree<C,D>&
BasicCharTree<C,D>::clear() {
	
	if ( udata_free ) {
		if ( this->data ) {
			udata_free(this->data);
			this->data = nullptr;
		}
	}
	for ( auto& pair : nodes ) {
		pair.second->clear();
		delete pair.second;
	}
	nodes.clear();
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Typical Usage where Node key is type 'char':
//
//	typename D	Data type (data is allocated as D*)
//
//////////////////////////////////////////////////////////////////////

template<typename D>
using CharTree = BasicCharTree<char,D>;

#endif // CHARTREE_HPP

// End chartree.hpp
