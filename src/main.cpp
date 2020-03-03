/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhoffsch <rhoffsch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/18 22:45:30 by rhoffsch          #+#    #+#             */
/*   Updated: 2019/04/03 14:58:50 by rhoffsch         ###   ########.fr       */
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
#include "transformBH.hpp"
#include "fps.hpp"
#include "gamemanager.hpp"
#include "framebuffer.hpp"
#include "uipanel.hpp"
#include "quadtree.hpp"

#include <string>
#include <cstdio>
#include <vector>
#include <list>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#define WINX 1900
#define WINY 1050
#define WIN32_VS_FOLDER string("")

void	renderObj3d(list<Obj3d*>	obj3dList, Cam& cam) {
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
#ifdef _WIN322
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

class AnchorCameraBH : public Behavior
{
	/*
		La rotation fonctionne bien sur la cam (ca a l'air),
		mais le probleme vient de l'ordre de rotation sur l'anchor.
		? Ne pas utiliser ce simple system de rotation
		? rotater par rapport au system local de l'avion
		? se baser sur une matrice cam-point-at
			https://mikro.naprvyraz.sk/docs/Coding/Atari/Maggie/3DCAM.TXT
	*/
public:
	AnchorCameraBH() : Behavior() {
		this->copyRotation = true;
	}
	void	behaveOnTarget(BehaviorManaged* target) {
		if (this->_anchor) {
			Object*	speAnchor = dynamic_cast<Object*>(this->_anchor);//specialisation part
			// turn this in Obj3d to get the BP, to get the size of the ovject,
			// to position the camera relatively to the obj's size.

			Cam*	speTarget = dynamic_cast<Cam*>(target);//specialisation part

			Math::Vector3	forward(0, -15, -35);
			if (this->copyRotation) {
				Math::Rotation	rot = speAnchor->local.getRot();
				forward.rotate(rot, 1);
				rot.mult(-1);
				speTarget->local.setRot(rot);
			}
			forward.mult(-1);// invert the forward to position the cam on the back, a bit up
			Math::Vector3	pos = speAnchor->local.getPos();
			pos.translate(forward);
			speTarget->local.setPos(pos);
		}
	}
	bool	isCompatible(BehaviorManaged* target) const {
		//dynamic_cast check for Cam
		(void)target;
		return (true);
	}

	void			setAnchor(Object* anchor) {
		this->_anchor = anchor;
	}

	bool			copyRotation;
private:
	Object*			_anchor;
	Math::Vector3	_offset;

};

void blitToWindow(FrameBuffer* readFramebuffer, GLenum attachmentPoint, UIPanel* panel) {
	GLuint fbo;
	if (readFramebuffer) {
		fbo = readFramebuffer->fbo;
	}
	else {
		fbo = panel->getFbo();
	}
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

	//glViewport(0, 0, manager->glfw->getWidth(), manager->glfw->getHeight());//size of the window/image or panel width ?
	glReadBuffer(attachmentPoint);
	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	int w;
	int h;
	if (readFramebuffer) {
		w = readFramebuffer->getWidth();
		h = readFramebuffer->getHeight();
	} else if (panel->getTexture()) {
		w = panel->getTexture()->getWidth();
		h = panel->getTexture()->getHeight();
	} else {
		std::cout << "FUCK " << __PRETTY_FUNCTION__ << std::endl;
		exit(2);
	}
	if (0) {
		std::cout << "copy " << w << "x" << h << "\tresized\t" << panel->_width << "x" << panel->_height \
			<< "\tat pos\t" << panel->_posX << ":" << panel->_posY << std::endl;
		// << " -> " << (panel->posX + panel->width) << "x" << (panel->posY + panel->height) << std::endl;
	}
	glBlitFramebuffer(0, 0, w, h, \
		panel->_posX, panel->_posY, panel->_posX2, panel->_posY2, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void	scene1() {
	Glfw	glfw(1600, 900);
	glfw.setTitle("This title is long, long enough to test how glfw manages oversized titles. At this point I dont really know what to write, so let's just bullshiting it ....................................................... is that enough? Well, it depends of the size of the current window. I dont really know how many characters i have to write for a width of 1920. Is it possible to higher the police ? It could save some characters. Ok, im bored, lets check if this title is long enough!");

	//Program for Obj3d (can render Obj3d) with vertex & fragmetns shaders
	Obj3dPG			obj3d_prog(OBJ3D_VS_FILE, OBJ3D_FS_FILE);

#ifndef BLUEPRINTS
	//Create Obj3dBP from .obj files
	Obj3dBP			the42BP("obj3d/42.obj", true);
	Obj3dBP			cubeBP("obj3d/cube.obj", true);
	Obj3dBP			teapotBP("obj3d/teapot2.obj", true);
	// Obj3dBP			rocketBP("obj3d/Rocket_Phoenix/AIM-54_Phoenix_OBJ/Aim-54_Phoenix.obj", true);
	Obj3dBP			rocketBP("obj3d/ARSENAL_VG33/Arsenal_VG33.obj", true);
	// Obj3dBP			lamboBP("obj3d/lambo/Lamborginhi_Aventador_OBJ/Lamborghini_Aventador.obj", true);
	Obj3dBP			lamboBP("obj3d/lambo/Lamborginhi_Aventador_OBJ/Lamborghini_Aventador_no_collider.obj", true);
	cout << "======" << endl;
#endif
#ifndef TEXTURES
		Texture*	texture1 = new Texture("images/lena.bmp");
		Texture*	texture2 = new Texture("images/skybox2.bmp");
		Texture*	texture3 = new Texture("images/skyboxfuck.bmp");
	//	Texture*	texture4 = new Texture("images/skybox4096.bmp");
		Texture*	texture5 = new Texture("images/skytest.bmp");
		// Texture*	texture6 = new Texture("obj3d/Rocket_Phoenix/AIM-54_Phoenix_OBJ/Phoenix.bmp");
		// Texture*	texture6 = new Texture("obj3d/ARSENAL_VG33/Arsenal_VG33.bmp");
	//	Texture*	texture7 = new Texture("obj3d/lambo/Lamborginhi_Aventador_OBJ/Lamborginhi_Aventador_diffuse.bmp");
		Texture		texture8 = *texture1;
#endif
#ifndef OBJ3D
	list<Obj3d*>	obj3dList;

	float s = 1.0f;//scale
	//Create Obj3d with the blueprint & by copy
	Obj3d			the42_1(cubeBP, obj3d_prog);//the42BP !
		the42_1.local.setPos(0, 0, 0);
		the42_1.setTexture(texture1);
		the42_1.displayTexture = true;
		the42_1.setPolygonMode(GL_FILL);
		the42_1.local.setScale(1, 1, 1);

	Obj3d			the42_2(the42_1);
		//the42_2.local.setPos(0, 3, -5);
		the42_2.local.setPos(-4, -2, -2);
		the42_2.setTexture(texture2);
		the42_2.displayTexture = false;
		// the42_2.setScale(3.0f, 0.75f, 0.3f);
		the42_2.setPolygonMode(GL_LINE);

	Obj3d			teapot1(teapotBP, obj3d_prog);
		teapot1.local.setPos(0, 0, 2);
		teapot1.local.getMatrix().setOrder(ROW_MAJOR);
		// teapot1.setRot(0, 90, 0);
		teapot1.setTexture(texture1);
		teapot1.displayTexture = false;
		teapot1.setPolygonMode(GL_LINE);
		// teapot1.setScale(1.5, 2, 0.75);

	Obj3d			cube1(cubeBP, obj3d_prog);
		cube1.local.setPos(0, -2, 3);
		cube1.setTexture(texture1);
		cube1.displayTexture = false;

	Object			empty1;
		empty1.local.setScale(1,1,1);

	Obj3d			rocket1(rocketBP, obj3d_prog);
		// rocket1.local.setPos(-10, -20, -2000);
		rocket1.local.setPos(0, -300, 0);
		rocket1.local.rotate(0, 180, 0);
		rocket1.setTexture(texture5);
		rocket1.displayTexture = true;
		// rocket1.setPolygonMode(GL_LINE);
		s = 10.0f;
		rocket1.local.setScale(s,s,s);
		rocket1.setParent(&empty1);


	// Properties::defaultSize = 13.0f;
	Obj3d			lambo1(lamboBP, obj3d_prog);
		lambo1.local.setPos(-20, 0, 0);
		// lambo1.local.setScale(1, 1, 1);
		lambo1.local.setPos(0, -5, 7);
		lambo1.setTexture(texture1);
		lambo1.displayTexture = true;
		lambo1.setPolygonMode(GL_LINE);
		s = 0.025f;
		// lambo1.setScale(s, s, s);

	Obj3d			lambo2(lamboBP, obj3d_prog);
		// lambo2.local.setPos(0, -1.9f, 0);
		lambo2.local.setPos(0, -6.0f, 0);
		// lambo2.local.setScale(1, 5, 1);
		lambo2.local.setRot(0, 180.0f, 0);
		lambo2.setTexture(texture1);
		lambo2.displayTexture = true;
		// lambo2.setPolygonMode(GL_LINE);
		s = 0.4f;
		// lambo2.local.setScale(s, s, s);
		// lambo2.setParent(&the42_1);
		lambo2.setParent(&rocket1);

	Obj3d			lambo3(lamboBP, obj3d_prog);
		lambo3.local.setPos(0, -4, 0);
		lambo3.local.setRot(0, 0.0f, 180);
		lambo3.setTexture(&texture8);
		lambo3.displayTexture = true;
		// lambo3.setPolygonMode(GL_LINE);
		s = 30.0f;
		// lambo3.local.setScale(s, s, s);
		// lambo3.setParent(&the42_1);
		lambo3.setParent(&lambo2);

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
				Obj3d* lamboPlus = new Obj3d(lamboBP, obj3d_prog);
				lamboPlus->setParent(&lambo3);
				lamboPlus->displayTexture = (i % 2) ? true : false;
				lamboPlus->setTexture(&texture8);
				lamboPlus->setColor(i * 45 % 255, i * 10 % 255, i * 73 % 255);
				float maxScale = 3;
				float scale = (float)((i % (int)maxScale) - (maxScale / 2));
				lamboPlus->local.enlarge(scale, scale, scale);
				float	val = cosf(Math::toRadian(i * 10)) * 10;
				float	coef = 1.0f;
				lamboPlus->local.setPos(lambo3.local.getPos());
				lamboPlus->local.translate(float(i) / coef, val / coef, val / coef);
				lamboPlus->local.rotate(Math::Rotation(i * 5, i * 5, i * 5));

				obj3dList.push_back(lamboPlus);
				// backObj = lamboPlus;
			}
		}
#endif


	// Properties::defaultSize = PP_DEFAULT_SIZE;

	cout << &rocket1 << endl;
	// test_behaviors(rocket1);

	cout << "Object # : " << Object::getInstanceAmount() << endl;
	cout << "Obj3d # : " << Obj3d::getInstanceAmount() << endl;
	cout << endl;
	cout << "GL_MAX_CUBE_MAP_TEXTURE_SIZE " << GL_MAX_CUBE_MAP_TEXTURE_SIZE << endl;
	cout << "GL_MAX_TEXTURE_SIZE " << GL_MAX_TEXTURE_SIZE << endl;

	SkyboxPG	sky_pg(CUBEMAP_VS_FILE, CUBEMAP_FS_FILE);
	Skybox		skybox(*texture3, sky_pg);

#ifndef CAM
	Cam		cam(glfw);
	cam.local.setPos(0, 0, 10);
	cam.printProperties();
	cam.lockedMovement = false;
	cam.lockedOrientation = false;

	glfw.setMouseAngle(45);
	std::cout << "MouseAngle: " <<  glfw.getMouseAngle() << std::endl;
	//exit(0);

	if (false) {//cam anchor to rocket1, bugged with Z rot
		cam.local.setPos(0, 1.5f, 3.5f);
		cam.setParent(&rocket1);
		cam.lockedMovement = true;
		cam.lockedOrientation = true;
	}
#endif

	Fps	fps144(144);
	Fps	fps60(60);
	Fps	fps30(30);
	Fps* defaultFps = &fps144;

	//followObjectArgs	st = { defaultFps, &cam };


#ifndef BEHAVIORS
	cout << "behavior:" << endl;
	TransformBH		b1;
		b1.transform.rot.setUnit(ROT_DEG);
		b1.transform.rot.z = 10 * defaultFps->getTick();
		b1.modeRot = ADDITIVE;
		float ss = 1.0f + 0.1f * defaultFps->getTick();
		//b1.transform.scale = Math::Vector3(ss, ss, ss);
	//	b1.modeScale = MULTIPLICATIVE;
		cout << "___ adding rocket1: " << &rocket1 << endl;
		b1.addTarget(&rocket1);
		b1.addTarget(&lambo1);
		b1.removeTarget(&lambo1);
		b1.setTargetStatus(&rocket1, false);
		std::cout << "rocket1 status: " << b1.getTargetStatus(&rocket1) << std::endl;
		b1.setTargetStatus(&rocket1, true);
		std::cout << "rocket1 status: " << b1.getTargetStatus(&rocket1) << std::endl;

	TransformBH		b2;// = b1;//bug
		b2.transform.scale = Math::Vector3(0,0,0);
		b2.modeScale = ADDITIVE;
		// b2.transform.rot.z = 0.0f;
		b2.transform.rot.x = -45.0f * defaultFps->getTick();
		// b2.removeTarget(&rocket1);
		b2.addTarget(&empty1);
		//bug if i do:
		b2.addTarget(&empty1);
		b2.removeTarget(&rocket1);
	
	TransformBH		b3;// = b1;//bug
		b3.transform.scale = Math::Vector3(0,0,0);
		b3.modeScale = ADDITIVE;
		b3.transform.rot.y = 102.0f * defaultFps->getTick();
		b3.addTarget(&lambo2);

		cout << "b1: " << b1.getTargetList().size() << endl;
		cout << "b2: " << b2.getTargetList().size() << endl;

	TransformBH		b4;// = b1;//bug
		b4.transform.scale = Math::Vector3(0,0,0);
		b4.modeScale = ADDITIVE;
		b4.transform.rot.y = 720.0f * defaultFps->getTick();
		b4.addTarget(&teapot1);
	// exit(0);

	if (false) {// check behavior target, add remove

		cout << "behaviorsActive: " << (empty1.behaviorsActive ? "true" : "false") << endl;
		cout << "------------" << endl;
		cout << "lambo1:\t" << lambo1.behaviorList.size() << endl;
		cout << "b2:    \t" << b2.getTargetList().size() << endl;
		cout << "b2.addTarget(&lambo1);" << endl;
		b2.addTarget(&lambo1);
		cout << "lambo1:\t" << lambo1.behaviorList.size() << endl;
		cout << "b2:    \t" << b2.getTargetList().size() << endl;
		cout << "b2.removeTarget(&lambo1);" << endl;
		b2.removeTarget(&lambo1);
		cout << "lambo1:\t" << lambo1.behaviorList.size() << endl;
		cout << "b2:    \t" << b2.getTargetList().size() << endl;

		cout << "------------" << endl;
		cout << "lambo1:\t" << lambo1.behaviorList.size() << endl;
		cout << "b2:    \t" << b2.getTargetList().size() << endl;
		cout << "lambo1.addBehavior(&b2);" << endl;
		lambo1.addBehavior(&b2);
		cout << "lambo1:\t" << lambo1.behaviorList.size() << endl;
		cout << "b2:    \t" << b2.getTargetList().size() << endl;
		cout << "lambo1.removeBehavior(&b2);" << endl;
		lambo1.removeBehavior(&b2);
		cout << "lambo1:\t" << lambo1.behaviorList.size() << endl;
		cout << "b2:    \t" << b2.getTargetList().size() << endl;
	}
#endif
// exit(0);

#ifndef GAMEMANAGER
	GameManager	manager;
	manager.glfw = &glfw;
	manager.cam = &cam;

	glfw.activateDefaultCallbacks(&manager);

#endif

#ifndef RENDER
	cout << "Begin while loop" << endl;
	// cam.local.setScale(s,s,s);//bad, undefined behavior
	while (!glfwWindowShouldClose(glfw._window)) {
		if (defaultFps->wait_for_next_frame()) {
			//////////////////////////////////////////
			if (true) {
				if (false) {
					Math::Matrix4	matRocket = rocket1.getWorldMatrix();
					matRocket.printData();
					cout << "---------------" << endl;
				}
				if (false) {
					the42_1.getWorldMatrix().printData();
				}
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
			}
			////////////////////////////////////////// motion/behaviors
			//this should be used in another func, life a special func managing all events/behavior at every frames
			if (true) {
				b1.run();
				b2.run();
				b3.run();
				b4.run();
				if (false) {
					Math::Vector3 p = teapot1.local.getPos();
					float r = 180 * defaultFps->getTick();
					p.rotateAround(cam.local.getPos(), Math::Rotation(r, 0, 0));
					teapot1.local.setPos(p);
				} else if (true) {
					float r = 180 * defaultFps->getTick();
					teapot1.local.rotateAround(cam.local.getPos(), Math::Rotation(r, 0, 0));
				}
				// Math::Matrix4	matEmpty = empty1.getWorldMatrix();
				// matEmpty.printData();
			}
			////////////////////////////////////////// motion end

			glfwPollEvents();
			glfw.updateMouse();//to do before cam's events
			cam.events(glfw, float(defaultFps->getTick()));
			// printFps();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderObj3d(obj3dList, cam);
			renderSkybox(skybox, cam);
			glfwSwapBuffers(glfw._window);

			if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_ESCAPE))
				glfwSetWindowShouldClose(glfw._window, GLFW_TRUE);
		}
	}
	cout << "End while loop" << endl;
#endif

	cout << "deleting textures..." << endl;
	delete texture1;
	delete texture2;
	delete texture3;
//	delete texture4;
	delete texture5;
//	delete texture6;
	//delete texture7;
}

void scene2() {
	Glfw	glfw(1600, 900);
	glfw.setTitle("Tests camera anchor");

	//Program for Obj3d (can render Obj3d) with vertex & fragmetns shaders
	Obj3dPG			obj3d_prog(OBJ3D_VS_FILE, OBJ3D_FS_FILE);
	SkyboxPG		sky_pg(CUBEMAP_VS_FILE, CUBEMAP_FS_FILE);

	Obj3dBP			rocketBP("obj3d/ARSENAL_VG33/Arsenal_VG33.obj", true);

	Texture* texture3 = new Texture("images/skyboxfuck.bmp");
	Texture* texture5 = new Texture("images/skytest.bmp");

	Skybox		skybox(*texture3, sky_pg);


	list<Obj3d*>	obj3dList;

	Obj3d			rocket(rocketBP, obj3d_prog);
	rocket.local.setPos(0, 0, 0);
	rocket.setTexture(texture5);
	rocket.displayTexture = true;
	float s = 10.0f;
	rocket.local.setScale(s, s, s);

	Obj3d			rocket2(rocketBP, obj3d_prog);
	rocket2.local.setPos(5, 0, 0);
	rocket2.setTexture(texture5);
	rocket2.displayTexture = true;
	s = 5000.0f;
	rocket2.local.setScale(s, s, s);

	obj3dList.push_back(&rocket);
	obj3dList.push_back(&rocket2);


	Cam		cam(glfw);
	cam.local.setPos(0, 15, 35);
	cam.printProperties();
	cam.lockedMovement = false;
	cam.lockedOrientation = false;

	glfw.setMouseAngle(-1);
	std::cout << "MouseAngle: " << glfw.getMouseAngle() << std::endl;
	//exit(0);

	//cam.local.setPos(0, 1.5f, 3.5f);
	AnchorCameraBH	b1;
	b1.setAnchor(&rocket);
	b1.addTarget(&cam);
	b1.copyRotation = true;
	cam.lockedMovement = true;
	cam.lockedOrientation = true;


	Fps	fps144(144);
	Fps	fps60(60);
	Fps* defaultFps = &fps144;

	float	mvt = 30.0f * defaultFps->getTick();

	std::cout << "Begin while loop" << endl;
	int c = 0;
	while (!glfwWindowShouldClose(glfw._window)) {
		if (defaultFps->wait_for_next_frame()) {

			if (1) {
				b1.run();
			}

			//print data
			if (false) {
				c++;
				if (c == 60) {
					c = 0;
					system("cls");
					if (true) {
						std::cout << "--- ROCKET" << endl;
						//rocket.local.getScale().printData();
						rocket.local.getRot().printData();
						rocket.getWorldMatrix().printData();
					}
					if (true) {
						std::cout << "--- CAM" << endl;
						cam.local.getRot().printData();
						std::cout << "- local" << endl;
						cam.getLocalProperties().getMatrix().printData();
						std::cout << "- world" << endl;
						cam.getWorldMatrix().printData();
					}
				}
			}

			glfwPollEvents();
			glfw.updateMouse();//to do before cam's events
			cam.events(glfw, float(defaultFps->getTick()));
			if (true) {
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_LEFT))
					rocket.local.rotate(0,		mvt,	0);
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_RIGHT))
					rocket.local.rotate(0,		-mvt,	0);
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_DOWN))
					rocket.local.rotate(mvt,	0,		0);
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_UP))
					rocket.local.rotate(-mvt,	0,		0);
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_KP_1))
					rocket.local.rotate(0,		0,		mvt);
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_KP_2))
					rocket.local.rotate(0,		0,		-mvt);
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_KP_0))
					rocket.local.setRot(0,		0,		0);

				Math::Vector3	pos = rocket.local.getPos();
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_KP_8))
					pos.add(0, 0, -mvt);
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_KP_5))
					pos.add(0, 0, mvt);
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_KP_4))
					pos.add(-mvt, 0, 0);
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_KP_6))
					pos.add(mvt, 0, 0);
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_KP_7))
					pos.add(0, -mvt, 0);
				if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_KP_9))
					pos.add(0, mvt, 0);
				rocket.local.setPos(pos);


			//#define 	GLFW_KEY_RIGHT   262
			//#define 	GLFW_KEY_LEFT   263
			//#define 	GLFW_KEY_DOWN   264
			//#define 	GLFW_KEY_UP   265
			}

			// printFps();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderObj3d(obj3dList, cam);
			renderSkybox(skybox, cam);
			glfwSwapBuffers(glfw._window);

			if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_ESCAPE))
				glfwSetWindowShouldClose(glfw._window, GLFW_TRUE);
		}
	}
	std::cout << "End while loop" << endl;

	std::cout << "deleting textures..." << endl;
	delete texture3;
	delete texture5;
}

void sceneHumanGL() {
	Glfw		glfw(1600, 900); // screen size
	glDisable(GL_CULL_FACE);
	Obj3dPG		obj3d_prog(OBJ3D_VS_FILE, OBJ3D_FS_FILE);
	SkyboxPG	sky_pg(CUBEMAP_VS_FILE, CUBEMAP_FS_FILE);

	Obj3dBP::defaultSize = 1.0f;
	Obj3dBP			cubebp("obj3d/cube.obj", true);
	Math::Vector3	dimensions = cubebp.getDimensions();

	Texture *	lena = new Texture("images/lena.bmp");

#ifndef MEMBERS
	float		epaisseur_tronc = 2.0f;

	Object			containerTronc;
	Obj3d			tronc(cubebp, obj3d_prog);
	tronc.setTexture(lena);
	tronc.displayTexture = true;
	tronc.local.setScale(epaisseur_tronc, epaisseur_tronc * 3, epaisseur_tronc);//bad, use rescale of obj3d vertices
	tronc.setColor(0xff, 0, 0);

	Obj3d			avant_bras_gauche(cubebp, obj3d_prog);
	avant_bras_gauche.displayTexture = false;
	avant_bras_gauche.setColor(0, 0xff, 0);

	Obj3d			avant_bras_droit(cubebp, obj3d_prog);
	avant_bras_droit.displayTexture = false;
	avant_bras_droit.setColor(0, 0, 0xff);

	Obj3d			apres_bras_gauche(cubebp, obj3d_prog);
	apres_bras_gauche.displayTexture = false;
	apres_bras_gauche.setColor(0xff, 0, 0);

	Obj3d			apres_bras_droit(cubebp, obj3d_prog);
	apres_bras_droit.displayTexture = false;
	apres_bras_droit.setColor(0, 0xff, 0xff);

	//hierarchy
	tronc.setParent(&containerTronc);
	avant_bras_gauche.setParent(&containerTronc);
	apres_bras_gauche.setParent(&avant_bras_gauche);
	avant_bras_droit.setParent(&containerTronc);
	apres_bras_droit.setParent(&avant_bras_droit);


	//relative position
	// avant_bras_gauche.local.translate(-1.5, 0, 0);
	avant_bras_droit.local.translate(4.5, 0, 0);
	// apres_bras_gauche.local.translate(VEC3_DOWN);
	// apres_bras_droit.local.translate(VEC3_DOWN);

	//debug
	avant_bras_gauche.local.translate(0, 0.1, 0);
	
	avant_bras_gauche.local.getPos().printData();
	avant_bras_gauche.local.updateMatrix();
	avant_bras_gauche.local.getMatrix().printData();
	cout << "----------\n";
	apres_bras_gauche.local.getPos().printData();
	apres_bras_gauche.local.updateMatrix();
	apres_bras_gauche.local.getMatrix().printData();

	list<Obj3d*>	obj3dList;
	// obj3dList.push_back(&containerTronc);
	obj3dList.push_back(&tronc);
	obj3dList.push_back(&avant_bras_gauche);
	obj3dList.push_back(&avant_bras_droit);
	// obj3dList.push_back(&apres_bras_droit);
	// obj3dList.push_back(&apres_bras_gauche);
#endif // MEMBERS

	Texture*	texture2 = new Texture("images/skybox4.bmp");//skybox3.bmp bug?
	Skybox		skybox(*texture2, sky_pg);
	
	Cam		cam(glfw);
	cam.local.setPos(0, 0, 10);
	cam.setFov(90);
	cam.lockedMovement = false;
	cam.lockedOrientation = false;
	cam.speed /= 4;

	Fps	fps144(144);
	Fps	fps60(60);
	Fps	fps20(5);
	Fps* defaultFps = &fps60;

#ifndef BEHAVIORS
	TransformBH		b1_rot;
	b1_rot.transform.rot.setUnit(ROT_DEG);
	b1_rot.transform.rot.x = 20 * defaultFps->getTick();
	// b1_rot.transform.rot.y = 20 * defaultFps->getTick();
	// b1_rot.transform.rot.z = 20 * defaultFps->getTick();
	b1_rot.modeRot = ADDITIVE;
	b1_rot.addTarget(&avant_bras_droit);
	b1_rot.addTarget(&apres_bras_droit);
	// b1_rot.addTarget(&containerTronc);

	TransformBH		b2_rot;
	b2_rot.transform.rot.setUnit(ROT_DEG);
	// b2_rot.transform.rot.x = 20 * defaultFps->getTick();
	b2_rot.transform.rot.y = 50 * defaultFps->getTick();
	b2_rot.modeRot = ADDITIVE;
	// b2_rot.addTarget(&avant_bras_gauche);
	// b2_rot.addTarget(&avant_bras_droit);
	// b2_rot.addTarget(&apres_bras_droit);

	// avant_bras_gauche.behaviorsActive = false;
	// Behavior::areActive = false;
#endif // BEHAVIORS

#ifndef RENDER

	double t = glfwGetTime();
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(glfw._window)) {
		// std::cout << glfwGetTime() - t << std::endl;

		if (defaultFps->wait_for_next_frame()) {
			b1_rot.run();
			b2_rot.run();

			glfwPollEvents();
			glfw.updateMouse(); // to do before cam's events
			cam.events(glfw, float(defaultFps->getTick()));
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderObj3d(obj3dList, cam);
			renderSkybox(skybox, cam);
			glfwSwapBuffers(glfw._window);
			if (GLFW_PRESS == glfwGetKey(glfw._window, GLFW_KEY_ESCAPE))
				glfwSetWindowShouldClose(glfw._window, GLFW_TRUE);
		}
	}
#endif // RENDER
}

void	fillScreen(char*** screen, int len, QuadNode* node, int* leafAmount) {
	if (!node)
		return;
	if (node->isLeaf()) {
		(*leafAmount)++;
		std::cout << "leaf: " << node->width << "x" << node->height << " at " << node->x << ":" << node->y << std::endl;
		char color;
		if (node->pixel.r == 255) { color = 0; }
		else if (node->pixel.g == 255) { color = 1; }
		else if (node->pixel.b == 255) { color = 2; }
		else {
			std::cout << "error with the pixel\n";
			std::cout << node->pixel.r << " " << node->pixel.g << " " << node->pixel.b << std::endl;
			exit(1);
		}
		if (node->width == 0 || node->height == 0 || node->width > len || node->height > len) {
			std::cout << "error with tree data\n"; exit(2);
		}

		if (0) {
			char** t = *screen;
			for (int j = 0; j < node->height; j++) {
				for (int i = 0; i < node->width; i++) {
					t[node->y + j][node->x + i] = color;
				}
			}
		}
	}
	else {
		fillScreen(screen, len, node->children[0], leafAmount);
		fillScreen(screen, len, node->children[1], leafAmount);
		fillScreen(screen, len, node->children[2], leafAmount);
		fillScreen(screen, len, node->children[3], leafAmount);
	}
}

#define BORDERS_ON	true
#define BORDERS_OFF	false
void	fillData(uint8_t* dst, QuadNode* node, int* leafAmount, int width, bool draw_borders, int threshold) {
	if (!node)
		return;
	//if (node->isLeaf()) {
	if (node->detail <= threshold) {
		//(*leafAmount)++;
		//std::cout << "leaf: " << node->width << "x" << node->height << " at " << node->x << ":" << node->y << std::endl;
		if (node->width == 0 || node->height == 0) {
			std::cout << "error with tree data\n"; exit(2);
		}
		if (node->width * node->height >= DEBUG_LEAF_AREA && DEBUG_LEAF && *leafAmount == 0) {
			std::cout << "Fill new leaf: " << node->width << "x" << node->height << " at " << node->x << ":" << node->y << "\t";
			std::cout << (int)node->pixel.r << "  \t" << (int)node->pixel.g << "  \t" << (int)node->pixel.b << std::endl;
		}
		for (int j = 0; j < node->height; j++) {
			for (int i = 0; i < node->width; i++) {
				unsigned int posx = node->x + i;
				unsigned int posy = node->y + j;
				unsigned int index = (posy * width + posx) * 3;
				//std::cout << ((posy * width + posx) * 3 + 0) << std::endl;
				//std::cout << ((posy * width + posx) * 3 + 1) << std::endl;
				//std::cout << ((posy * width + posx) * 3 + 2) << std::endl;
				if (draw_borders && (i == 0 || j == 0)) {
					dst[index + 0] = 0;
					dst[index + 1] = 0;
					dst[index + 2] = 0;
				}
				else {
					dst[index + 0] = node->pixel.r;
					dst[index + 1] = node->pixel.g;
					dst[index + 2] = node->pixel.b;
				}
			}
		}
	}
	else if (node->children) {
		fillData(dst, node->children[0], leafAmount, width, draw_borders, threshold);
		fillData(dst, node->children[1], leafAmount, width, draw_borders, threshold);
		fillData(dst, node->children[2], leafAmount, width, draw_borders, threshold);
		fillData(dst, node->children[3], leafAmount, width, draw_borders, threshold);
	}
}
#define THRESHOLD 0
QuadNode* textureToQuadTree(Texture* tex) {
	uint8_t* data = tex->getData();
	unsigned int	w = tex->getWidth();
	unsigned int	h = tex->getHeight();
	Pixel** pix = new Pixel * [h];
	for (size_t j = 0; j < h; j++) {
		pix[j] = new Pixel[w];
		for (size_t i = 0; i < w; i++) {
			pix[j][i].r = data[(j * w + i) * 3 + 0];
			pix[j][i].g = data[(j * w + i) * 3 + 1];
			pix[j][i].b = data[(j * w + i) * 3 + 2];
		}
	}
	std::cout << "pixel: " << sizeof(Pixel) << std::endl;

	QuadNode* root = new QuadNode(pix, 0, 0, w, h, THRESHOLD);
	std::cout << "root is leaf: " << (root->isLeaf() ? "true" : "false") << std::endl;

	return root;
}

class QuadTreeManager : public GameManager {
public:
	QuadTreeManager() : GameManager() {
		this->threshold = 0;
		this->draw_borders = BORDERS_OFF;
	}
	virtual ~QuadTreeManager() {}
	unsigned int	threshold;
	bool			draw_borders;
};

static void		keyCallback_quadTree(GLFWwindow* window, int key, int scancode, int action, int mods) {
	(void)window; (void)key; (void)scancode; (void)action; (void)mods;
	//std::cout << __PRETTY_FUNCTION__ << std::endl;

	if (action == GLFW_PRESS) {
		//std::cout << "GLFW_PRESS" << std::endl;
		QuadTreeManager* manager = static_cast<QuadTreeManager*>(glfwGetWindowUserPointer(window));
		if (!manager) {
			std::cout << "static_cast failed" << std::endl;
		} else if (manager->glfw) {
			if (key == GLFW_KEY_EQUAL) {
				manager->threshold++;
				manager->glfw->setTitle(std::to_string(manager->threshold).c_str());
			}
			else if (key == GLFW_KEY_MINUS && manager->threshold > 0) {
				manager->threshold--;
				manager->glfw->setTitle(std::to_string(manager->threshold).c_str());
			}
			else if (key == GLFW_KEY_ENTER)
				manager->draw_borders = !manager->draw_borders;
		}
	}
}

void scene_4Tree() {
	QuadTreeManager	manager;
	manager.glfw = new Glfw(WINX, WINY);
	manager.glfw->setTitle("Tests texture quadtree");
	manager.glfw->activateDefaultCallbacks(&manager);
	manager.glfw->func[GLFW_KEY_EQUAL] = keyCallback_quadTree;
	manager.glfw->func[GLFW_KEY_MINUS] = keyCallback_quadTree;
	manager.glfw->func[GLFW_KEY_ENTER] = keyCallback_quadTree;

	Texture* lena = new Texture(WIN32_VS_FOLDER + "images/lena.bmp");
	Texture* rgbtest = new Texture(WIN32_VS_FOLDER + "images/test_rgb.bmp");
	Texture* red = new Texture(WIN32_VS_FOLDER + "images/red.bmp");
	Texture* monkey = new Texture(WIN32_VS_FOLDER + "images/monkey.bmp");
	Texture* flower = new Texture(WIN32_VS_FOLDER + "images/flower.bmp");

	Texture* baseImage = flower;
	QuadNode* root = textureToQuadTree(baseImage);
	int w = baseImage->getWidth();
	int h = baseImage->getHeight();
	uint8_t* dataOctree = new uint8_t[w * h * 3];

	float size_coef = float(WINX) / float(baseImage->getWidth()) / 2.0f;
	UIImage	uiBaseImage(baseImage);
	uiBaseImage.setPos(0, 0);
	uiBaseImage.setSize(uiBaseImage.getTexture()->getWidth() * size_coef, uiBaseImage.getTexture()->getHeight() * size_coef);

	Texture* image4Tree = new Texture(dataOctree, w, h);

	UIImage	ui4Tree(image4Tree);
	ui4Tree.setPos(baseImage->getWidth() * size_coef, 0);
	ui4Tree.setSize(ui4Tree.getTexture()->getWidth() * size_coef, ui4Tree.getTexture()->getHeight() * size_coef);


	Fps	fps144(144);
	Fps	fps60(60);
	Fps* defaultFps = &fps144;

	std::cout << "Begin while loop" << endl;
	int	leafAmount = 0;
	while (!glfwWindowShouldClose(manager.glfw->_window)) {
		if (defaultFps->wait_for_next_frame()) {

			glfwPollEvents();
			//glfw.updateMouse();//to do before cam's events
			//cam.events(glfw, float(defaultFps->tick));

			fillData(dataOctree, root, &leafAmount, w, manager.draw_borders, manager.threshold);
			leafAmount = -1;
			//std::cout << "leafs: " << leafAmount << std::endl;
			//std::cout << "w * h * 3 = " << w << " * " << h << " * 3 = " << w * h * 3 << std::endl;
			image4Tree->updateData(dataOctree, root->width, root->height);

			// printFps();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			blitToWindow(nullptr, GL_COLOR_ATTACHMENT0, &uiBaseImage);
			blitToWindow(nullptr, GL_COLOR_ATTACHMENT0, &ui4Tree);
			glfwSwapBuffers(manager.glfw->_window);

			if (GLFW_PRESS == glfwGetKey(manager.glfw->_window, GLFW_KEY_ESCAPE))
				glfwSetWindowShouldClose(manager.glfw->_window, GLFW_TRUE);
		}
	}

	std::cout << "End while loop" << endl;
	std::cout << "deleting textures..." << endl;
	delete lena;
	delete red;
	delete rgbtest;
}

int		main(void) {
	check_paddings();
	// test_behaviors();
	//test_mult_mat4(); exit(0);
	std::cout << "____START____ : " << Misc::getCurrentDirectory() << std::endl;
	//	test_obj_loader();

	//scene1();
	//scene2();
	scene_4Tree();
	// while(1);

	return (EXIT_SUCCESS);
}
