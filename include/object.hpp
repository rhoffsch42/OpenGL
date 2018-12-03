#pragma once
#include "math.hpp"
#include "properties.hpp"

class Object
{
public:
	static unsigned int		getInstanceAmount();

	Object();
	Object(Properties object_pp);
	Object(const Object& src);
	~Object();
	Object&	operator=(const Object& src);

	void	runMothionBehavior(void* ptr);
	bool	update();
	virtual void	render(Math::Matrix4& PVmatrix);
	//mutators
	// void			setMotionBehavior(/*...*/);//useless if _motionBehavior is public
	void			setParent(Object* parent);
	//accessors
	unsigned int	getId(void) const;
	void			(*getMotionBehaviorFunc(void) const) (Object&, void*);
	Properties		getLocalProperties() const;
	Math::Matrix4&	getWorldMatrix() const;//dangerous
	Object*			getParent() const;

	//settings
	bool			_motionBehavior;
	//motion behavior
	void			(*_motionBehaviorFunc)(Object& ref, void* ptr);
	/*
		The static variables behavior must NOT depend on the object properties.
		As they do not belong to the Obj3d instance,
		its static variables will be altered by all instances using it.
	*/
	Properties		local;
	bool			_worldMatrixChanged;
protected:

	unsigned int	_id;
	Object*			_parent;
	Math::Matrix4	_worldMatrix;
	// Properties		_world;//pp trop complique a recuperer depuis la matrice finale
	// Program&		_program;
private:
	//duplicate this in Obj (amount, Obj3d amount will be included in Obj amount)
	static unsigned int	_instanceAmount;	// should never be altered manually
	static unsigned int	_instanceId;		// should never be altered manually
};