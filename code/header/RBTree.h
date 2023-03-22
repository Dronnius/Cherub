#pragma once
#include <functional>

//simple aliases instead of templates
using key_t = int;
using data_t = void;


class RBTree
{
private:
	struct Node
	{
		key_t key;
		data_t* data = nullptr;
		Node* left = nullptr;
		Node* right = nullptr;
		bool red = true;

		//constructor
		Node(key_t key, data_t* data)
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
		key_t key;
		Keylist* next = nullptr;

		Keylist(key_t key)
		{
			this->key = key;
		}
	};

	//recursively searches through the tree in order to modify it
	//returns the possibly new root of the subtree, for the purpose of rebinding links, or nullptr if no new node was introduced
	Node* setTrace(Node* source, key_t targetKey, data_t* newData);

	//recursively searches through the Red-Black Tree
	//returns a pointer to the node, or nullptr if none was found
	Node* getTrace(Node* source, key_t targetKey);

	//applies the given lambda expression to all nodes in the tree
	//the provided lambda expression should return true if the lambda expression succeeds and false if it does not
	//arguments for lambda expression: Node*: pointer to current node, int, depth value of current node, 0 at root only
	//returns nullptr if successful
	//returns a pointer to the node that failed if unsuccessful
	Node* fullTrace(Node* node, std::function<bool(Node*, int)> exp, int depth = 0);

public:
	//constructor
	RBTree();
	//destructor, takes care of the created tree by invoking the overridden delete operator of the Node struct
	~RBTree();
	//add or modify element
	//returns true if the node already existed and false if a new node was created
	bool set(key_t key, data_t* data);
	//find element by key, returns pointer to data, or nullptr if none was found
	void* get(key_t key);
	//remove data from target node, but does not remove it from the tree, returns pointer to data removed from tree, because why not
	void* mute(key_t key);
	//reconstructs the tree, ignoring muted nodes, call this after any series of mute calls, purges memory of old nodes afterward
	void trim();
	//find element by data, returns a key that has matching data, slow function
	Keylist* fullSearch(data_t* data);
	//iteratre though list of keys returned by fullSearch, returns -1 when finished, destroys list while iterating. typically called like this: while((a = iterateKeylist) != -1)
	key_t iterateKeylist(Keylist*& list);
	//prints the contents of the tree using the standard Debug system, contents are listen from lowest to highest with indicators of depth and colour
	void printTree();
	//resets the tree to it's initial, empty state
	void empty();

	//unit test function
	static void debug_unitTest();
};