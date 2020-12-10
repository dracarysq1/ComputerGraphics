#pragma once
#define GLUT_DISABLE_ATEXIT_HACK
#include <glut.h>  


class particle {
	bool is_forever;//永生
	bool has_tex;//纹理或颜色(colour)
	float x, y;//位置(position)
	float size_x;//大小(size)
	float size_y;
	unsigned int texture;//纹理(texture)
	float speed_x;//速度(speed)
	float speed_y;
	float acc_x;//加速度(acceleration)
	float acc_y;
	float life;//生命
	float angle;//角度
	unsigned char color[3];//颜色
	friend class emitter;
	void draw(GLuint texture);//绘制粒子（纹理）(draw particles(texture))
	void draw();//绘制粒子（颜色）(draw particles(colour))
	void show();
public:
	particle();
	particle(float _size_x, float _size_y,
		float _speed_x, float _speed_y,
		float _acc_x, float _acc_y,
		float _life, float _angle, unsigned int _texture, bool _is_forever);
	particle(float _size_x, float _size_y,
		float _speed_x, float _speed_y,
		float _acc_x, float _acc_y,
		float _life, float _angle, unsigned char* _color, bool _is_forever);
};

class emitter {
	float x1, y1, x2, y2; //发射器位置(transmitter location)
	int speed;//发射速率(Launch speed)
	particle **p;//发射粒子(Emitted particles)
	particle* (*f)(); //初始化粒子的函数指针
public:
	void emit(particle* (init)());
	void update();
	emitter(int _speed, float _x1,
		float _x2, float _y1, float _y2);
};