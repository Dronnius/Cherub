#include <Collider.h>
#include <cstring>

Collider* Collider::fullList = nullptr;
Collider* Collider::groupList = nullptr;



//internal function for safely setting name
void Collider::setName(char* name)
{
	if (name == nullptr)
		return;
	for (int i = 0; i < COLLIDER_NAME_LEN; i++)
	{
		this->name[i] = name[i];
	}
	//cutoff
	this->name[COLLIDER_NAME_LEN - 1] = '\0';
	return;
}


//helper function for recursive calls to modify flags of supercolliders
void Collider::recursiveFlagSet(int flags)
{
	this->setFlags(this->getFlags() | flags);	//add flags to self
	if (this->superCollider != nullptr)	//if supercollider exists
	{
		this->superCollider->recursiveFlagSet(flags);	//recursive call
		//since we are OR-ing we should only need to send in the new flags
	}
}

void Collider::construct(char* name, int flags, Collider* superCollider)
{
	setName(name);
	//add to full list
	if (Collider::fullList == nullptr)	//first collider
	{
		Collider::fullList = this;
	}
	else	//successive collider
	{
		Collider* fullListIterator = Collider::fullList;
		while (fullListIterator->fullNext != nullptr)
		{
			fullListIterator = fullListIterator->fullNext;
		}
		fullListIterator->fullNext = this;
		this->fullPrev = fullListIterator;
	}
	//add correctly and VERY carefully to group list
	if (superCollider == nullptr)	//no supercollider, add to static groupList
	{
		if (Collider::groupList == nullptr)	//first collider
		{
			Collider::fullList = this;
		}
		else	//successive collider
		{
			Collider* groupListIterator = Collider::groupList;
			while (groupListIterator->groupNext != nullptr)
			{
				groupListIterator = groupListIterator->groupNext;
			}
			groupListIterator->groupNext = this;
			this->groupPrev = groupListIterator;
		}
		//set tier
		this->tier = 0;
	}
	else	//child collider, add to supercollider's sublist
	{
		if (superCollider->subColliderList == nullptr)	//first subcollider
		{
			superCollider->subColliderList = this;
		}
		else	//successive collider
		{
			Collider* subGroupListIterator = superCollider->subColliderList;
			while (subGroupListIterator->groupNext != nullptr)
			{
				subGroupListIterator = subGroupListIterator->groupNext;
			}
			subGroupListIterator->groupNext = this;
			this->groupPrev = subGroupListIterator;
		}
		//set tier
		this->tier = this->superCollider->tier + 1;
	}
	//set flags, potentially add flags to supercolliders
	this->recursiveFlagSet(flags);
}

Collider::Collider(Transform* parentTransform, char* name, int flags, Collider* superCollider) : parentTransform(parentTransform)
{
	construct(name, flags, superCollider);
}

Collider::Collider(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Transform* parentTransform, char* name, int flags, Collider* superCollider) : Transform(position, rotation, scale), parentTransform(parentTransform)
{
	construct(name, flags, superCollider);
}

//will also delete all subcolliders assigned to it
Collider::~Collider()
{
	//relink full collider list
	if (Collider::fullList == this)	//this is the first collider
	{
		Collider::fullList = this->fullNext;
	}
	if (this->fullNext != nullptr)
	{
		this->fullNext->fullPrev = this->fullPrev;
	}
	if (this->fullPrev != nullptr)
	{
		this->fullPrev->fullNext = this->fullNext;
	}

	//relink group collider list
	if (Collider::groupList == this)	//this is the first supercollider
	{
		Collider::groupList = this->groupNext;
	}
	if (this->superCollider != nullptr && this->superCollider->subColliderList == this)	//superCollider pointer can remain, but potential list pointers to this CANNOT
	{
		this->superCollider->subColliderList = this->groupNext;
	}
	if (this->groupNext != nullptr)
	{
		this->groupNext->groupPrev = this->groupPrev;
	}
	if (this->groupPrev != nullptr)
	{
		this->groupPrev->groupNext = this->groupNext;
	}

	//finally, delete any subcolliders
	while (this->subColliderList != nullptr)
	{
		delete this->subColliderList; //deleting this will modify the head of the list (in this very function, up a bit), therefor the while-loop should work
	}
}

//private recursive function
//checks collision on this collider and then all subcolliders (recursively)
Collider::touch* Collider::recursiveGroupCollideCheck(Collider* sourceCollider, int flags)
{
	//check for self-collision
	if (sourceCollider == this)	//never collide with own collider, or subcolliders of self
	{
		return nullptr;
	}
	//check flags
	if ((this->flags & flags) != flags)	//flag mismatch
	{
		return nullptr;
	}
	//check self
	touch* chain = sourceCollider->intersect(this);
	if (chain == nullptr)	//no collision, don't check subcolliders
	{
		return nullptr;
	}
	touch* chainStart = chain;	//copy first element in chain

	//check subcolliders
	Collider* list = this->subColliderList;
	while (list != nullptr)
	{
		chain->next = recursiveGroupCollideCheck(list, flags);	//attach colliding subcolliders to chain
		while (chain->next != nullptr)	//"fast forward" to end of chain
		{
			chain = chain->next;
		}
		list = list->groupNext;
	}
	return chainStart;
}


//Here are the general rules for intersect queries
//They return a pointer to a touch struct, possibly a list of such, or nullptr if no collision is detected
//There is no need to check supercolliders, as all colliders should do simple checks (such as distance between centers) first
Collider::touch* Collider::collide(Collider* targetCollider)
{
	return intersect(targetCollider);	//simple enough
}

//to check everything, use flags = 0x0
Collider::touch* Collider::collide(int flags)
{
	Collider* list = Collider::groupList;
	touch dud(nullptr, nullptr, false);	//simple dummy that will be removed on return due to stack-allocation
	touch* chain = &dud;
	while (list != nullptr)
	{
		chain->next = recursiveGroupCollideCheck(this, flags);	//attach colliding subcolliders to chain
	
		while (chain->next != nullptr)	//"fast forward" to end of chain
		{
			chain = chain->next;
		}
		list = list->groupNext;
	}
	return dud.next;	//disconnect dummy by returning the next element
}

/*	//replace with collide(0)
Collider::touch* Collider::collide()
{
	
}
*/

//use only once per object to retrieve references
//returns a pointer to the first found collider with a matching name, or nullptr if nothing is found
Collider* Collider::findByName(char* targetName)
{
	Collider* iterator = Collider::fullList;
	while (iterator != nullptr)
	{
		if ((strcmp(iterator->name, targetName) == 0) && (strcmp(iterator->name, "no name") != 0))
		{
			return iterator;
		}
		iterator = iterator->fullNext;
	}
	return nullptr;
}