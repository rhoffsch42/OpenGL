#include "simplegl.h"
#include "behavior.hpp"

bool	Behavior::areActive = true;

Behavior::Behavior() {
	cout << "_ Behavior cons" << endl;
	this->isActive = true;
}

Behavior::Behavior(const Behavior& src) {
	cout << "_ Behavior cons by copy" << endl;
	*this = src;
}

Behavior&   Behavior::operator=(const Behavior& src) {
	cout << "_ Behavior operator=" << endl;
	this->isActive = src.isActive;
	this->targetList = src.getTargetList();
	return (*this);
}

Behavior::~Behavior() {
	cout << "_ Behavior des" << endl;
	//need to delete/empty list?
}

void	Behavior::removeTarget(const Object& target) {
/*
	lambda expression:
	https://stackoverflow.com/questions/17965728/find-a-pair-in-a-stl-list-where-only-first-element-is-known
	remove_if:
	https://stackoverflow.com/questions/22729906/stdremove-if-not-working-properly/22729987#22729987
	it just move the eligible one to the end, and return a pointer to the start of the moved stuff
*/
	this->targetList.erase(std::remove_if(this->targetList.begin(), this->targetList.end(),
		[&target](std::pair<Object&, bool> elem) { return (&elem.first == &target); }),
		this->targetList.end());
}

void	Behavior::setTargetStatus(const Object& target, bool status) {
	auto it = std::find_if(this->targetList.begin(), this->targetList.end(),
		[&target](std::pair<Object&, bool> elem) { return (&elem.first == &target); });
	(*it).second = status;
}
//accessor
std::list< std::pair<Object&, bool> >	Behavior::getTargetList() const { return (this->targetList); }