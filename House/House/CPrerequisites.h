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

// ����2D���࣬���ڱ���ģ�͵�UV�������� 
class CVector2
{
public:
	float x, y;
};

// ��Ľṹ���� 
struct tFace
{
	int vertIndex[3];      // �������� 
	int coordIndex[3];      // ������������ 
};

// ������Ϣ�ṹ�� 
struct tMaterialInfo
{
	char strName[255];      // �������� 
	char strFile[255];      // �����������ӳ�䣬���ʾ�����ļ����� 
	BYTE color[3];        // �����RGB��ɫ 
	int texureId;        // ����ID 
	float uTile;        // u �ظ� 
	float vTile;        // v �ظ� 
	float uOffset;       // u ����ƫ�� 
	float vOffset;        // v ����ƫ�� 
};

// ������Ϣ�ṹ�� 
struct t3DObject
{
	int numOfVerts;      // ģ���ж������Ŀ 
	int numOfFaces;      // ģ���������Ŀ 
	int numTexVertex;      // ģ���������������Ŀ 
	int materialID;      // ����ID 
	bool bHasTexture;      // �Ƿ��������ӳ�� 
	char strName[255];      // ��������� 
	CVector3 *pVerts;      // ����Ķ��� 
	CVector3 *pNormals;    // ����ķ����� 
	CVector2 *pTexVerts;    // ����UV���� 
	tFace *pFaces;        // ���������Ϣ 
};

// ģ����Ϣ�ṹ�� 
struct t3DModel
{
	UINT texture[MAX_TEXTURES];
	int numOfObjects;          // ģ���ж������Ŀ 
	int numOfMaterials;          // ģ���в��ʵ���Ŀ 
	vector<tMaterialInfo> pMaterials;  // ����������Ϣ 
	vector<t3DObject> pObject;      // ģ���ж���������Ϣ 
};



struct tIndices
{
	unsigned short a, b, c, bVisible;
};

// �������Ϣ�Ľṹ 
struct tChunk
{
	unsigned short int ID;          // ���ID     
	unsigned int length;          // ��ĳ��� 
	unsigned int bytesRead;          // ��Ҫ���Ŀ����ݵ��ֽ��� 
};




typedef struct tagBoundingBoxStruct
{
	CVector3  BoxPosMaxVertex;
	CVector3  BoxNegMaxVertex;
} BoundingBoxVertex2;


// ����ĺ��������������ʸ�� 
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2);
// ����ĺ�������ʸ����� 
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2);

// ����ĺ�������ʸ�������� 
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler);
// ����ĺ�����������ʸ���Ĳ�� 
CVector3 Cross(CVector3 vVector1, CVector3 vVector2);

// ����ĺ����淶��ʸ�� 
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