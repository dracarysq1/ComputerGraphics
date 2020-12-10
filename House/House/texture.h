#pragma once
#include <glut.h>              // OpenGL32���ͷ�ļ�
#include<stdio.h>
struct TextureTga            // ����һ���ṹ��
{
	GLubyte *imageData;           // ͼ������ (���32bit)
	GLuint bpp;            // ÿһ���ص�ͼ����ɫ���
	GLuint width;            // ͼ����
	GLuint height;            // ͼ��߶�
	GLuint texID;            // ����ID
};

// ����BMP,JPG,GIF���ļ�(Load BMP,JPG,GIF...file)
bool BuildTexture(char *szPathName, GLuint &texid);

// ����TGA�ļ�(Load TGA file)
bool BuildTexture(char *filename, TextureTga *texture);