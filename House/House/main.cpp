#define _CRT_SECURE_NO_DEPRECATE //为了高版本VS中使用fopen（）函数
#include <GLAUX.H>
#include <glut.h>
#include <iostream>
#include <windows.h>
#include<time.h>  
#include"particle.h"
#include"texture.h"
#define PI 3.14159265359
#define MAX_CHAR        128

static GLuint texName[30];//存放纹理(Store texture)
static GLuint color[3];//存放纹理
static double Ox = 0;
static double Oy = 0;
static double Oz = 0;
int cargo1 = 1;
int carback1 = 1;
int cargo2 = 1;
int carback2 = 1;
static double direction = 0;//站立位置及前进方向；(Standing position and forward direction)
static double step = 0.4;
static GLfloat spin = 0.0;
static double car1 = 37;
static double car2 = -37;
static double car3 = 37;
static double car4 = -37;
bool light = false;
bool rolate = false;
bool snow = false;
GLUquadricObj*g_text;//曲面，制作一个包围房间的大球体做背景(Curved surface, make a large sphere surrounding the room as the background)

//纹理贴图(Texture map)
void loadTexture(char* filename, GLuint &texture)

{
	static GLint    ImageWidth;
	static GLint    ImageHeight;
	static GLint    PixelLength;
	static GLubyte* PixelData;
	// 打开文件(open file)
	FILE* pFile = fopen(filename, "rb");
	if (pFile == 0)
		exit(0);
	// 读取24位bmp图象的大小信息(read 24 bit bmp)
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&ImageWidth, sizeof(ImageWidth), 1, pFile);
	fread(&ImageHeight, sizeof(ImageHeight), 1, pFile);
	// 计算像素数据长度(Calculate pixel data length)
	PixelLength = ImageWidth * 3;
	while (PixelLength % 4 != 0)//bmp图像保证数据区域的长度是4的倍数
		++PixelLength;
	PixelLength *= ImageHeight;
	// 读取像素数据(Read pixel data)
	PixelData = (GLubyte*)malloc(PixelLength);
	if (PixelData == 0)	exit(0);
	fseek(pFile, 54, SEEK_SET);
	fread(PixelData, PixelLength, 1, pFile);
	// 关闭文件(close file)
	fclose(pFile);
	//加载纹理(Load texture)
	glGenTextures(1, &texture);//用来生成纹理的数量   存储纹理索引的第一个元素指针(The number of textures used to store the first element pointer of the texture index)

	glBindTexture(GL_TEXTURE_2D, texture);//绑定（指定）纹理对象
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//纹理过滤函数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ImageWidth, ImageHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, PixelData);

	//生成一个2D纹理（Texture）。bmp图片的像素顺序是BGR所以用GL_BGR_EXT来反向加载数据

}
//室内光照(Indoor lighting)
void lightinit(double x, double y)
{
	//材质反光性设置(Material reflectivity settings)
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };  //镜面反射参数(Specular reflection parameters)
	GLfloat mat_shininess[] = { 50.0 };               //高光指数(Specular Index)
	GLfloat light_position[] = { Ox, Oy, Oz-10, 1.0 };//灯位置(1,1,1), 最后1-开关
	GLfloat white_light[] = {1, 1, 1, 1.0 };
	GLfloat Light_Model_Ambient[] = { 0.2 , 0.2 , 0.2 , 1.0 }; //环境光参数(Ambient light parameters)

//	GLfloat spot_direction[] = { 0,0,-20 };
		//材质属性(Material attributes)
//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	//灯光设置(Lighting settings)
//	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, -10.0);//高光指数(Specular Index)
//	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);   //散射光属性(Scattered light properties)
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);  //镜面反射光(Specular reflection)
	
//	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);  //环境光参数(Ambient light parameters)

	if (light) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	else {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);//只用纹理(Texture only)
	}
	glEnable(GL_LIGHTING);   //开关:使用光(Switch: use light)
	glEnable(GL_LIGHT0);     //打开0#灯(Turn on 0# light)
	glEnable(GL_DEPTH_TEST); //打开深度测试(Open depth test)
	
}
//地球旋转(Earth spinning)
void spinDisplay(void)
{
	spin = spin + 0.08;
	if (spin > 360.0)
	{
		spin = spin - 360.0;
	}
	glutPostRedisplay();//glutPostRedisplay 标记当前窗口需要重新绘制。通过glutMainLoop下一次循环时，窗口显示将被回调以重新显示窗口的正常面板
}

//-------------------------------------------粒子----------
emitter *e1;
GLuint texture[1];
//视区    
float whRatio;
int wHeight = 0;
int wWidth = 0;

particle* init_particle()
{
	float size = rand() % 100 * 0.01f;
	particle* p = new particle(size, size, float(rand() % 10 - 4) / 80, float(rand() % 10 - 4) / 40,
		1.0 / 10000, -4.9 / 40000, rand() % 200, rand() % 360, texture[0], false);
	return p;
}

void drawScene()
{
	e1->update();


}
//初始化(initialization)
void init(void)

{
	srand(unsigned(time(nullptr)));
	glClearColor(0.0, 0.0, 0.0, 0.0);//清除颜色缓冲区，设置颜色(Clear the color buffer, set the color)
	glShadeModel(GL_FLAT);//设置着色模式 GL_FLAT 恒定着色，GL_SMOOTH光滑着色(Set the coloring mode GL_FLAT constant coloring, GL_SMOOTH smooth coloring)
	BuildTexture("snow.jpg", texture[0]);
	e1 = new emitter(500, -40, 40, 40, 40);
	e1->emit(init_particle);

	glEnable(GL_DEPTH_TEST);
	g_text = gluNewQuadric();//这句代码需放置在绘图及显示之前
	loadTexture("地板.bmp", texName[0]);//图片与程序源代码放置在同一目录下即可(Picture and program source code can be placed in the same directory)
	loadTexture("墙体.bmp", texName[1]);
	loadTexture("屋顶.bmp", texName[2]);
	loadTexture("音箱.bmp", texName[3]);
	loadTexture("地球.bmp", texName[4]);
	loadTexture("月亮.bmp", texName[5]);
	loadTexture("电视.bmp", texName[6]);
	loadTexture("电视柜子.bmp", texName[7]);
	loadTexture("电视墙.bmp", texName[8]);
	loadTexture("电视柜子四周.bmp", texName[9]);
	loadTexture("音箱四周.bmp", texName[10]);
	loadTexture("门.bmp", texName[11]);
	loadTexture("窗户.bmp", texName[12]);
	loadTexture("被子.bmp", texName[13]);
	loadTexture("枕头.bmp", texName[14]);
	loadTexture("水族箱.bmp", texName[15]);
	loadTexture("外面地板.bmp", texName[16]);
	loadTexture("蹲坑.bmp", texName[17]);
	loadTexture("沙发.bmp", texName[18]);
	loadTexture("沙发四周.bmp", texName[19]);
	loadTexture("路面.bmp", texName[20]);
	loadTexture("花.bmp", texName[21]);
	loadTexture("世界地图.bmp", texName[22]);
	loadTexture("风景画.bmp", texName[23]);
	loadTexture("red.bmp", color[0]);
	loadTexture("blue.bmp", color[1]);
	loadTexture("black.bmp", color[2]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//所读取的图像数据的行对齐方式
	

}

//左右面(Left and right)
void Left_right(float m ,float n,float x,float y1,float y2,float z1,float z2) {

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(x, y1, z1);
	glTexCoord2f(0.0, n); glVertex3f(x, y1, z2);
	glTexCoord2f(m, n); glVertex3f(x, y2, z2);
	glTexCoord2f(m, 0.0); glVertex3f(x, y2, z1);
	glEnd();
}
//前后面(Front and back)
void Before_back(float m, float n, float x1, float x2, float y, float z1, float z2) {

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(x1, y, z1);
	glTexCoord2f(0.0, n); glVertex3f(x1, y, z2);
	glTexCoord2f(m, n); glVertex3f(x2, y, z2);
	glTexCoord2f(m, 0.0); glVertex3f(x2, y, z1);
	glEnd();
}
//上下面(Up and down)
void Up_down(float m, float n, float x1, float x2, float y1, float y2, float z) {

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(x1, y1, z);
	glTexCoord2f(0.0, n); glVertex3f(x1, y2, z);
	glTexCoord2f(m, n); glVertex3f(x2, y2, z);
	glTexCoord2f(m, 0.0); glVertex3f(x2, y1, z);
	glEnd();
}
void drawcar() {
	//车下半部（Lower half of the car）
	glBindTexture(GL_TEXTURE_2D, color[1]);
	glBegin(GL_POLYGON);
	glVertex3f(-40.0f, 0.0f, 15.0f);
	glVertex3f(40.0f, 0.0f, 15.0f);
	glVertex3f(40.0f, -15.0f, 15.0f);
	glVertex3f(-40.0f, -15.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, 0.0f, 15.0f);
	glVertex3f(40.0f, -15.0f, 15.0f);
	glVertex3f(40.0f, -15.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-40.0f, 0.0f, -15.0f);
	glVertex3f(-40.0f, 0.0f, 15.0f);
	glVertex3f(-40.0f, -15.0f, 15.0f);
	glVertex3f(-40.0f, -15.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, -15.0f, -15.0f);
	glVertex3f(-40.0f, -15.0f, -15.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, color[0]);
	glBegin(GL_POLYGON);
	glVertex3f(-40.0f, 0.0f, 15.0f);
	glVertex3f(-40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, 0.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-40.0f, -15.0f, 15.0f);
	glVertex3f(-40.0f, -15.0f, -15.0f);
	glVertex3f(40.0f, -15.0f, -15.0f);
	glVertex3f(40.0f, -15.0f, 15.0f);
	glEnd();
	//车上半部（Upper part of the car）
	glBegin(GL_POLYGON);
	glVertex3f(-20.0f, 0.0f, 15.0f);
	glVertex3f(-10.0f, 10.0f, 15.0f);
	glVertex3f(20.0f, 10.0f, 15.0f);
	glVertex3f(25.0f, 0.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-20.0f, 0.0f, -15.0f);
	glVertex3f(-10.0f, 10.0f, -15.0f);
	glVertex3f(20.0f, 10.0f, -15.0f);
	glVertex3f(25.0f, 0.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-10.0f, 10.0f, 15.0f);
	glVertex3f(-10.0f, 10.0f, -15.0f);
	glVertex3f(20.0f, 10.0f, -15.0f);
	glVertex3f(20.0f, 10.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-10.0f, 10.0f, 15.0f);
	glVertex3f(-20.0f, 0.0f, 15.0f);
	glVertex3f(-20.0f, 0.0f, -15.0f);
	glVertex3f(-10.0f, 10.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(20.0f, 10.0f, 15.0f);
	glVertex3f(20.0f, 10.0f, -15.0f);
	glVertex3f(25.0f, 0.0f, -15.0f);
	glVertex3f(25.0f, 0.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-30.0f, -15.0f, 15.0f);
	glVertex3f(-30.0f, -15.0f, -15.0f);
	glVertex3f(30.0f, -15.0f, -15.0f);
	glVertex3f(30.0f, -15.0f, 15.0f);
	glEnd();
	//车轮（wheel）
	glBindTexture(GL_TEXTURE_2D,color[2]);
	glTranslated(-20.0f, -15.0f, 15.0f);
	glutSolidTorus(2, 5, 5, 100);
	glTranslated(0.0f, 0.0f, -30.0f);
	glutSolidTorus(2, 5, 5, 100);
	glTranslated(45.0f, 0.0f, 0.0f);
	glutSolidTorus(2, 5, 5, 100);
	glTranslated(0.0f, 0.0f, 30.0f);
	glutSolidTorus(2, 5, 5, 100);
}
//鼠标（mouse）
void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
		}
		break;
	case GLUT_MIDDLE_BUTTON:
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
	//		glutIdleFunc(nullptr);
		break;
	default:
		break;
	}
}
void tip() {
	std::cout << "操作说明：（Instructions）" << std::endl;

	std::cout << "上下键前进后退，左右键转头，WS上升下降（Press up and down keys to move forward and backward, left and right keys to turn head, WS goes up and down）" << std::endl;
	std::cout << "A键关掉房间灯（A key to turn off the room light）" << std::endl;
	std::cout << "C键开始下雪和停止下雪（C key to start snowing and stop snowing）" << std::endl;
	std::cout << "空格键地球旋转（Space bar earth rotation）" << std::endl;
}
void display(void)
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除颜色缓冲以及深度缓
	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture(g_text, GLU_TRUE);              //建立纹理坐标 
	gluQuadricDrawStyle(g_text, GLU_FILL);            //用面填充
	glPushMatrix();//2
	glRotatef((GLfloat)direction, 0.0, 0.0, 1.0);
	glTranslated(Ox, Oy,Oz);
	glPushMatrix();//3



	glPushMatrix();//4
	//太阳灯（sun）
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	GLfloat light1_ambient[] = { 1,1,1,1.0 };// 材料的环境光颜色
	GLfloat light1_diffuse[] = { 1,1,1,1.0 };
	GLfloat light1_specular[] = { 1,1,1,1.0 };//材料的镜面反射光颜色
	GLfloat light1_position[] = { Ox,Oy,Oz, 1.0 };//位置
	GLfloat spot_direction[] = { 20,0,0 };
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 90.0);//高光指数
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glEnable(GL_LIGHT1);
	glPopMatrix();//4

	glPushMatrix();//5
	glRotatef(spin, 0.0, 0.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, texName[4]);
	gluSphere(g_text, 60, 3600, 3600);//地球(earth)
		//------------月亮-------------(Moon)
	glPushMatrix();//6
	glRotatef((GLfloat)direction, 0.0, 0.0, 1.0);
	glTranslated(-100, 0, 0.0);
	glBindTexture(GL_TEXTURE_2D, texName[5]);
	gluSphere(g_text, 6, 200, 200);
	glPopMatrix();//6
	glPopMatrix();//5
	
	//光照压栈
	glPushMatrix();//7
	lightinit(Ox, Oy);
	glPopMatrix();//7

	glPopMatrix();//4

	//--------------------------汽车压栈
	//前
	glPushMatrix();//1
	if (cargo1==1) {
		car1 -=0.05;
	}
	else {
		car1 += 0.05;
	}
	if (car1 <= -37) {
		cargo1 = 0;
	}
	if (car1 >= 37) {
		cargo1 = 1;
	}
	glTranslated(car1, 37, 2);
	glRotated(90.0, 90.0, 0.0, 1.0);
	glScalef(0.1, 0.1, 0.1);
	drawcar();
	glPopMatrix();//1

	//右(right)
	glPushMatrix();//1
	if (cargo2 == 1) {
		car2 -= 0.05;
	}
	else {
		car2 += 0.05;
	}
	if (car2 <= -37) {
		cargo2 = 0;
	}
	if (car2 >= 37) {
		cargo2 = 1;
	}
	glTranslated(37, car2, 2);
	glRotated(90.0, 0.0, 0.0, 1.0);
	glRotated(90.0, 90.0, 0.0, 1.0);
	glScalef(0.1, 0.1, 0.1);
	drawcar();
	glPopMatrix();//1

	//左(left)
	glPushMatrix();//1
	if (carback1 == 1) {
		car3 -= 0.05;
	}
	else {
		car3 += 0.05;
	}
	if (car3 <= -37) {
		carback1 = 0;
	}
	if (car3 >= 37) {
		carback1 = 1;
	}
	glTranslated(-37, car3, 2);
	glRotated(90.0, 0.0, 0.0, 1.0);
	glRotated(90.0, 90.0, 0.0, 1.0);
	glScalef(0.1, 0.1, 0.1);
	drawcar();
	glPopMatrix();//1

	//后(Rear)
	glPushMatrix();
	glTranslated(0,-75, 0);
	glPushMatrix();//1
	if (carback2 == 1) {
		car4 -= 0.05;
	}
	else {
		car4 += 0.05;
	}
	if (car4 <= -37) {
		carback2 = 0;
	}
	if (car4 >= 37) {
		carback2 = 1;
	}
	glTranslated(car4, 37, 2);
	glRotated(90.0, 90.0, 0.0, 1.0);
	glScalef(0.1, 0.1, 0.1);
	drawcar();
	glPopMatrix();//1
	glPopMatrix();


	//外面(outside)
	glBindTexture(GL_TEXTURE_2D, texName[16]);
	Up_down(20.0, 20.0, -40.0, 40.0, -40.0, 40.0, -0.001);
	//地板(floor)
	glBindTexture(GL_TEXTURE_2D, texName[0]);
	Up_down(10.0, 10.0, -20.0, 20.0, -20.0, 20.0, 0.0);

	glBindTexture(GL_TEXTURE_2D, texName[20]);
	Up_down(10.0, 1.0, -40.0, 40.0, -40.0, -35, 0.0);
	Up_down(10.0, 1.0, -40.0, 40.0, 35, 40, 0.0);
	glPushMatrix();
	glRotated(90.0, 0.0, 0.0, 1.0);
	Up_down(10.0, 1.0, -40.0, 40.0, -40.0, -35, 0.0);
	glPopMatrix();
	glPushMatrix();
	glRotated(-90.0, 0.0, 0.0, 1.0);
	Up_down(10.0, 1.0, -40.0, 40.0, -40.0, -35, 0.0);
	glPopMatrix();

	//--------------室外墙----------------(Outdoor wall)
	glBindTexture(GL_TEXTURE_2D, texName[1]);
	//左边墙(left)
	Left_right(3.0, 1.0, -20.0,-20.0,20.0,0.0,10.0);
	//右边墙(right)
	Left_right(3.0, 1.0, 20.0, -20.0, 20.0, 0.0, 10.0);
	//后边墙(rear)
	Before_back(3.0, 1.0, -20.0, 20.0, -20.0, 0.0, 10.0);
	//前边墙(front)
	Before_back(3.0, 1.0, -20.0, 20.0, 20.0, 0.0, 10.0);

	//--------------屋顶----------------(roof)
	glBindTexture(GL_TEXTURE_2D, texName[2]);
	Up_down(4.0, 4.0, -20.0,20.0,-20.0,20.0,10.0);

	//门(door)
	glBindTexture(GL_TEXTURE_2D, texName[11]);
	Before_back(1.0, 1.0, -18.0, -15.0, 19.99, 0.0, 5.0);//大厅(hall)
	Before_back(1.0, 1.0, -18.0, -15.0, 20.01, 0.0, 5.0);//

	glBindTexture(GL_TEXTURE_2D, texName[11]);
	Before_back(1.0, 1.0, -8.0, -5.5, 4.99, 0.0, 5.0);//卫生间(wc)
	Before_back(1.0, 1.0, -8.0, -5.5, 5.01, 0.0, 5.0);//卫生间(wc)
	//---------------------------------------------------------------------大厅--------------------------------------------------(hall)
	//--------------电视墙----------------(TV Wall)

	glBindTexture(GL_TEXTURE_2D, texName[8]);
	Left_right(1.0, 1.0, 19.9, 0.0, -20.0, 0.0, 10.0);

	//--------------电视----------------(TV)

	glBindTexture(GL_TEXTURE_2D, texName[6]);
	Left_right(1.0, 1.0, 19.8, -6.0, -14.0, 3.0, 7.0);

	//--------------电视柜子----------------(TV cabinet)
	
	glBindTexture(GL_TEXTURE_2D, texName[7]);
	//前面(front)
	Left_right(1.0, 1.0, 18.0, -6.0, -14.0, 0.0, 2.3);
	//四周(All around)
	glBindTexture(GL_TEXTURE_2D, texName[9]);
	//左边(left)
	Before_back(1.0, 1.0, 18.0, 20.0,-6.0, 0.0, 2.3);
	//右边(right)
	Before_back(1.0, 1.0, 18.0, 20.0, -14.0, 0.0, 2.3);
	//上面(up)
	Up_down(1.0, 1.0, 18.0, 20.0, -6.0, -14.0, 2.3);

	//沙发组合-------------------------------------(sofa set)
	glBindTexture(GL_TEXTURE_2D, texName[19]);
	Left_right(1.0, 1.0, 5.0, -7.0, -12.0, 0.0, 3.0);
	Left_right(1.0, 1.0, 7.0, -7.0, -12.0, 0.0, 1.5);
	Before_back(1.0, 1.0, 5.5, 7.0, -7.0, 0.0, 1.5);
	Before_back(1.0, 1.0, 5.0, 5.5, -7.0, 0.0, 3);
	Before_back(1.0, 1.0, 5.5, 7.0, -12.0, 0.0, 1.5);
	Before_back(1.0, 1.0, 5.0, 5.5, -12.0, 0.0, 3);
	glBindTexture(GL_TEXTURE_2D, texName[18]);
	Up_down(1.0, 1.0, 5.0,7.0, -7.0, -12.0, 1.5);
	Left_right(1.0, 1.0, 5.5, -7.0, -12.0,1.5, 3.0);

	glPushMatrix();
	glRotatef(90, 0.0, 0.0, 1.0);
	glTranslated(-20,0, 0.0);
	glBindTexture(GL_TEXTURE_2D, texName[19]);
	Left_right(1.0, 1.0, 5.0, -7.0, -12.0, 0.0, 3.0);
	Left_right(1.0, 1.0, 7.0, -7.0, -12.0, 0.0, 1.5);
	Before_back(1.0, 1.0, 5.5, 7.0, -7.0, 0.0, 1.5);
	Before_back(1.0, 1.0, 5.0, 5.5, -7.0, 0.0, 3);
	Before_back(1.0, 1.0, 5.5, 7.0, -12.0, 0.0, 1.5);
	Before_back(1.0, 1.0, 5.0, 5.5, -12.0, 0.0, 3);
	glBindTexture(GL_TEXTURE_2D, texName[18]);
	Up_down(1.0, 1.0, 5.0, 7.0, -7.0, -12.0, 1.5);
	Left_right(1.0, 1.0, 5.5, -7.0, -12.0, 1.5, 3.0);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-90, 0.0, 0.0, 1.0);
	glTranslated(-1, 19, 0.0);
	glBindTexture(GL_TEXTURE_2D, texName[19]);
	Left_right(1.0, 1.0, 5.0, -7.0, -12.0, 0.0, 3.0);
	Left_right(1.0, 1.0, 7.0, -7.0, -12.0, 0.0, 1.5);
	Before_back(1.0, 1.0, 5.5, 7.0, -7.0, 0.0, 1.5);
	Before_back(1.0, 1.0, 5.0, 5.5, -7.0, 0.0, 3);
	Before_back(1.0, 1.0, 5.5, 7.0, -12.0, 0.0, 1.5);
	Before_back(1.0, 1.0, 5.0, 5.5, -12.0, 0.0, 3);
	glBindTexture(GL_TEXTURE_2D, texName[18]);
	Up_down(1.0, 1.0, 5.0, 7.0, -7.0, -12.0, 1.5);
	Left_right(1.0, 1.0, 5.5, -7.0, -12.0, 1.5, 3.0);
	glPopMatrix();


	//--------------门口柜子----------------(Door cabinet)

	glBindTexture(GL_TEXTURE_2D, texName[7]);
	//前面(front)
	Left_right(2.0, 1.0, -12.0,20.0, 5.0, 0.0, 2.3);
	Left_right(2.0, 1.0, -12.0, 20.0, 5.0, 7.7,10.0);
	//四周(all around)
	glBindTexture(GL_TEXTURE_2D, texName[9]);
	Before_back(1.0, 1.0, -12.0, -10.0, 5.0, 0.0, 2.3);
	//上面(up)
	Up_down(2.0, 1.0, -12.0, -10.0, 20.0,5.0, 2.3);
	//四周(all around)
	Before_back(1.0, 1.0, -12.0, -10.0, 5.0, 7.7,10.0);
	//下面(down)
	Up_down(2.0, 1.0, -12.0, -10.0, 20.0, 5.0, 7.7);

	//--------------左音响----------------(Left speaker)
	
	glBindTexture(GL_TEXTURE_2D, texName[3]);
	//前面
	Left_right(1.0, 1.0, 19.0, -3.5, -4.5, 0.0, 4.0);
	//四周
	glBindTexture(GL_TEXTURE_2D, texName[10]);
	//左边
	Before_back(1.0, 1.0, 19.0, 20.0, -3.5, 0.0, 4.0);
	//右边
	Before_back(1.0, 1.0, 19.0, 20.0, -4.5, 0.0, 4.0);
	//上面
	Up_down(1.0, 1.0, 19.0, 20.0, -3.5, -4.5, 4.0);

	//--------------右音响---------------(right speaker)
	glBindTexture(GL_TEXTURE_2D, texName[3]);
	//前面
	Left_right(1.0, 1.0, 19.0, -15.5, -16.5, 0.0, 4.0);
	//四周
	glBindTexture(GL_TEXTURE_2D, texName[10]);
	//左边
	Before_back(1.0, 1.0, 19.0, 20.0, -15.5, 0.0, 4.0);
	//右边
	Before_back(1.0, 1.0, 19.0, 20.0, -16.5, 0.0, 4.0);
	//上面
	Up_down(1.0, 1.0, 19.0, 20.0, -15.5, -16.5, 4.0);

	//--------------餐桌------------------(dining table)
	glBindTexture(GL_TEXTURE_2D, texName[9]);
	Up_down(2.0, 2.0, -15.0, -13.0, -13, -15, 2.0);
	Up_down(2.0,2.0, -15.0, -13.0, -13, -15, 1.9);
	Before_back(1.0, 1.0, -15.0, -13.0, -15.0, 1.9, 2.0);
	Before_back(1.0, 1.0, -13.0, -13.0, -15.0, 1.9, 2.0);
	Left_right(1.0, 1.0, -15.0, -13.0, -15.0, 1.9, 2.0);
	Left_right(1.0, 1.0, -13.0, -13.0, -15.0, 1.9, 2.0);
	//逆时针
	//1
	Before_back(1.0, 1.0, -14.8, -14.6, -14.8, 0.0, 2.0);
	Before_back(1.0, 1.0, -14.8, -14.6, -14.6, 0.0, 2.0);
	Left_right(1.0, 1.0, -14.6, -14.6, -14.8, 0.0, 2.0);
	Left_right(1.0, 1.0, -14.8, -14.6, -14.8, 0.0, 2.0);
	//2
	Before_back(1.0, 1.0, -13.4, -13.2, -14.8, 0.0, 2.0);
	Before_back(1.0, 1.0, -13.4, -13.2, -14.6, 0.0, 2.0);
	Left_right(1.0, 1.0, -13.4, -14.6, -14.8, 0.0, 2.0);
	Left_right(1.0, 1.0, -13.2, -14.6, -14.8, 0.0, 2.0);
	//3
	Before_back(1.0,  1.0, -13.4, -13.2, -13.3, 0.0, 2.0);
	Before_back(1.0, 1.0, -13.4, -13.2, -13.2, 0.0, 2.0);
	Left_right(1.0,1.0, -13.4, -13.4, -13.2, 0.0, 2.0);
	Left_right(1.0,  1.0, -13.2, -13.4, -13.2, 0.0, 2.0);
	//4
	Before_back(1.0, 1.0, -14.8, -14.6, -13.3, 0.0, 2.0);
	Before_back(1.0, 1.0, -14.6, -14.6, -13.2, 0.0, 2.0);
	Left_right(1.0, 1.0, -14.6, -13.4, -13.2, 0.0, 2.0);
	Left_right(1.0, 1.0, -14.8, -13.4, -13.2, 0.0, 2.0);

	//------------------------椅子------------(chair)
	Up_down(1.0, 1.0, -14.3, -13.7, -16.0, -15.4, 1.3);
	Before_back(1.0, 1.0, -14.3, -13.7, -16.0, 0.0, 1.3);
	Before_back(1.0, 1.0, -14.3, -13.7, -15.4, 0.0, 1.3);
	Left_right(1.0, 1.0, -13.7, -16.0, -15.4, 0.0, 1.3);
	Left_right(1.0, 1.0, -14.3, -16.0, -15.4, 0.0, 1.3);


	Up_down(1.0, 1.0, -14.3, -13.7, -12.6, -12, 1.3);
	Before_back(1.0, 1.0, -14.3, -13.7, -12.6, 0.0, 1.3);
	Before_back(1.0, 1.0, -14.3, -13.7, -12, 0.0, 1.3);
	Left_right(1.0, 1.0, -13.7, -12.6, -12, 0.0, 1.3);
	Left_right(1.0, 1.0, -14.3, -12.6, -12, 0.0, 1.3);

	Up_down(1.0, 1.0, -12.6, -12, -14.3, -13.7, 1.3);
	Before_back(1.0, 1.0, -12.6, -12, -14.3, 0.0, 1.3);
	Before_back(1.0, 1.0, -12.6, -12, -13.7, 0.0, 1.3);
	Left_right(1.0, 1.0, -12.6, -14.3, -13.7, 0.0, 1.3);
	Left_right(1.0, 1.0, -12.0, -14.3, -13.7, 0.0, 1.3);

	Up_down(1.0, 1.0, -16.0, -15.4, -14.3, -13.7, 1.3);
	Before_back(1.0, 1.0, -16.0, -15.4, -14.3, 0.0, 1.3);
	Before_back(1.0, 1.0, -16.0, -15.4, -13.7, 0.0, 1.3);
	Left_right(1.0, 1.0, -16.0, -14.3, -13.7, 0.0, 1.3);
	Left_right(1.0, 1.0, -15.4, -14.3, -13.7, 0.0, 1.3);

	//水族箱(Aquarium)

	Before_back(1.0, 1.0, -5.0, -3, -5, 0.0, 1.0);
	Before_back(1.0, 1.0, -5.0,-3.0, -15, 0.0, 1.0);
	Left_right(1.0, 1.0, -5.0, -5.0, -15.0, 0.0, 1.0);
	Left_right(1.0, 1.0, -3.0, -5.0, -15.0, 0.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, texName[15]);
	Before_back(1.0, 1.0, -5.0, -3.0, -5, 1.0, 5.0);
	Before_back(1.0, 1.0, -5.0, -3.0, -15, 1.0, 5.0);
	Left_right(1.0, 1.0, -5.0, -5.0, -15.0, 1.0, 5.0);
	Left_right(1.0, 1.0, -3.0, -5.0, -15.0, 1.0, 5.0);

	//全家福(Family portrait)
	glBindTexture(GL_TEXTURE_2D, texName[21]);
	Before_back(1.0, 1.0, 3.0, 13.0, -0.01, 2.0, 8.0);//
	Before_back(1.0, 1.0, 3.0, 13.0, -19.99, 2.0, 8.0);//

	glBindTexture(GL_TEXTURE_2D, texName[22]);
	Before_back(1.0, 1.0, 3.0, 13.0, 0.01, 2.0, 8.0);//

	//-------------------------------------------------------卫生间--------------------------------------------------------(wc)
	glBindTexture(GL_TEXTURE_2D, texName[1]);
	Before_back(2.0, 1.0, -10.0, -3.0, 5.0, 0.0, 10.0);//卫生间墙(wc wall)
	Left_right(2.0, 1.0, -10.0, 5.0, 20.0, 0.0, 10.0);//大门墙(door wall)
	glBindTexture(GL_TEXTURE_2D, texName[16]);
	Up_down(1.0,1.0,-10.0,-3.0,20.0,15.0,1);//蹲坑
	Before_back(1.0, 1.0, -10.0, -3.0, 15.0, 0.0, 1);//
	glBindTexture(GL_TEXTURE_2D, texName[17]);
	Up_down(1.0, 1.0, -8.0, -5.0, 19.0, 16.0, 1.001);//蹲坑

	//----------------------------------------------------------房间-------------------------------------------------------(room)
	glBindTexture(GL_TEXTURE_2D, texName[1]);
   //前边墙
	Before_back(2.0, 1.0, -3.0, 20.0, 0.0, 0.0, 10.0);//
	//左边墙	
	Left_right(2.0, 1.0, -3.0, 20.0, 0.0, 0.0, 10.0);//房门墙
	//门
	glBindTexture(GL_TEXTURE_2D, texName[11]);
	Left_right(1.0, 1.0, -2.99, 1.0, 3.5, 0.0, 5.0);
	Left_right(1.0, 1.0, -3.01, 1.0, 3.5, 0.0, 5.0);
	//窗(windows)
	glBindTexture(GL_TEXTURE_2D, texName[12]);
	Before_back(1.0, 1.0, 0.0, 7.0, 19.99, 3.0, 7.0);

	//婚纱照
	glBindTexture(GL_TEXTURE_2D, texName[23]);
	Before_back(1.0, 1.0, 14.0, 19.0, 19.99, 3.0, 7.0);
	//-------床----------(bed)
	glBindTexture(GL_TEXTURE_2D, texName[10]);
	Before_back(1.0, 1.0, 13.0, 20.0, 19.3, 0.0, 2.5);
	Left_right(1.0, 1.0, 13.0, 20.0, 19.3, 0.0, 2.5);
	Up_down(1.0, 1.0, 13.0, 20.0, 20.0, 19.3, 2.5);
	Up_down(1.0, 1.0, 13.0, 20.0, 20.0, 11.0, 1.0);
	Left_right(1.0, 1.0, 13.0, 20.0,11.0, 0.0, 1.0);
	Before_back(1.0, 1.0, 13.0, 20.0,11.0, 0.0, 1.0);
	//被子(quilt)
	glBindTexture(GL_TEXTURE_2D, texName[13]);
	Up_down(1.0, 1.0, 12.99, 20.0, 17.0, 11.0, 1.01);
	Left_right(1.0, 1.0, 12.99,17.0, 11.0, 0.0, 1.01);
	//枕头(pillow)
	glBindTexture(GL_TEXTURE_2D, texName[14]);
	Before_back(1.0, 1.0, 17.0, 19.0, 18.0, 0.0, 1.5);
	Left_right(1.0, 1.0, 17.0, 20.0, 18.0, 0.0,1.5);
	Up_down(1.0, 1.0, 17.0, 19.0, 20.0, 18.0, 1.5);

	Before_back(1.0, 1.0, 14.0, 16.0, 18.0, 0.0, 1.5);
	Left_right(1.0, 1.0, 14.0, 20.0, 18.0, 0.0, 1.5);
	Up_down(1.0, 1.0, 14.0, 16.0, 20.0, 18.0, 1.5);

	//学习桌(Study desk)
	glBindTexture(GL_TEXTURE_2D, texName[10]);
	Up_down(1.0, 1.0,6.0,9.0,1.5, 0.0, 2.0);
	Up_down(1.0, 1.0, 6.0, 9.0, 1.5, 0.0, 1.8);
	Left_right(1.0, 1.0, 6.0, 1.5, 0.0, 0.0, 2);
	Left_right(1.0, 1.0, 9.0, 1.5, 0.0, 0.0, 2);
	Left_right(1.0, 1.0, 6.2, 1.5, 0.0, 0.0, 2);
	Left_right(1.0, 1.0, 8.8, 1.5, 0.0, 0.0, 2);
	Before_back(1.0, 1.0, 6.0, 6.2, 1.51, 0.0, 2.0);
	Before_back(1.0, 1.0, 8.8, 9.0, 1.51, 0.0, 2.0);
	Before_back(1.0, 1.0, 6.0, 9.0, 1.51, 1.8, 2.0);

	Up_down(1.0, 1.0, 7.2, 8.3, 2.0, 2.8, 1);
	Left_right(1.0, 1.0, 7.2, 2.0, 2.8, 0.0, 1);
	Left_right(1.0, 1.0, 8.3, 2.0, 2.8, 0.0, 1);
	Before_back(1.0, 1.0, 7.2, 8.3, 2.8, 0.0, 2.0);
	Before_back(1.0, 1.0, 7.2, 8.3, 2.6, 0.0, 2.0);
	Left_right(1.0, 1.0, 7.2, 2.6, 2.8, 0.0, 2);
	Left_right(1.0, 1.0, 8.3, 2.6, 2.8, 0.0, 2);
	Up_down(1.0, 1.0, 7.2, 8.3, 2.6, 2.8, 2);

	if (snow) {
		//雪花(snow)
		glPushMatrix();
		glRotated(90.0, 90.0, 0.0, 1.0);
		drawScene();//绘制场景(Draw the scene)
		glPopMatrix();//
		glPushMatrix();
		glTranslated(0, 20, 0);
		glRotated(90.0, 90.0, 0.0, 1.0);
		drawScene();//绘制场景
		glPopMatrix();//
		glPushMatrix();
		glTranslated(0, -20, 0);
		glRotated(90.0, 90.0, 0.0, 1.0);
		drawScene();//绘制场景
		glPopMatrix();//
		glTranslated(0, 30, 0);
		glRotated(90.0, 90.0, 0.0, 1.0);
		drawScene();//绘制场景
		glPopMatrix();//
	}
	glPopMatrix();//3
	glPopMatrix();//2
	glFlush();//用于强制刷新缓存
	glutSwapBuffers();//交换双缓存
	glutPostRedisplay();//标志下次刷新，没有的话，程序打开后不会刷新界面
}
void reshape(int w, int h)

{

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);//在默认情况下，视口被设置为占据打开窗口的整个像素矩形
	glMatrixMode(GL_PROJECTION);//选择投影矩阵
	glLoadIdentity();//重置当前指定的矩阵为单位矩阵
	gluPerspective(90.0, (GLfloat)w / (GLfloat)h, 0.1, 500.0);//角度，宽高比，z轴近处，远处。
	glMatrixMode(GL_MODELVIEW);//模型视景矩阵||GL_TEXTURE,对纹理矩阵堆栈应用随后的矩阵操作.
	glLoadIdentity();
	gluLookAt(0, 0, 3, 0, 10, 3, 0.0, 0.0, 1.0);
	if (h == 0) { //如果高度为0   
		h = 1;   //让高度为1（避免出现分母为0的现象）        
	}
	wHeight = h;
	wWidth = w;

}
//键盘
void keyboard(unsigned char key, int x, int y)

{
	switch (key) {

	case 65://A灯开关(A light switch)
		if (light)
			light = false;
		else
			light = true;
		break;

	case 83://W键上升(W key up)
		Oz+=1.0f;
		break;
	case 87://S键下降(S key down)
		Oz -= 1.0f;
		break;

	case 27:
		exit(0);
		break;

	case 32://空格旋转(Space rotation)
		if (!rolate) {		
			glutIdleFunc(spinDisplay);  //设备空闲时调用的函数
			rolate =true;
		}
		else {		
			glutIdleFunc(nullptr);
			rolate = false;
		}
		break;
		case 67://C键开关雪(C key switch snow)
			if (snow) {
				snow = false;
			}
			else {
				snow = true;
			}
			break;

	default:
		break;

	}
}
//特殊按钮
void SpecialKey(GLint key, GLint x, GLint y) {
	switch (key) {

	case GLUT_KEY_UP:
		
		
		Ox -= step * sin(direction / 180 * PI);
		
		Oy -= step * cos(direction / 180 * PI);

		

		glutPostRedisplay();

		break;

	case GLUT_KEY_DOWN:

		
		Ox += step * sin(direction / 180 * PI);

		
		Oy += step * cos(direction / 180 * PI);


		glutPostRedisplay();

		break;

	case GLUT_KEY_LEFT:

		direction = direction - 2;
		glutPostRedisplay();

		break;

	case GLUT_KEY_RIGHT:

		
		direction = direction + 2;

		glutPostRedisplay();

		break;

	default:

		break;

	}
}



int main(int argc, char* argv[])

{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	tip();
	init();
	glutDisplayFunc(display);//回调函数
	glutMouseFunc(mouse);//鼠标函数
	glutReshapeFunc(reshape);
	glutKeyboardFunc(&keyboard);//键盘
	glutSpecialFunc(&SpecialKey);//键盘
	glutMainLoop();
	return 0;

}