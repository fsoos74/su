#ifndef TREE2D_H
#define TREE2D_H

#include<vector>
#include<stdexcept>
#include<cstdint>
#include<limits>


template<typename COORD, typename VALUE>
class Tree2D{

public:
	
	struct Node{

		typedef uint32_t  index_type;
		static const index_type NULL_INDEX = std::numeric_limits<index_type>::max();

		Node( COORD x=COORD(0), COORD y=COORD(0), VALUE value=VALUE(0),
			index_type l=NULL_INDEX, index_type r=NULL_INDEX, 
			index_type a=NULL_INDEX, index_type b=NULL_INDEX):
			x(x), y(y), value(value), left(l), right(r), above(a), below(b){
		}

		~Node(){
		}

		COORD x;
		COORD y;
		VALUE value;

		index_type left;
		index_type right;
		index_type above;
		index_type below;
	};

	typedef typename std::vector<Node>::iterator iterator;

	Tree2D();
	~Tree2D();

	iterator begin(){ return nodes.begin(); }
	iterator end(){ return nodes.end(); }

	void add( COORD x, COORD y, VALUE value );
	bool contains( COORD x, COORD y)const;
	VALUE value( COORD x, COORD y)const;
	
private:

	typename Node::index_type find( COORD x, COORD y)const;

	std::vector< Node > nodes;
};


template<typename COORD, typename VALUE>
Tree2D<COORD, VALUE>::Tree2D():nodes(1){	// create root node
}

template<typename COORD, typename VALUE>
Tree2D<COORD, VALUE>::~Tree2D(){
}

template<typename COORD, typename VALUE>
void Tree2D<COORD, VALUE>::add( COORD x, COORD y, VALUE value){

	typename Node::index_type root=0;
 
	while( root!=Node::NULL_INDEX ){
		
		if( x < nodes[root].x ){
			if( nodes[root].left != Node::NULL_INDEX ){
				root=nodes[root].left;
				continue;
			}
			else{
				nodes[root].left=nodes.size();
				nodes.push_back( Node( x, y, value ) );				
				return;
			}
		}

		if( x > nodes[root].x ){
			if( nodes[root].right != Node::NULL_INDEX ){
				root=nodes[root].right;
				continue;
			}
			else{
				nodes[root].right=nodes.size();
				nodes.push_back( Node( x, y, value ) );
				return;
			}
		}

		// x == nodes[root].x

		if( y < nodes[root].y ){
			if( nodes[root].above != Node::NULL_INDEX ){
				root=nodes[root].above;
				continue;
			}
			else{
				nodes[root].above=nodes.size();
				nodes.push_back( Node( x, y, value ) );
				return;
			}
		}

		if( y > nodes[root].y ){
			if( nodes[root].below != Node::NULL_INDEX ){
				root=nodes[root].below;
				nodes.push_back( Node( x, y, value ) );
				continue;
			}
			else{
				nodes[root].below=nodes.size();
				nodes.push_back( Node( x, y, value ) );
				return;
			}
		}

		// y == nodes[root].y

		break;	// node at x,y exists

	}

	throw std::runtime_error("Tree2D adding node failed!");			
}

template<typename COORD, typename VALUE>
bool Tree2D<COORD, VALUE>::contains( COORD x, COORD y)const{
	return find( x, y ) != Node::NULL_INDEX;
}

template<typename COORD, typename VALUE>
VALUE Tree2D<COORD, VALUE>::value( COORD x, COORD y)const{
	typename Node::index_type idx = find( x, y );
	if( idx==Node::NULL_INDEX ) 
		throw std::runtime_error("Tree2D::value(): no such value!");
	return nodes[idx].value;
}


template<typename COORD, typename VALUE>
typename Tree2D<COORD, VALUE>::Node::index_type Tree2D<COORD, VALUE>::find( COORD x, COORD y)const{

	typename Node::index_type root=0;

	while( root!=Node::NULL_INDEX ){

		if( x==nodes[root].x && y==nodes[root].y ) break;

		if( x < nodes[root].x ){
			root=nodes[root].left;
			continue;
		}

		if( x > nodes[root].x ){
			root=nodes[root].right;
			continue;
		}

		if( y < nodes[root].y ){
			root=nodes[root].above;
			continue;
		}

		if( y > nodes[root].y ){
			root=nodes[root].below;
			continue;
		}

	}


	return root;
}

#endif
