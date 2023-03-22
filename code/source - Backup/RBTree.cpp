#include <RBTree.h>
#include <Debug.h>
#include <string>
#include <iostream>
#include <functional>
#include <sstream>	//for stringstream used in printTree

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
//				data_t*				   B		
//			  // \				  /	\\		
//		     B    E		--->	 A	  data_t*	
//			/ \						 / \
//         A   C					C   E
//		Keep in mind that the colours of data_t and B are swapped during the change, if both started red, both remain red, if data_t was black, B becomes black
//		It shouldn't be possible for the B to C link to be red, so don't worry about that (I'll add a warning though)
// - In the event of a chained red link, observed from node B in the example, which should only occur on the right handrearrange the nodes as such:
//				 /                           //
//				B*							data_t
//			  /   \\					  /   \
//		     A      data_t	       			 B*    F
//		     	   / \\			--->    / \   / \	
//                C    F			   A   C E   H
//		              / \
//                   E   H 	
//		In this example, the second rule will be applied to the new parent of data_t
//		It shouldn't be possible for the data_t to C link to be red, so don't worry about that (I'll add a warning though)
// - When a new node is added, check all of these rules at each of these steps while tracing back along the call stack
// 


//struct Node, defined in header file
//{
//	key_t key;
//	data_t* data = nullptr;
//	Node* left = nullptr;
//	Node* right = nullptr;
//	bool red = false;
//};

//recursively searches through the tree in order to modify it
//returns the possibly new root of the subtree, for the purpose of rebinding links, or nullptr if no new node was introduced
RBTree::Node* RBTree::setTrace(RBTree::Node* source, key_t targetKey, data_t* newData)
{
	//check if node key is lesser than the targetKey
	if (source->key < targetKey)		//if true, follow right pointer OR create new node and return
	{
		if (source->right == nullptr)	//if empty leaf
		{
			//LOG("Added right child to " + std::to_string(source->key));		//DEBUG
			source->right = new RBTree::Node(targetKey, newData);
			//if (source->right == nullptr) { LOGALERT("\"NEW\" Command failed, right side"); }			//DEBUG
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
			//LOG("Added left child to " + std::to_string(source->key));		//DEBUG
			source->left = new RBTree::Node(targetKey, newData);
			//if (source->left == nullptr) { LOGALERT("\"NEW\" Command failed, left side"); }				//DEBUG
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
		//LOG("Correction A @" + std::to_string(source->key));		//DEBUG
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
		//LOG("Correction B @" + std::to_string(source->key));		//DEBUG
		Node* errNode = source->left;		//copy pointer to left child
		//Node* midNode = errNode->right;	//temporary variable in case a node needs to switch parents
		errNode->red = source->red;			//copy over value of parent link
		source->left = errNode->right;		//link in midNode
		errNode->right = source;			//bind left child's left link to self
		source->red = true;					//new parent link (the one to current left child) is made red
		if (source->left != nullptr && source->left->red)				//double check a potential issue
		{
			LOGWARNING("Midnode in RBTreeT rule 2 operation had red link to parent, there may be a bug in the system!");
		}
		return errNode;						//the previously erroneous node is now the top of the sub-tree, return a pointer to it to make things clear
	}
	//rule three, if right child of red right child is red
	else if (source->right != nullptr && source->right->red && source->right->right != nullptr && source->right->right->red)
	{
		//LOG("Correction C @" + std::to_string(source->key));		//DEBUG
		Node* chaiNode = source->right;			//copy pointer to right child, the "chain-node"
		chaiNode->right->red = false;			//the chaiNode's right link is made black
		source->right = chaiNode->left;			//pass over the node caught in between
		chaiNode->left = source;				//link to self to reform tree structure
		//source->red = false;					//we're operating under the assumption that this is already black, if this is not true, the second warning message below will be tripped
		if (source->right != nullptr && source->right->red)					//double check a potential issue
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

//recursively searches through the Red-Black Tree
//returns a pointer to the node, or nullptr if none was found
RBTree:: Node* RBTree::getTrace(RBTree::Node* source, key_t targetKey)
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

//applies the given lambda expression to all nodes in the tree
//the provided lambda expression should return true if the lambda expression succeeds and false if it does not
//arguments for lambda expression: Node*: pointer to current node, int, depth value of current node, 0 at root only
//returns nullptr if successful
//returns a pointer to the node that failed if unsuccessful
RBTree:: Node* RBTree::fullTrace(RBTree::Node* node, std::function<bool(RBTree::Node*, int)> exp, int depth)
{
	if (node == nullptr)	//if there is no node here, return
	{
		return nullptr;
	}
	Node* result;
	//the right branch is greater, so start there
	if((result = fullTrace(node->right, exp, depth + 1)) != nullptr)	//if trace fails
	{
		return result;
	}
	//do own node
	if (!exp(node, depth))			//if trace fails
	{
		return node;
	}
	//then left branch
	if ((result = fullTrace(node->left, exp, depth + 1)) != nullptr)	//if trace fails
	{
		return result;
	}
	return nullptr;		//everything OK!
}


//PUBLIC FUNCTIONS---

//constructor
RBTree::RBTree()
{
	//nuthin'
}

//destructor, takes care of the created tree by invoking the overridden delete operator of the Node struct
RBTree::~RBTree()
{
	delete root;	//delete operator overridden in Node struct (root is a Node struct) so the whole tree should be deleted by this, I hope
}


//add or modify element
//returns true if the node already existed and false if a new node was created
bool RBTree::set(key_t key, data_t* data)
{
	if (this->root == nullptr)
	{
		root = new RBTree::Node(key, data);
		root->red = false;
		return false;
	}
	else
	{
		Node* result = setTrace(root, key, data);
		if (result == nullptr)	//if existing value changed
		{
			return true;
		}
		else			//if a new node was introduced, result represents potentially new root, not the new node
		{
			root = result;	//rebind the root in case it changed
			root->red = false;	//just to be safe
			return false;
		}
	}
}

//find element by key, returns pointer to data
data_t* RBTree::get(key_t key)
{
	if (root == nullptr)
	{
		return nullptr;
	}
	Node* result = getTrace(root, key);
	if (result != nullptr)
	{
		return result->data;
	}
	else
	{
		return nullptr;
	}
}

//remove data from target node, but does not remove the node from the tree, returns pointer to data removed from tree, because why not
data_t* RBTree::mute(key_t key)
{
	Node* target = getTrace(root, key);
	if (target == nullptr)	//if key wasn't found
	{
		return nullptr;
	}
	data_t* oldData = target->data;
	target->data = nullptr;
	return oldData;
}

//reconstructs the tree, ignoring muted nodes, call this after any series of mute calls, purges memory of old nodes afterward
void RBTree::trim()
{
	Node* oldTree = root;		//save pointer to root (along with rest of tree
	root = nullptr;				//disconnect root
	fullTrace(oldTree,
		//lambda start---
		[=](RBTree::Node* node, int depth)
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


//find element by data, returns a key that has matching data, slow function
RBTree::Keylist* RBTree::fullSearch(data_t* data)
{
	Keylist* dummy = new Keylist((key_t) -1);
	Keylist* last = dummy;
	Node* instance = fullTrace(root,
		//lambda start---
		[&](RBTree::Node* node, int depth)
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


//iteratre though list of keys returned by fullSearch, returns -1 when finished, destroys list while iterating. typically called like this: while((a = iterateKeylist) != -1)
key_t RBTree::iterateKeylist(Keylist*& list)
{
	if (list == nullptr)
		return (key_t)-1;
	Keylist* old = list;
	list = list->next;	//iterate (yes, before retriving the key value) list is an alias, so this should make a change outside function
	delete old;			//destroy struct behind us, this is why the dummu header is important
	return list->key;	//return keyvalue
}

void RBTree::printTree()
{
	LOG("\tSTARTED PRINTING TREE---");
	fullTrace(root,
		//lambda start---
		[](RBTree::Node* node, int depth)
		{
			std::stringstream ss;
			if (node->red)
			{
				ss << "RED node: ";
			}
			else
			{
				ss << "BLACK node: ";
			}
			ss << "\tDepth = " << depth << " \t\tKey = " << node->key << " \t\tData = ..." << std::hex << ((long)node->data & 0xff) << std::dec;
			if (node->left != nullptr)
			{
				ss << " \t\tL.Child = " << node->left->key;
			}
			else
			{
				ss << " \t\tNo Left Child";
			}
			if (node->right != nullptr)
			{
				ss << " \t\tR.Child = " << node->right->key;
			}
			else
			{
				ss << " \t\tNo Right Child";
			}
			LOG(ss.str());
			return true;
		});
		//---lambda end
	LOG("\t---DONE PRINTING TREE");
}


//resets the tree to it's initial, empty state
void RBTree::empty()
{
	delete root;
	root = nullptr;
}


void RBTree::debug_unitTest()
{
	RBTree tree = RBTree();
	tree.printTree();
	short keySeed = 0x7be5;
	short prev = 0;
	short prevPrev = 0;

	data_t* data = &keySeed;
	std::stringstream ss;
	for (int i = 0; i < 10; i++)
	{
		ss << std::dec << "Setting key: " << (0xff & keySeed) << std::hex << "\t\tdata: " << ((int)data & 0xff);
		LOG(ss.str());
		ss.str("");	//clear stream
		tree.set((0xff & keySeed), data);

		data_t* d = tree.get(prev);
		ss << std::dec << "Getting key:" << prev << std::hex << "\t\tdata: " << ((int)d & 0xff);
		LOG(ss.str());
		ss.str("");	//clear stream
		d = tree.get(prevPrev);
		ss << std::dec << "Getting key:" << prevPrev << std::hex << "\t\tdata: " << ((int)d & 0xff);
		LOG(ss.str());
		ss.str("");	//clear stream
		prevPrev = prev;
		prev = (0xff & keySeed);


		tree.printTree();
		data = (void*)((int)data + 1);
		keySeed = ((short)(keySeed * keySeed) + 53)%253;
	}
	ss << std::hex << "removing nodes with data: ...";
	data = tree.mute((0xff & keySeed));
	ss << ((int)data & 0xff) << ", ...";
	data = tree.mute(prev);
	ss << ((int)data & 0xff) << " and ...";
	data = tree.mute(prevPrev);
	ss << ((int)data & 0xff);
	ss << "\n\tTrimming...";
	LOG(ss.str());
	tree.trim();
	tree.printTree();
}