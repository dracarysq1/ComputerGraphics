#ifndef _CPrerequisites_h_ 
#define _CPrerequisites_h_ 
#include <windows.h>
#include "GL/glu.h"
#include <iostream>
#include <math.h>
using namespace std;
class CVector3
{
public:
	CVector3() {}
	CVector3(float X, float Y, float Z)
	{
		x = X; y = Y; z = Z;
	}
	inline CVector3 operator+(CVector3 vVector)
	{
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}
	inline CVector3 operator-(CVector3 vVector)
	{
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	inline CVector3 operator-()
	{
		return CVector3(-x, -y, -z);
	}
	inline CVector3 operator*(float num)
	{
		return CVector3(x * num, y * num, z * num);
	}
	inline CVector3 operator/(float num)
	{
		return CVector3(x / num, y / num, z / num);
	}

	inline CVector3 operator^(const CVector3 &rhs) const
	{
		return CVector3(y * rhs.z - rhs.y * z, rhs.x * z - x * rhs.z, x * rhs.y - rhs.x * y);
	}

	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float v[3];
	};
};

// 定义2D点类，用于保存模型的UV纹理坐标 
class CVector2
{
public:
	float x, y;
};

// 面的结构定义 
struct tFace
{
	int vertIndex[3];      // 顶点索引 
	int coordIndex[3];      // 纹理坐标索引 
};

// 材质信息结构体 
struct tMaterialInfo
{
	char strName[255];      // 纹理名称 
	char strFile[255];      // 如果存在纹理映射，则表示纹理文件名称 
	BYTE color[3];        // 对象的RGB颜色 
	int texureId;        // 纹理ID 
	float uTile;        // u 重复 
	float vTile;        // v 重复 
	float uOffset;       // u 纹理偏移 
	float vOffset;        // v 纹理偏移 
};

// 对象信息结构体 
struct t3DObject
{
	int numOfVerts;      // 模型中顶点的数目 
	int numOfFaces;      // 模型中面的数目 
	int numTexVertex;      // 模型中纹理坐标的数目 
	int materialID;      // 纹理ID 
	bool bHasTexture;      // 是否具有纹理映射 
	char strName[255];      // 对象的名称 
	CVector3 *pVerts;      // 对象的顶点 
	CVector3 *pNormals;    // 对象的法向量 
	CVector2 *pTexVerts;    // 纹理UV坐标 
	tFace *pFaces;        // 对象的面信息 
};

// 模型信息结构体 
struct t3DModel
{
	UINT texture[MAX_TEXTURES];
	int numOfObjects;          // 模型中对象的数目 
	int numOfMaterials;          // 模型中材质的数目 
	vector<tMaterialInfo> pMaterials;  // 材质链表信息 
	vector<t3DObject> pObject;      // 模型中对象链表信息 
};



struct tIndices
{
	unsigned short a, b, c, bVisible;
};

// 保存块信息的结构 
struct tChunk
{
	unsigned short int ID;          // 块的ID     
	unsigned int length;          // 块的长度 
	unsigned int bytesRead;          // 需要读的块数据的字节数 
};




typedef struct tagBoundingBoxStruct
{
	CVector3  BoxPosMaxVertex;
	CVector3  BoxNegMaxVertex;
} BoundingBoxVertex2;


// 下面的函数求两点决定的矢量 
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2);
// 下面的函数两个矢量相加 
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2);

// 下面的函数处理矢量的缩放 
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler);
// 下面的函数返回两个矢量的叉积 
CVector3 Cross(CVector3 vVector1, CVector3 vVector2);

// 下面的函数规范化矢量 
CVector3 Normalize(CVector3 vNormal);

void DrawModel(t3DModel& Model);


////////////////////////////////////////////////////////////////////////// 
#define FRAND   (((float)rand()-(float)rand())/RAND_MAX) 
#define Clamp(x, min, max)  x = (x<min  ? min : x<max ? x : max); 

#define SQUARE(x)  (x)*(x) 
struct vector3_t
{
	vector3_t(float x, float y, float z) : x(x), y(y), z(z) {}
	vector3_t(const vector3_t &v) : x(v.x), y(v.y), z(v.z) {}
	vector3_t() : x(0.0f), y(0.0f), z(0.0f) {}

	vector3_t& operator=(const vector3_t &rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	// vector add 
	vector3_t operator+(const vector3_t &rhs) const
	{
		return vector3_t(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	// vector subtract 
	vector3_t operator-(const vector3_t &rhs) const
	{
		return vector3_t(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	// scalar multiplication 
	vector3_t operator*(const float scalar) const
	{
		return vector3_t(x * scalar, y * scalar, z * scalar);
	}

	// dot product 
	float operator*(const vector3_t &rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	// cross product 
	vector3_t operator^(const vector3_t &rhs) const
	{
		return vector3_t(y * rhs.z - rhs.y * z, rhs.x * z - x * rhs.z, x * rhs.y - rhs.x * y);
	}

	float& operator[](int index)
	{
		return v[index];
	}

	float Length()
	{
		float length = (float)sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));
		return (length != 0.0f) ? length : 1.0f;
	}

	/*****************************************************************************
	Normalize()

	Helper function to normalize vectors
	*****************************************************************************/
	vector3_t Normalize()
	{
		*this = *this * (1.0f / Length());
		return *this;
	}

	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float v[3];
	};
};

#endif