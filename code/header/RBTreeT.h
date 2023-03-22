#pragma once
#include <functional>
#include <Debug.h>

template <typename K, typename D>
class RBTreeT
{
private:
	struct Node
	{
		K key;
		D* data = nullptr;
		Node* left = nullptr;
		Node* right = nullptr;
		bool red = true;

		//constructor
		Node(K key, D* data)
		{
			this->key = key;
			this->data = data;
		}

		//deleting this Node will remove all sub-nodes aswell
		//apparently delete nullptr is safe, and doesn't do anything, so we don't need to check for it: https://stackoverflow.com/questions/4190703/is-it-safe-to-delete-a-null-pointer
		~Node()
		{
			delete left;
			delete right;
			//data is external to the Red-black tree, and therefor not deleted here
		}
	};
	Node* root = nullptr;

	struct Keylist	//this is a list struct dedicated to the "fullSeach()" function, it is a list of keycodes that corresponds to 
	{
		K key;
		Keylist* next = nullptr;

		Keylist(K key)
		{
			this->key = key;
		}
	};

	//creates a new node and sets the provided pointer to point toward it
	//void addNode(Node*& parentPointer, K key, D* data);	replace with: parentPointer = new RBTreeT<K, D>::Node(key, data);
	//recursively searches through the tree in order to modify it
	//returns the possibly new root of the subtree, for the purpose of rebinding links, or nullptr if no new node was introduced
	RBTreeT<K, D>::Node* setTrace(Node* source, K targetKey, D* newData);

	//recursively searches through the Red-Black Tree
	//returns a pointer to the node, or nullptr if none was found
	RBTreeT<K, D>::Node* getTrace(Node* source, K targetKey);

	//applies the given lambda expression to all nodes in the tree
	//the provided lambda expression should return true if the lambda expression succeeds and false if it does not
	//returns nullptr if successful
	//returns a pointer to the node that failed if unsuccessful
	RBTreeT<K, D>::Node* fullTrace(RBTreeT<K, D>::Node* node, std::function<bool(RBTreeT<K, D>::Node*)> exp);

public:
	//constructor
	RBTreeT();
	//destructor, takes care of the created tree by invoking the overridden delete operator of the Node struct
	~RBTreeT();
	//add or modify element
	//returns true if the node already existed and false if a new node was created
	bool set(K key, D* data);
	//find element by key, returns pointer to data, or nullptr if none was found
	D* get(K key);
	//remove data from target node, but does not remove it from the tree, returns pointer to data removed from tree, because why not
	D* mute(K key);
	//reconstructs the tree, ignoring muted nodes, call this after any series of mute calls, purges memory of old nodes afterward
	void trim();
	//find element by data, returns a key that has matching data, slow function
	Keylist* fullSearch(D* data);
	//iteratre though list of keys returned by fullSearch, returns -1 when finished, destroys list while iterating. typically called like this: while((a = iterateKeylist) != -1)
	K iterateKeylist(Keylist*& list);
};

//IMPLEMENTATION BELOW, DEFINITIONS HAVE TO BE IN HEADER FILE BECAUSE TEMPLATES ARE FUNKY LIKE THAT

template <typename K, typename D>
//recursively searches through the tree in order to modify it
//returns the possibly new root of the subtree, for the purpose of rebinding links, or nullptr if no new node was introduced
RBTreeT<K, D>::template Node* RBTreeT<K, D>::setTrace(RBTreeT<K, D>::Node* source, K targetKey, D* newData)
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
	if ((result = fullTrace(node->right, exp)) != nullptr)	//if trace fails
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
bool RBTreeT<K, D>::set(K key, D* data)
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
	Keylist* dummy = new Keylist((K)-1);
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