#pragma once
#include <Transform.h>

#define COLLIDER_NAME_LEN 15


//Forward declartions of subclasses
//THESE ARE IMPORTANT
class SphereCollider;


//virtual class representing all collider shapes
// make at least sphere and cube subclasses

//colliders only collide with other collider shapes

//the touch struct provides details to the callee as to which collider was touched and how, returned as a linked list saved on the heap

// a logical process may query potential collisions in 3 ways: (no, 2)
	// collide(Collider*) do we touch a specific collider?
	// collide(int) show all touched colliders with user-defined flag(s) set? (flags are bits on a 4 byte integer, total of 32 flag slots)
		// to "OR" flags, simply call intersect(int) repeatedly with different flags, to "AND" flags, OR them as the int argument
		// OK so I'm not 100% on how this should work yet
	// collide() show all touched colliders, probably by calling	(THIS ONE COULD BE REPLACED BY COLLIDE(0)


// advanced entities may need a hierarchy of collider shapes, some colliders will therefor reference an enclosing collider "superCollider" (user-defined) that will be checked first
	//super colliders should be simple shapes, preferrably spheres
//NO NO NO WRONG, COLLIDER HIERARCHIES ARE FOR FINDING COLLISIONS FROM SUPER TO SUB, NOT THE OTHER WAY AROUND, FIX THIS
	//regardless, all shapes should start with simple checks to make sure the two colliders are closeby at all before calculating precise results

class Collider : public Transform
{
private:
	struct touch;	//forward declaration

	Transform* parentTransform = nullptr;	//WHENEVER a collision check is to be made, we need a reference to the parent's transform, to be combined with our own

	//The full list contains all colliders, sub- and supercolliders, used by FindByName function
	static Collider* fullList;	//init = nullptr
	Collider* fullNext = nullptr, * fullPrev = nullptr;		//internally managed double-linked-list of ALL colliders, starting with "firstCollider"
	//The Group list is a little more complicated: (used for query 2&3 of collide())
		//if the collider is at the TOP of a hierarchy, it is included in the list reffered to by groupList*, iterated through with groupNext and groupPrev, and superCollider is nullptr
		//when a subcollider linked, or vice versa, its own groupNext and groupPrev iterate through the superCollider's subColliderList of children
		//this hierarchy DOES NOT depend on each others transforms, they are managed individually
	static Collider* groupList;		//ini = nullptr
	Collider* groupNext = nullptr, * groupPrev = nullptr, * superCollider = nullptr, * subColliderList = nullptr;		//internally managed double-linked-list of colliders
	unsigned int tier = 0;		//tracks level in heirarchy to avoid circular dependencies

	//flags, in hierarchical collider structures, supercolliders should have the combined flags of all subcolliders, managed by constructors, may leave artifacts when destroyed
	int flags = 0x0;

	//internal function for safely setting name
	void setName(char* name);

	//helper function for recursive calls to modify flags of supercolliders
	void recursiveFlagSet(int flags);

	//helper function for constructors
	void construct(char* name, int flags, Collider* superCollider);

	touch* recursiveGroupCollideCheck(Collider* sourceCollider, int flags);

protected:
	char name[COLLIDER_NAME_LEN] = "no name";	//short name for user-friendly identification, use only to fetch reference once per user class, set in constructors
	//names should be unique, unnamed object cannot be found by other objects unless discovered via the 3rd touch query: all touched

	//Here are the specific, shape dependent, rules for intersect queries
	virtual touch* intersect(Collider* targetCollider) = 0;	//pure virtual, define in sub-classes

public:
	enum class colliderType { POINT, SPHERE, CUBE, CYLLINDER, CAPSULE, NONE};	//add a type here for each subclass of collider, none is reserved for error handling
	colliderType type = colliderType::NONE;	//type should be set in subclass' constructor
	struct touch
	{
		Collider* sourceCollider;
		Collider* targetCollider;
		bool newTouch;
		touch* next = nullptr;

		touch(Collider* sourceCollider, Collider* targetCollider, bool newTouch) : sourceCollider(sourceCollider), targetCollider(targetCollider), newTouch(newTouch) {}

		~touch()
		{
			delete next;
		}
	};

	Collider(Transform* parentTransform, char* name = nullptr, int flags = 0x0, Collider* superCollider = nullptr);
	Collider(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Transform* parentTransform, char* name = nullptr, int flags = 0x0, Collider* superCollider = nullptr);
	~Collider();

	//Here are the general rules for intersect queries
	//They return a pointer to a touch struct, possibly a list of such
	//nullptr implies NO collisions of the requested kind
	//REMEMBER TO DELETE THE LIST WHEN DONE WITH IT, delting the first element will delete the rest
	touch* collide(Collider* targetCollider);
	touch* collide(int flags);
	//touch* collide();	//use collide(0) instead
	
	static Collider* findByName(char* targetName);		//ok so what happens when a retrieved collider is later deleted elsewhere?
	void setSuperCollider(Collider* superCollider);
	void setFlags(int flags) { this->flags = flags; };
	int getFlags() { return flags; }



	//DECLARATTIONS OF INTERSECTION FUNCTIONS	(they are here in the superclass instead of subclass to avoid redefining each combination twice
	//most take in collider A and collider B, of specific subclasses of Collider, and a boolean (false if A is source or true if B is source)
	//the boolean argument is removed if both colliders are the same kind
	static touch* intersect(SphereCollider* source, SphereCollider* target);
};