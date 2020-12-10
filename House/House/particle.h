#pragma once
#define GLUT_DISABLE_ATEXIT_HACK
#include <glut.h>  


class particle {
	bool is_forever;//����
	bool has_tex;//�������ɫ(colour)
	float x, y;//λ��(position)
	float size_x;//��С(size)
	float size_y;
	unsigned int texture;//����(texture)
	float speed_x;//�ٶ�(speed)
	float speed_y;
	float acc_x;//���ٶ�(acceleration)
	float acc_y;
	float life;//����
	float angle;//�Ƕ�
	unsigned char color[3];//��ɫ
	friend class emitter;
	void draw(GLuint texture);//�������ӣ�����(draw particles(texture))
	void draw();//�������ӣ���ɫ��(draw particles(colour))
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
	float x1, y1, x2, y2; //������λ��(transmitter location)
	int speed;//��������(Launch speed)
	particle **p;//��������(Emitted particles)
	particle* (*f)(); //��ʼ�����ӵĺ���ָ��
public:
	void emit(particle* (init)());
	void update();
	emitter(int _speed, float _x1,
		float _x2, float _y1, float _y2);
};