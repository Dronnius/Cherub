#include <RBTreeT.h>
#include <Debug.h>
#include <string>
#include <iostream>
#include <functional>

//RED-BLACK TREE
// ... how do they work again?
// remember that this represents a tree with triple branched nodes
// the red boolean indicates that the link to a given node, from the parent, is a "red" link
// "red" links imply that this node and its parent represents a single doubled node
// 
// Rules: (double lines imply red links, single lines imply black links)	(* marks the currently processed node)
// - Newly added nodes are always marked as having red links, the colour of the root node should be black
// - If both the left and right hand children of a node are red, set both to black, and this node to red
//		There should never be a situation where both children and the own node are red, as rules conflicts would arise before then.
//			 /							 //
//			B*							B*	
//		  // \\			--->		  /   \	
//		 A     C					 A     C
//		Note that in this example, the rule below should be applied to the new red link
// - A node's left hand child may NOT be red, only the right hand child may be.
//		Correct this issue by moving nodes around as such:
//				 /                  /
//				D*				   B		
//			  // \				  /	\\		
//		     B    E		--->	 A	  D*	
//			/ \						 / \
//         A   C					C   E
//		Keep in mind that the colours of D and B are swapped during the change, if both started red, both remain red, if D was black, B becomes black
//		It shouldn't be possible for the B to C link to be red, so don't worry about that (I'll add a warning though)
// - In the event of a chained red link, observed from node B in the example, which should only occur on the right handrearrange the nodes as such:
//				 /                           //
//				B*							D
//			  /   \\					  /   \
//		     A      D	       			 B*    F
//		     	   / \\			--->    / \   / \	
//                C    F			   A   C E   H
//		              / \
//                   E   H 	
//		In this example, the second rule will be applied to the new parent of D
//		It shouldn't be possible for the D to C link to be red, so don't worry about that (I'll add a warning though)
// - When a new node is added, check all of these rules at each of these steps while tracing back along the call stack
// 


//struct Node, defined in header file
//{
//	K key;
//	D* data = nullptr;
//	Node* left = nullptr;
//	Node* right = nullptr;
//	bool red = false;
//};
/*
template <typename K, typename D>
//recursively searches through the tree in order to modify it
//returns the possibly new root of the subtree, for the purpose of rebinding links, or nullptr if no new node was introduced
RBTreeT<K, D>::template Node* RBTreeT<K,D>::setTrace(RBTreeT<K, D>::Node* source, K targetKey, D* newData)
{
	//check if node key is lesser than the targetKey
	if (source->key < targetKey)		//if true, follow right pointer OR create new node and return
	{
		if (source->right == nullptr)	//if empty leaf
		{
			source->right = new RBTreeT<K, D>::Node(targetKey, newData);
			//proceed to corrections
		}
		else
		{
			Node* link = setTrace(source->right, targetKey, newData);
			if (link == nullptr)	//existing value changed
			{
				return nullptr;	//no corrections needed
			}
			else
			{
				source->right = link;	//rebind the left child in case it changed
				//proceed to corrections
			}
		}
	}
	//check if node key is greater than the targetKey
	else if (source->key > targetKey)	//if true, follow left pointer OR create new node and return
	{
		if (source->left == nullptr)	//if empty leaf
		{
			source->left = new RBTreeT<K, D>::Node(targetKey, newData);
			//proceed to corrections
		}
		else
		{
			Node* link = setTrace(source->left, targetKey, newData);
			if (link == nullptr)	//existing value changed
			{
				return nullptr;	//no corrections needed
			}
			else
			{
				source->left = link;	//rebind the left child in case it changed
				//proceed to corrections
			}
		}
	}
	
	else	//if neither, must be equal, set value and return
	{
		source->data = newData;
		return nullptr;	//no corrections needed
	}

	//--Corrections--
	//if this point is reached it means we're working backwards from the added node, and not including it
	//here is were we check the rules of the RBTreeT

	//unless i'm mistaken, only one rule can apply per node, therefor an if-else-if structure is used
	//rule one, if both children are red
	if (source->right != nullptr && source->left != nullptr && source->right->red && source->left->red)
	{
		if (source->red)
		{
			LOGWARNING("RBTreeT -> Red node's children are both red, indicates that something has gone wrong!");
		}
		source->red = true;
		source->right->red = false;
		source->left->red = false;
		return source;
	}
	//rule two, if left child is red
	else if (source->left != nullptr && source->left->red)
	{
		Node* errNode = source->left;		//copy pointer to left child
		//Node* midNode = errNode->right;	//temporary variable in case a node needs to switch parents
		errNode->red = source->red;			//copy over value of parent link
		source->left = errNode->right;		//link in midNode
		errNode->right = source;			//bind left child's left link to self
		source->red = true;					//new parent link (the one to current left child) is made red
		if (source->left->red)				//double check a potential issue
		{
			LOGWARNING("Midnode in RBTreeT rule 2 operation had red link to parent, there may be a bug in the system!");
		}
		return errNode;						//the previously erroneous node is now the top of the sub-tree, return a pointer to it to make things clear
	}
	//rule three, if right child of red right child is red
	else if (source->right != nullptr && source->right->red && source->right->right != nullptr && source->right->right->red)
	{
		Node* chaiNode = source->right;			//copy pointer to right child, the "chain-node"
		chaiNode->right->red = false;			//the chaiNode's right link is made black
		source->right = chaiNode->left;			//pass over the node caught in between
		chaiNode->left = source;				//link to self to reform tree structure
		//source->red = false;					//we're operating under the assumption that this is already black, if this is not true, the second warning message below will be tripped
		if (source->right->red)					//double check a potential issue
		{
			LOGWARNING("Chain-node's left child in RBTreeT rule 3 operation had red link to parent, there may be a bug in the system!");
		}
		if (source->red)						//double check a potential issue
		{
			LOGWARNING("The observed node in RBTreeT rule 3 operation had red link to parent, there may be a bug in the system!");
		}
		return chaiNode;						//the previously erroneous node is now the top of the sub-tree, return a pointer to it to make things clear
	}
	else
	{
		return source;
	}
}

template <typename K, typename D>
//recursively searches through the Red-Black Tree
//returns a pointer to the node, or nullptr if none was found
RBTreeT<K, D>::template Node* RBTreeT<K, D>::getTrace(RBTreeT<K, D>::Node* source, K targetKey)
{
	//check if node key is lesser than the targetKey
	if (source->key < targetKey)		//if true, follow right pointer OR create new node and return
	{
		if (source->right == nullptr)	//if empty leaf
		{
			return nullptr;		//node not found, return failure
		}
		else
		{
			return getTrace(source->right, targetKey);	//search in right sub-tree
		}
	}
	//check if node key is greater than the targetKey
	else if (source->key > targetKey)	//if true, follow left pointer OR create new node and return
	{
		if (source->left == nullptr)	//if empty leaf
		{
			return nullptr;		//node not found, return failure
		}
		else
		{
			return getTrace(source->left, targetKey);	//search in left sub-tree
		}
	}
	else	//if neither, must be equal, set return value
	{
		return source;	//node found, return pointer to it
	}
}

template <typename K, typename D>
//applies the given lambda expression to all nodes in the tree
//the provided lambda expression should return true if the lambda expression succeeds and false if it does not
//returns nullptr if successful
//returns a pointer to the node that failed if unsuccessful
RBTreeT<K, D>::template Node* RBTreeT<K, D>::fullTrace(RBTreeT<K, D>::Node* node, std::function<bool(RBTreeT<K, D>::Node*)> exp)
{
	if (node == nullptr)	//if there is no node here, return
	{
		return nullptr;
	}
	Node* result;
	//the right branch is greater, so start there
	if((result = fullTrace(node->right, exp)) != nullptr)	//if trace fails
	{
		return result;
	}
	//do own node
	if (!exp(node))			//if trace fails
	{
		return node;
	}
	//then left branch
	if ((result = fullTrace(node->left, exp)) != nullptr)	//if trace fails
	{
		return result;
	}
	return nullptr;		//everything OK!
}



//PUBLIC FUNCTIONS

template <typename K, typename D>
//constructor
RBTreeT<K, D>::RBTreeT()
{
	//nuthin'
}

template <typename K, typename D>
//destructor, takes care of the created tree by invoking the overridden delete operator of the Node struct
RBTreeT<K, D>::~RBTreeT()
{
	delete root;	//delete operator overridden in Node struct (root is a Node struct) so the whole tree should be deleted by this, I hope
}


template <typename K, typename D>
//add or modify element
//returns true if the node already existed and false if a new node was created
bool RBTreeT<K,D>::set(K key, D* data)
{
	if (this->root == nullptr)
	{
		root = new RBTreeT<K, D>::Node(key, data);
		root->red = false;
		return false;
	}
	else
	{
		void* result = setTrace(root, key, data);
		root->red = false;	//just to be safe
		return result == nullptr;
	}
}

template <typename K, typename D>
//find element by key, returns pointer to data
D* RBTreeT<K, D>::get(K key)
{
	return getTrace(root, key)->data;
}

template <typename K, typename D>
//remove data from target node, but does not remove the node from the tree, returns pointer to data removed from tree, because why not
D* RBTreeT<K, D>::mute(K key)
{
	Node* target = getTrace(root, key);
	D* oldData = target->data;
	target->data = nullptr;
	return oldData;
}


template <typename K, typename D>
//reconstructs the tree, ignoring muted nodes, call this after any series of mute calls, purges memory of old nodes afterward
void RBTreeT<K, D>::trim()
{
	Node* oldTree = root;		//save pointer to root (along with rest of tree
	root = nullptr;				//disconnect root
	fullTrace(oldTree,
		//lambda start---
		[=](RBTreeT<K, D>::Node* node)
		{
			if (node->data == nullptr)	//if node is muted, ignore
			{
				return true;
			}
			this->set(node->key, node->data);	//add to new tree
			return true;
		}
		//---lambda end
	);
	delete oldTree;			//remove the old tree, we don't need it anymore, this will purge any muted nodes
	return;
}


template <typename K, typename D>
//find element by data, returns a key that has matching data, slow function
RBTreeT<K, D>::template Keylist* RBTreeT<K, D>::fullSearch(D* data)
{
	Keylist* dummy = new Keylist((K) -1);
	Keylist* last = dummy;
	Node* instance = fullTrace(root,
		//lambda start---
		[&](RBTreeT<K, D>::Node* node)
		{
			if (node->data == data)
			{
				last->next = new Keylist(node->key);
				last = last->next;
			}
			return true;
		}
		//---lambda end
	);
	return dummy;
}


template <typename K, typename D>
//iteratre though list of keys returned by fullSearch, returns -1 when finished, destroys list while iterating. typically called like this: while((a = iterateKeylist) != -1)
K RBTreeT<K, D>::iterateKeylist(Keylist*& list)
{
	if (list == nullptr)
		return (K)-1;
	Keylist* old = list;
	list = list->next;	//iterate (yes, before retriving the key value) list is an alias, so this should make a change outside function
	delete old;			//destroy struct behind us, this is why the dummu header is important
	return list->key;	//return keyvalue
}

*/