/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhoffsch <rhoffsch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/18 22:45:30 by rhoffsch          #+#    #+#             */
/*   Updated: 2018/12/03 11:17:48 by rhoffsch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simplegl.h"

#include "program.hpp"
#include "object.hpp"
#include "obj3dPG.hpp"
#include "obj3dBP.hpp"
#include "obj3d.hpp"
#include "misc.hpp"
#include "cam.hpp"
#include "texture.hpp"
#include "skyboxPG.hpp"
#include "skybox.hpp"
#include "glfw.hpp"

#include <string>
#include <cstdio>
#include <vector>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

class Fps
{
public:
	Fps(int fps_val) {
		this->fps = fps_val;
		this->old_fps = fps_val;
		this->tick = 1.0 / this->fps;
		this->last_time = glfwGetTime();
		this->ellapsed_time = 0.0;
	}
	double				fps;
	double				old_fps;
	double				tick;
	double				ellapsed_time;
	double				last_time;

	bool		wait_for_next_frame() {
		this->ellapsed_time = glfwGetTime() - this->last_time;
		if (this->ellapsed_time >= this->tick)
		{
			this->last_time += this->ellapsed_time;
			this->ellapsed_time = 0.0;
			return (true);
		}
		else
			return (false);
	}
};

//tmp
void	demonstrate_scale_in_matrix(Object& obj) {//and try to extract euler angles
	// this show that the elements (concerned by rot) are multiplied by the scale
	// depending of scale xyz

	Math::Vector3	XX;
	Math::Vector3	YY;
	Math::Vector3	ZZ;

	cout << "test scale in matrix : ---------------------" << endl;
	obj.local.setRot(47, 123.0f, 220);
	obj.local.setScale(1, 1, 1);
	obj.update();
	Math::Matrix4&	mat1 = obj.getWorldMatrix();
	mat1.setOrder(ROW_MAJOR);
	// mat1.setOrder(COLUMN_MAJOR);
	
	mat1.printData();
	XX.x = mat1.tab[0][0];
	XX.y = mat1.tab[1][0];
	XX.z = mat1.tab[2][0];
	YY.x = mat1.tab[0][1];
	YY.y = mat1.tab[1][1];
	YY.z = mat1.tab[2][1];
	ZZ.x = mat1.tab[0][2];
	ZZ.y = mat1.tab[1][2];
	ZZ.z = mat1.tab[2][2];
	cout << "magnitude: " << XX.magnitude()/10.0f << " " << YY.magnitude()/10.0f << " " << ZZ.magnitude()/10.0f << endl;
	cout << endl;


	cout << endl;

	obj.local.setScale(12.2, 26.1, 32.8);
	obj.update();
	Math::Matrix4&	mat = obj.getWorldMatrix();
	mat.setOrder(ROW_MAJOR);
	// mat.setOrder(COLUMN_MAJOR);

	mat.printData();
	XX.x = mat.tab[0][0];
	XX.y = mat.tab[1][0];
	XX.z = mat.tab[2][0];
	YY.x = mat.tab[0][1];
	YY.y = mat.tab[1][1];
	YY.z = mat.tab[2][1];
	ZZ.x = mat.tab[0][2];
	ZZ.y = mat.tab[1][2];
	ZZ.z = mat.tab[2][2];
	cout << "magnitude: " << XX.magnitude()/10.0f << " " << YY.magnitude()/10.0f << " " << ZZ.magnitude()/10.0f << endl;
	cout << endl;
	// why scale is magnitude / 10 ?


	exit(0);
	//extract euler angles	
	Math::Rotation euler;
	euler.setUnit(ROT_RAD);
	if (0) {
		mat.setOrder(ROW_MAJOR);
		euler.x = atan2(mat.tab[1][2], mat.tab[2][2]);
		float c2 = sqrtf(powf(mat.tab[0][0], 2) + powf(mat.tab[0][1], 2));
		euler.y = atan2(-mat.tab[0][2], c2);
		float s1 = sinf(euler.x);
		float c1 = cosf(euler.x);
		euler.z = atan2( s1*mat.tab[2][0] - c1*mat.tab[1][0],
		 				 c1*mat.tab[1][1] - s1*mat.tab[2][1] );
	}

	if (1) {
		mat.setOrder(COLUMN_MAJOR);
		float sy = sqrt(powf(mat.tab[0][0], 2) + powf(mat.tab[0][1], 2));
		bool singular = sy < 1e-6; // If
		if (!singular)
		{
			euler.x = atan2(mat.tab[2][1], mat.tab[2][2]);
			// x = atan2(R.at<double>(2,1) , R.at<double>(2,2));
			euler.y = atan2(mat.tab[2][0], sy);
			// y = atan2(-R.at<double>(2,0), sy);
			euler.z = atan2(mat.tab[1][0], mat.tab[0][0]);
			// z = atan2(R.at<double>(1,0), R.at<double>(0,0));
		}
		else
		{
			euler.x = atan2(mat.tab[1][2], mat.tab[1][1]);
			// x = atan2(-R.at<double>(1,2), R.at<double>(1,1));
			euler.y = atan2(mat.tab[2][0], sy);
			// y = atan2(-R.at<double>(2,0), sy);
			euler.z = 0;
		}
	}

	euler.printData();
	cout << endl;
	euler.setAsDegree();
	euler.printData();
	exit(0);

/*
	161.80 -22.74 251.60 0.00
	-210.69 -177.00 119.50 -400.00
	139.39 -241.15 -111.43 0.00
	0.00 0.00 0.00 1.00

	5.39 -0.76 8.39 0.00
	-7.02 -5.90 3.98 -400.00
	4.65 -8.04 -3.71 0.00
	0.00 0.00 0.00 1.00
*/		

}

void	printFps() {
	static double last_time = 0;
	static double ellapsed_time = 0;
	double	current_time;
	double	fps;
	double	cent;
	current_time = glfwGetTime();
	ellapsed_time = current_time - last_time;
	fps = 1.0 / ellapsed_time;
	cent = fps - double(int(fps));
	if (cent >= 0.5)
		fps += 1.0;
	cout << (float)current_time << "\t" << int(fps) << "fps" << endl;
	last_time += ellapsed_time;
}

void	renderObj3d(vector<Obj3d*>	obj3dList, Cam& cam) {
	// cout << "render all Obj3d" << endl;
	//assuming all Obj3d have the same program
	Obj3d*		obj = *(obj3dList.begin());
	Obj3dPG&	pg = obj->getProgram();
	glUseProgram(pg._program);//used once for all obj3d
	Math::Matrix4	proMatrix(cam.getProjectionMatrix());
	Math::Matrix4	viewMatrix = cam.getViewMatrix();
	proMatrix.mult(viewMatrix);// do it in shader ? NO cauz shader will do it for every vertix
	for (Obj3d* object : obj3dList)
		object->render(proMatrix);
	for (Obj3d* object : obj3dList) {
		object->local._matrixChanged = false;
		object->_worldMatrixChanged = false;
	}
}

void	renderSkybox(Skybox& skybox, Cam& cam) {
	// cout << "render Skybox" << endl;
	SkyboxPG&	pg = skybox.getProgram();
	glUseProgram(pg._program);//used once
	
	Math::Matrix4	proMatrix(cam.getProjectionMatrix());
	Math::Matrix4&	viewMatrix = cam.getViewMatrix();
	proMatrix.mult(viewMatrix);

	skybox.render(proMatrix);
}

void	check_paddings() {
	//	cout << sizeof(BITMAPINFOHEADER) << " = " << sizeof(BMPINFOHEADER) << endl;
#ifdef _WIN32
	cout << sizeof(BITMAPFILEHEADER) << " = " << sizeof(BMPFILEHEADER) << endl;
	cout << "bfType\t" << offsetof(BMPINFOHEADERBITMAPFILEHEADER, bfType) << endl;
	cout << "bfSize\t" << offsetof(BITMAPFILEHEADER, bfSize) << endl;
	cout << "bfReserved1\t" << offsetof(BITMAPFILEHEADER, bfReserved1) << endl;
	cout << "bfReserved2\t" << offsetof(BITMAPFILEHEADER, bfReserved2) << endl;
	cout << "bfOffBits\t" << offsetof(BITMAPFILEHEADER, bfOffBits) << endl;
#endif//_WIN32
	cout << "unsigned short\t" << sizeof(unsigned short) << endl;
	cout << "unsigned long \t" << sizeof(unsigned long) << endl;
	cout << "long          \t" << sizeof(long) << endl;
	cout << "long long     \t" << sizeof(long long) << endl;
	cout << "int           \t" << sizeof(int) << endl;
	if (sizeof(BMPFILEHEADER) != 14 || sizeof(BMPINFOHEADER) != 40) {
		cerr << "Padding in structure, exiting..." << endl << endl;
		cout << "BMPFILEHEADER\t" << sizeof(BMPFILEHEADER) << endl;
		cout << "bfType     \t" << offsetof(BMPFILEHEADER, bfType) << endl;
		cout << "bfSize     \t" << offsetof(BMPFILEHEADER, bfSize) << endl;
		cout << "bfReserved1\t" << offsetof(BMPFILEHEADER, bfReserved1) << endl;
		cout << "bfReserved2\t" << offsetof(BMPFILEHEADER, bfReserved2) << endl;
		cout << "bfOffBits\t" << offsetof(BMPFILEHEADER, bfOffBits) << endl;
		cout << "-----" << endl;
		cout << "BMPINFOHEADER\t" << sizeof(BMPINFOHEADER) << endl;
		cout << "biSize     \t" << offsetof(BMPINFOHEADER, biSize) << endl;
		cout << "biWidth    \t" << offsetof(BMPINFOHEADER, biWidth) << endl;
		cout << "biHeight\t" << offsetof(BMPINFOHEADER, biHeight) << endl;
		cout << "biPlanes\t" << offsetof(BMPINFOHEADER, biPlanes) << endl;
		cout << "biBitCount\t" << offsetof(BMPINFOHEADER, biBitCount) << endl;
		cout << "biCompression\t" << offsetof(BMPINFOHEADER, biCompression) << endl;
		cout << "biSizeImage\t" << offsetof(BMPINFOHEADER, biSizeImage) << endl;
		cout << "biXPelsPerMeter\t" << offsetof(BMPINFOHEADER, biXPelsPerMeter) << endl;
		cout << "biYPelsPerMeter\t" << offsetof(BMPINFOHEADER, biYPelsPerMeter) << endl;
		cout << "biClrUsed\t" << offsetof(BMPINFOHEADER, biClrUsed) << endl;
		cout << "biClrImportant\t" << offsetof(BMPINFOHEADER, biClrImportant) << endl;
		exit(ERROR_PADDING);
	}
}

void	growAndShrink(Object& ref, void* ptr) {
	static float	growCoef = 1;

	Fps * fps_ptr = (Fps*)ptr;
	Math::Vector3	s = ref.local.getScale();
	float	addScale = (s.x / 2.0f) * (float)fps_ptr->tick;
	addScale *= growCoef;
	s.add(Math::Vector3(addScale, addScale, addScale));
	ref.local.setScale(s);
	
	//alternate grow/shrink each second
	int	t = int(fps_ptr->last_time) % 2;
	growCoef = (t == 0) ? -1 : 1;
}

void	rotAndGoZaxis(Object& ref, void* ptr) {
	static float	anglePerSec = 50;
	static float	distPerSec = 200;
	Fps * fps_ptr = (Fps*)ptr;
	
	Math::Rotation	rot = ref.local.getRot();
	rot.setAsDegree();
	rot.z += anglePerSec * (float)fps_ptr->tick;
	ref.local.setRot(rot);

	// Math::Vector3	pos = ref.local.getPos();
	// pos.add(Math::Vector3(0, 0, distPerSec * (float)fps_ptr->tick));
	// ref.local.setPos(pos);
}

void	rotX(Object& ref, void* ptr) {
	static float	anglePerSec = -20;
	Fps * fps_ptr = (Fps*)ptr;
	
	ref.local.rotate(anglePerSec * (float)fps_ptr->tick, 0, 0);
}

void	rotY(Object& ref, void* ptr) {
	static float	anglePerSec = 20;
	Fps * fps_ptr = (Fps*)ptr;

	ref.local.rotate(0, anglePerSec * (float)fps_ptr->tick, 0);
}

struct	followObjectArgs
{
	Fps*	fps;
	Object*	o;
};
void	followObject(Object& ref, void *ptr) {
	followObjectArgs * st = (followObjectArgs*)ptr;

	Math::Matrix4	targetWorldMat = st->o->getWorldMatrix();
	Math::Vector3	targetPos;
	targetWorldMat.setOrder(COLUMN_MAJOR);
	targetPos.x = targetWorldMat.tab[3][0];
	targetPos.y = targetWorldMat.tab[3][1];
	targetPos.z = targetWorldMat.tab[3][2];

	Math::Matrix4	worldMat = ref.getWorldMatrix();
	worldMat.setOrder(COLUMN_MAJOR);
	Math::Vector3	objPos;
	objPos.x = worldMat.tab[3][0];
	objPos.y = worldMat.tab[3][1];
	objPos.z = worldMat.tab[3][2];

	Math::Vector3	diff = targetPos;
	diff.sub(objPos);
	float	magnitude = diff.magnitude();
	float	speed = 120.0f * st->fps->tick;
	if (magnitude > speed) {
		diff.div(magnitude);
		diff.mult(speed);
		objPos.add(diff);
		ref.local.setPos(objPos);
	} else {
		ref.local.setPos(targetPos);
	}
}

void	scene1() {
	Glfw	glfw(1600, 900);
	glfw.setTitle("This title is long, long enough to test how glfw manages oversized titles. At this point I dont really know what to write, so let's just bullshiting it ....................................................... is that enough? Well, it depends of the size of the current window. I dont really know how many characters i have to write for a width of 1920. Is it possible to higher the police ? It could save some characters. Ok, im bored, lets check if this title is long enough!");

	//Program for Obj3d (can render Obj3d) with vertex & fragmetns shaders
	Obj3dPG			obj3d_prog(OBJ3D_VS_FILE, OBJ3D_FS_FILE);

	//Create Obj3dBP from .obj files
	Obj3dBP			the42BP("obj3d/42.obj", true);
	Obj3dBP			cubeBP("obj3d/cube.obj", true);
	Obj3dBP			teapotBP("obj3d/teapot2.obj", true);
	// Obj3dBP			rocketBP("obj3d/Rocket_Phoenix/AIM-54_Phoenix_OBJ/Aim-54_Phoenix.obj", true);
	Obj3dBP			rocketBP("obj3d/ARSENAL_VG33/Arsenal_VG33.obj", true);
	// Obj3dBP			lamboBP("obj3d/lambo/Lamborginhi_Aventador_OBJ/Lamborghini_Aventador.obj", true);
	Obj3dBP			lamboBP("obj3d/lambo/Lamborginhi_Aventador_OBJ/Lamborghini_Aventador_no_collider.obj", true);
	cout << "======" << endl;

	Texture*	texture1 = new Texture("images/lena.bmp");
	Texture*	texture2 = new Texture("images/skybox2.bmp");
	Texture*	texture3 = new Texture("images/skyboxfuck.bmp");
	Texture*	texture4 = new Texture("images/skybox4096.bmp");
	Texture*	texture5 = new Texture("images/skytest.bmp");
	// Texture*	texture6 = new Texture("obj3d/Rocket_Phoenix/AIM-54_Phoenix_OBJ/Phoenix.bmp");
	Texture*	texture6 = new Texture("obj3d/ARSENAL_VG33/Arsenal_VG33.bmp");
	Texture*	texture7 = new Texture("obj3d/lambo/Lamborginhi_Aventador_OBJ/Lamborginhi_Aventador_diffuse.bmp");
	Texture		texture8 = *texture7;


	float s = 1.0f;//scale
	//Create Obj3d with the blueprint & by copy
	Obj3d			the42_1(cubeBP, obj3d_prog);//the42BP !
		the42_1.local.setPos(-4, -2, -2);
		the42_1.setTexture(texture1);
		the42_1.displayTexture = true;
		the42_1.setPolygonMode(GL_FILL);
		the42_1.local.setScale(10, 10, 10);
		// the42_1.centered = true;

	Obj3d			the42_2(the42_1);
		//the42_2.local.setPos(0, 3, -5);
		the42_2.local.setPos(-4, -2, -2);
		the42_2.setTexture(texture2);
		the42_2.displayTexture = false;
		the42_2.local.centered = true;
		// the42_2.setScale(3.0f, 0.75f, 0.3f);
		the42_2.setPolygonMode(GL_LINE);

	Obj3d			teapot1(teapotBP, obj3d_prog);
		teapot1.local.setPos(0, 0, 2);
		teapot1.local.getMatrix().setOrder(ROW_MAJOR);
		// teapot1.setRot(0, 90, 0);
		teapot1.setTexture(texture1);
		teapot1.displayTexture = false;
		teapot1.setPolygonMode(GL_LINE);
		teapot1._motionBehaviorFunc = &followObject;
		teapot1._motionBehavior = true;
		// teapot1.setScale(1.5, 2, 0.75);

	Obj3d			cube1(cubeBP, obj3d_prog);
		cube1.local.setPos(0, -2, 3);
		cube1.setTexture(texture1);
		cube1.displayTexture = false;

	Object			empty1;
		empty1._motionBehaviorFunc = &rotX;
		empty1._motionBehavior = true;
		empty1.local.setScale(1,1,1);

	Obj3d			rocket1(rocketBP, obj3d_prog);
		// rocket1.local.setPos(-10, -20, -2000);
		rocket1.local.setPos(0, -300, 0);
		rocket1.local.rotate(0, 180, 0);
		rocket1.setTexture(texture6);
		rocket1.displayTexture = true;
		rocket1.local.centered = true;
		// rocket1.setPolygonMode(GL_LINE);
		rocket1._motionBehaviorFunc = &rotAndGoZaxis;
		rocket1._motionBehavior = true;
		s = 10.0f;
		rocket1.local.setScale(s,s,s);
		rocket1.setParent(&empty1);

	// Properties::defaultSize = 13.0f;
	Obj3d			lambo1(lamboBP, obj3d_prog);
		lambo1.local.setPos(-20, 0, 0);
		lambo1.local.setPos(0, -5, 7);
		lambo1.setTexture(texture7);
		lambo1.displayTexture = true;
		lambo1.local.centered = true;
		// lambo1.setPolygonMode(GL_LINE);
		lambo1._motionBehaviorFunc = &growAndShrink;
		lambo1._motionBehavior = true;
		s = 0.025f;
		// lambo1.setScale(s, s, s);

	Obj3d			lambo2(lamboBP, obj3d_prog);
		// lambo2.local.setPos(0, -1.9f, 0);
		lambo2.local.setPos(0, -6.0f, 0);
		lambo2.local.setRot(0, 180.0f, 0);
		lambo2.setTexture(texture7);
		lambo2.displayTexture = true;
		lambo2.local.centered = true;
		// lambo2.setPolygonMode(GL_LINE);
		lambo2._motionBehaviorFunc = &rotY;
		lambo2._motionBehavior = true;
		s = 0.4f;
		// lambo2.local.setScale(s, s, s);
		// lambo2.setParent(&the42_1);
		lambo2.setParent(&rocket1);

	Obj3d			lambo3(lamboBP, obj3d_prog);
		lambo3.local.setPos(0, -4, 0);
		lambo3.local.setRot(0, 0.0f, 180);
		lambo3.setTexture(&texture8);
		lambo3.displayTexture = true;
		lambo3.local.centered = true;
		// lambo3.setPolygonMode(GL_LINE);
		// lambo3._motionBehaviorFunc = &growAndShrink;
		// lambo3._motionBehavior = true;
		s = 30.0f;
		// lambo3.local.setScale(s, s, s);
		// lambo3.setParent(&the42_1);
		lambo3.setParent(&lambo2);

	// demonstrate_scale_in_matrix(lambo2);

	// Properties::defaultSize = PP_DEFAULT_SIZE;

	cout << "Object # : " << Object::getInstanceAmount() << endl;
	cout << "Obj3d # : " << Obj3d::getInstanceAmount() << endl;
	cout << endl;

	cout << "GL_MAX_CUBE_MAP_TEXTURE_SIZE " << GL_MAX_CUBE_MAP_TEXTURE_SIZE << endl;
	cout << "GL_MAX_TEXTURE_SIZE " << GL_MAX_TEXTURE_SIZE << endl;

	SkyboxPG	sky_pg(CUBEMAP_VS_FILE, CUBEMAP_FS_FILE);
	Skybox		skybox(*texture4, sky_pg);
	
	vector<Obj3d*>	obj3dList;
	obj3dList.push_back(&the42_1);
	// obj3dList.push_back(&the42_2);
	obj3dList.push_back(&teapot1);
	// obj3dList.push_back(&cube1);
	obj3dList.push_back(&rocket1);
	obj3dList.push_back(&lambo1);
	obj3dList.push_back(&lambo2);
	obj3dList.push_back(&lambo3);

	if (true) {//spiral
		// Obj3d*	backObj = &lambo3;
		for (int i = 0; i < 20; i++) {
			Obj3d*			lamboPlus = new Obj3d(lamboBP, obj3d_prog);
			lamboPlus->setParent(&lambo3);
			lamboPlus->displayTexture = (i % 2 ) ? true : false;
			lamboPlus->local.centered = true;
			lamboPlus->setTexture(&texture8);
			// lamboPlus->_motionBehaviorFunc = &rotY;
			lamboPlus->_motionBehavior = false;
			float maxScale = 3;
			float scale = (float)((i % (int)maxScale) - (maxScale/2));
			lamboPlus->local.enlarge(scale, scale, scale);
			float	val = cosf(Math::toRadian(i*10)) * 10;
			float	coef = 1.0f;
			lamboPlus->local.setPos(lambo3.local.getPos());
			lamboPlus->local.translate(float(i)/coef, val/coef, val/coef);
			lamboPlus->local.rotate(Math::Rotation(i*5,i*5,i*5));

			obj3dList.push_back(lamboPlus);
			// backObj = lamboPlus;
		}
	}


	Cam		cam(glfw);
	cam.local.centered = false;
	cam.local.setPos(0, 1, 3);
	cam.printProperties();

	cam.setParent(&rocket1);
	cam.lockedMovement = true;
	// cam.lockedOrientation = true;

	cout << "Begin while loop" << endl;
	Fps	fps144(144);
	Fps	fps60(60);
	Fps	fps30(30);
	Fps* defaultFps = &fps60;

	followObjectArgs	st = { defaultFps, &cam };

	// cam.local.setScale(s,s,s);//bad, undefined behavior
	while (!glfwWindowShouldClose(glfw._window)) {
		if (defaultFps->wait_for_next_frame()) {
			// printFps();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderObj3d(obj3dList, cam);
			renderSkybox(skybox, cam);
			glfwSwapBuffers(glfw._window);
			glfwPollEvents();
			glfw.updateMouse();//to do before cam's events
			cam.events(glfw, float(defaultFps->tick));
			//////////////////////////////////////////
			Math::Matrix4	matRocket = rocket1.getWorldMatrix();
			matRocket.printData();
			cout << "---------------" << endl;
			//////////////////////////////////////////
			if (false) {
			cout << "---rocket1" << endl;
			rocket1.local.getScale().printData();
			// rocket1.getWorldMatrix().printData();
			cout << "---lambo2" << endl;
			lambo2.local.getScale().printData();
			// lambo2.getWorldMatrix().printData();
			cout << "---lambo3" << endl;
			lambo3.local.getScale().printData();
			// lambo3.getWorldMatrix().printData();
			cout << "---------------" << endl;
			lamboBP.getDimensions().printData();
			cout << "---------------" << endl;
			}


			////////////////////////////////////////// motion
			//this should be used in another func, life a special func managing all events/behavior at every frames
			rocket1.runMothionBehavior((void*)defaultFps);
			lambo1.runMothionBehavior((void*)defaultFps);
			lambo2.runMothionBehavior((void*)defaultFps);
			teapot1.runMothionBehavior((void*)&st);
			empty1.render(cam.getProjectionMatrix());
			empty1.runMothionBehavior((void*)defaultFps);
			////////////////////////////////////////// motion end
		
			if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_ESCAPE))
				glfwSetWindowShouldClose(glfw._window, GLFW_TRUE);
		}
	}
	cout << "End while loop" << endl;

	cout << "deleting textures..." << endl;
	delete texture1;
	delete texture2;
	delete texture3;
	delete texture4;
	delete texture5;
	delete texture6;
	delete texture7;
}

#include "properties.hpp"
void	test_pp() {
	Properties	parent_local;
	parent_local.setPos(10.0f, 5.0f, 8.0f);
	parent_local.setRot(40.0f, 89.72f, 50.0f);
	Math::Matrix4&	plmatrix = parent_local.getMatrix();
	plmatrix.modelMatrix(parent_local.getPos(), parent_local.getRot(), parent_local.getScale());

	Math::Matrix4 mat(plmatrix);
	mat.setOrder(ROW_MAJOR);
	float	(&m)[4][4] = *reinterpret_cast<float(*)[4][4]>(mat.getData());

	// Angle Y
	// m[0][2] = sinf(y)
	// y = asinf(m[0][2])
	
	// Angle X
	// m[2][2] = cosf(x) * cosf(y)
	// cosf(x) = m[2][2] / cosf(y)
	// x = acosf(m[2][2] / cosf(y))

	// Angle Z
	// m[0][0] = cosf(y) * cosf(z)
	// cosf(z) = m[0][0] / cosf(y)
	// z = acosf(m[0][0] / cosf(y))

	float	x, y, z;
	y = asinf(m[0][2]);
	float	cos_y = cosf(y);
	if (fabs(cos_y) < 0.005f) {
		/*
			get fucked by Gimball lock
			in Properties mutators:
				always check for a Yangle too close from 90.0f degrees
				correct it with 0.17 degree from 90
		*/
		cout << "x and z angle are false" << endl;
	}
	x = acosf(m[2][2] / cos_y);
	z = acosf(m[0][0] / cos_y);

	cout << "angle x :\t" << x << " (" << Math::toDegree(x) << ")" << endl;
	cout << "angle y :\t" << y << " (" << Math::toDegree(y) << ")" << endl;
	cout << "angle z :\t" << z << " (" << Math::toDegree(z) << ")" << endl;
	exit(0);
}

int		main(void) {
	// test_pp();
	check_paddings();
//	test_mult_mat4(); exit(0);
	cout << "____START____" << endl;
//	test_obj_loader();

	scene1();
	// while(1);

	return (EXIT_SUCCESS);
}
