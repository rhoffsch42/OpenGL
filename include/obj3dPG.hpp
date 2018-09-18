#pragma once

#include "program.hpp"
#include "obj3d.hpp"
class Obj3d;
#include "math.hpp"

class Obj3dPG : public Program
{
public:
	Obj3dPG(std::string vs_file, std::string fs_file);
	~Obj3dPG();
	void	linkBuffers(GLuint& vboVertex, GLuint& vboColor, GLuint& vboTexture) const;
	void	render(Obj3d& obj, Math::Matrix4 VPmatrix);//HAS to be cpy constructor! (1 PV*M per obj3d)
protected:
private:
	void	getLocations(void);
	
	//slots
	GLint		_mat4_mvp;
	GLint		_dismod;
	GLint		_plain_color;
	GLint		_tex_coef;
	GLint		_vertex_position_data;
	GLint		_vertex_color_data;
	GLint		_vertex_UV_data;
	//slots end
};
