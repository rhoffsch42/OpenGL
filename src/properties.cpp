#include "simplegl.h"
#include "properties.hpp"

bool			Properties::defaultCentered = false;
float			Properties::defaultSize = PP_DEFAULT_SIZE;
bool			Properties::defaultRescaled = true;

Properties::Properties() : _scaleCoef(1.0f) {
	// cout << "_ Properties cons" << endl;
	this->_scale = Math::Vector3(1, 1, 1);
	this->_finalScale = this->_scale;
	this->_finalScale.mult(this->_scaleCoef);
	this->_rescaled = Properties::defaultRescaled;
	this->_matrixUpdated = false;
}

Properties::Properties(float scaleCoef) : _scaleCoef(scaleCoef) {
	// cout << "_ Properties cons" << endl;
	this->_scale = Math::Vector3(1, 1, 1);
	this->_finalScale = this->_scale;
	this->_finalScale.mult(this->_scaleCoef);
	this->_rescaled = Properties::defaultRescaled;
	this->_matrixUpdated = false;
}

Properties::Properties(const Properties& src) : _scaleCoef(src.getScaleCoef()) {
	// cout << "_ Properties cons by copy" << endl;
	*this = src;
}

Properties&		Properties::operator=(const Properties& src) {
	this->_matrix = Math::Matrix4(src.getMatrix());
	this->_matrixUpdated = false;
	this->_pos = src.getPos();
	this->_rot = src.getRot();
	this->_scale = src.getScale();
	this->_finalScale = src.getFinalScale();
	this->_rescaled = src.isRescaled();
	this->_centerOffset = src.getCenterOffset();
	return (*this);
}

Properties::~Properties() {
	cout << "_ Properties des" << endl;
}

void		Properties::center() {
	//a faire a chaque changement de pos/rot/scale
/*
	https://gamedev.stackexchange.com/questions/59843/rotating-an-object-when-the-center-in-not-the-origin-opengl
	Easy way of building the rotation matrix :
		1	Start with an identity matrix
		2	Translate the matrix by - centre of the object
		3	Rotate the matrix by the desired amount
		4	Translate the matrix by centre of the object
		5	Use the resulting matrix to transform the object that you desire to rotate
*/
	Math::Vector3	newpos = this->_pos;
	Math::Vector3	offset = this->_centerOffset;
	Math::Vector3&	usedScale = this->_rescaled ? this->_finalScale : this->_scale;
	offset.x *= usedScale.x;
	offset.y *= usedScale.y;
	offset.z *= usedScale.z;
	Math::Vector3	offsetneg(-offset.x, -offset.y, -offset.z);
	offsetneg.rotate(this->_rot, ROT_WAY);
	newpos.add(offsetneg);//stocker dans this->_posCentered ?
	this->_matrix.updatePosValue(newpos);
}

//relative mutators
void		Properties::translate(float x, float y, float z) {
	// this->_pos.operation(x, y, z);//cf .hpp
	this->_pos.x += x;
	this->_pos.y += y;
	this->_pos.z += z;
	if (this->centered)
		this->center();
	else
		this->_matrix.updatePosValue(this->_pos);
}
void		Properties::translate(Math::Vector3 pos) {
	// this->_pos.operation(Math::Vector3);//cf .hpp
	this->_pos.add(pos);
	if (this->centered)
		this->center();
	else
		this->_matrix.updatePosValue(this->_pos);
}
void		Properties::rotate(float x, float y, float z) {// in degree!
	// this->_rot.operation(x, y, z);//cf .hpp
	if (this->_rot.isRadian()) {
		x = Math::toRadian(x);
		y = Math::toRadian(y);
		z = Math::toRadian(z);
	}
	this->_rot.x += x;
	this->_rot.y += y;
	this->_rot.z += z;
	this->_matrixUpdated = false;
}
void		Properties::rotate(Math::Rotation rot) {
	// this->_rot.operation(Math::Vector3);//cf .hpp
	if (rot.isRadian() != this->_rot.isRadian()) {
		rot.setAsRad();
		this->_rot.setAsRad();
	}
	this->_rot.add(rot);
	this->_matrixUpdated = false;
}
void		Properties::enlarge(float x, float y, float z) {
	// this->_scale.operation(x, y, z);//cf .hpp
	this->_scale.x += x;
	this->_scale.y += y;
	this->_scale.z += z;
	this->_matrixUpdated = false;
}
void		Properties::enlarge(Math::Vector3 scale) {
	// this->_scale.operation(Math::Vector3);//cf .hpp
	this->_scale.add(scale);
	this->_matrixUpdated = false;
}
//mutators
void		Properties::setPos(float x, float y, float z) {
	this->_pos.x = x;
	this->_pos.y = y;
	this->_pos.z = z;
	if (this->centered)
		this->center();
	else
		this->_matrix.updatePosValue(this->_pos);
}
void		Properties::setPos(Math::Vector3 pos) {
	this->_pos = pos;
	if (this->centered)
		this->center();
	else
		this->_matrix.updatePosValue(this->_pos);
}
void		Properties::setRot(float x, float y, float z) {// in degree!
	this->_rot.x = x;
	this->_rot.y = y;
	this->_rot.z = z;
	this->_rot.setUnit(ROT_DEG);
	this->_matrixUpdated = false;
}
void		Properties::setRot(Math::Rotation rot) {
	this->_rot = rot;
	this->_matrixUpdated = false;
}
void		Properties::setScale(float x, float y, float z) {
	this->_scale.x = x;
	this->_scale.y = y;
	this->_scale.z = z;
	this->_finalScale = this->_scale;
	this->_finalScale.mult(this->_scaleCoef);
	this->_matrixUpdated = false;
}
void		Properties::setScale(Math::Vector3 scale) {
	this->_scale = scale;
	this->_finalScale = this->_scale;
	this->_finalScale.mult(this->_scaleCoef);
	this->_matrixUpdated = false;
}
void		Properties::setRescaled(bool value) {
	//check to avoid useless calculation 
	if (this->_rescaled != value) {
		this->_rescaled = value;
		this->_matrixUpdated = false;
	}
}

//accessors
Math::Matrix4&	Properties::getMatrix(void) const { return ((Math::Matrix4&)this->_matrix); }
Math::Vector3	Properties::getPos(void) const { return (this->_pos); }
Math::Rotation	Properties::getRot(void) const { return (this->_rot); }
Math::Vector3	Properties::getScale(void) const { return (this->_scale); }
Math::Vector3	Properties::getFinalScale(void) const { return (this->_finalScale); }
float			Properties::getScaleCoef(void) const { return (this->_scaleCoef); }
bool			Properties::isRescaled() const { return (this->_rescaled); }
Math::Vector3	Properties::getCenterOffset(void) const { return (this->_centerOffset); }
