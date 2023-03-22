#include <Collider.h>
#include <Debug.h>


//TODO:
//	intersection functions
//	parent transforms may be require alterations of transform
class SphereCollider : public Collider	//inherits Transform, in turn
{

	//constructor notes
	//position should be provided by caller
	//rotation is irrelevant (unless we decide to include elliptical shapes (perhaps a different subclass?))
	//scale is equivalent with twice that of radius, the base sphere is contained by a 1x1x1 cardinal cube, multiplied by scale
	SphereCollider(glm::vec3 position, float radius, Transform* parentTransform, char* name = nullptr, int flags = 0x0, Collider* superCollider = nullptr)
		:Collider(position, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(radius*2.0f), parentTransform, name, flags, superCollider)
	{
		this->type = colliderType::SPHERE;
	}

	//Here are the specific, shape dependent, rules for intersect queries
	virtual touch* intersect(Collider* targetCollider) override
	{
		switch (targetCollider->type)
		{
		case colliderType::POINT:
			LOGERROR("Unimplemented or uncalled collision check (Sphere and Point), this function needs to be updated");
			break;

		case colliderType::SPHERE:
			return Collider::intersect(this, (SphereCollider*)targetCollider);
			break;

		case colliderType::CUBE:
			LOGERROR("Unimplemented or uncalled collision check (Sphere and Point), this function needs to be updated");
			break;

		case colliderType::CYLLINDER:
			LOGERROR("Unimplemented or uncalled collision check (Sphere and Point), this function needs to be updated");
			break;

		case colliderType::CAPSULE:
			LOGERROR("Unimplemented or uncalled collision check (Sphere and Point), this function needs to be updated");
			break;

		case colliderType::NONE:
			LOGALERT("Undefined collider type, defaulting to \"no collision\", all colliders should have their type defined in their constructors");
			break;

		default:
			LOGERROR("Unrecognised collider type, defaulting to \"no collision\", this function needs to be updated");
			break;
		}
		return nullptr;
	}
};