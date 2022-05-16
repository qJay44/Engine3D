#include <vector>
#include <iostream>
#include <fstream>
#include <strstream>
#include <algorithm>
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

using namespace std;

void showLines();
void onUpdate(sf::RenderWindow&);
void onCreate(const unsigned int, const unsigned int);
void moveCamera(int);

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "3D Engine", sf::Style::Close);

	onCreate(window.getSize().x, window.getSize().y);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

			// change color mode
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::C)
				showLines();

        }

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveCamera(0);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveCamera(1);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveCamera(2);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveCamera(3);

        window.clear();

        onUpdate(window);

        window.display();
    }

    return 0;
}

struct vec3d
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;
};

struct triangle
{
    vec3d p[3];
	int col = 0;
};

struct mesh
{
    vector<triangle> tris;

	bool LoadFromObjectFile(string sFilename)
	{
		ifstream f(sFilename);
		if (!f.is_open()) return false;

		// Local cache of verts
		vector<vec3d> verts;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				vec3d v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}

		return true;
	}
};

struct mat4x4
{
    float m[4][4] = { 0 };
};

mesh meshCube;
mat4x4 matProj;

vec3d vCamera;
vec3d vLookDir;

bool drawLines = false;
float fTheta;

void moveCamera(int dir)
{
	float step = 8.f * 75.f / 1000.f / 10.f;
	switch (dir)
	{
	case 0:
		vCamera.y += step;
		break;

	case 1:
		vCamera.x += step;
		break;

	case 2:
		vCamera.y -= step;
		break;

	case 3:
		vCamera.x -= step;
		break;

	default:
		break;
	}
}

void showLines()
{
	drawLines = !drawLines;
}

vec3d Matrix_MultiplyVector(mat4x4& m, vec3d& i)
{
	vec3d v;
	v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
	v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
	v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
	v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
	
	return v;
}

// Only for Rotation/Translation Matrices
mat4x4 Matrix_QuickInverse(mat4x4& m)
{
	mat4x4 matrix;
	matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.f;
	matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.f;
	matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.f;
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);

	return matrix;
}

mat4x4 Matrix_MakeIdentity()
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.f;
	matrix.m[1][1] = 1.f;
	matrix.m[2][2] = 1.f;
	matrix.m[3][3] = 1.f;

	return matrix;
}

mat4x4 Matrix_MakeRotationX(float AngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.f;
	matrix.m[1][1] = cosf(AngleRad);
	matrix.m[1][2] = sinf(AngleRad);
	matrix.m[2][1] = -sinf(AngleRad);
	matrix.m[2][2] = cosf(AngleRad);
	matrix.m[3][3] = 1.f;

	return matrix;
}

mat4x4 Matrix_MakeRotationY(float AngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = cosf(AngleRad);
	matrix.m[0][2] = sinf(AngleRad);
	matrix.m[2][0] = -sinf(AngleRad);
	matrix.m[1][1] = 1.f;
	matrix.m[2][2] = cosf(AngleRad);
	matrix.m[3][3] = 1.f;

	return matrix;
}

mat4x4 Matrix_MakeRotationZ(float AngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = cosf(AngleRad);
	matrix.m[0][1] = sinf(AngleRad);
	matrix.m[1][0] = -sinf(AngleRad);
	matrix.m[1][1] = cosf(AngleRad);
	matrix.m[2][2] = 1.f;
	matrix.m[3][3] = 1.f;

	return matrix;
}

mat4x4 Matrix_MakeTranslation(float x, float y, float z)
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.f;
	matrix.m[1][1] = 1.f;
	matrix.m[2][2] = 1.f;
	matrix.m[3][3] = 1.f;
	matrix.m[3][0] = x;
	matrix.m[3][1] = y;
	matrix.m[3][2] = z;

	return matrix;
}

mat4x4 Matrix_MakeProjection(float FovDegrees, float AspectRatio, float Near, float Far)
{
	float FovRad = 1.f / tanf(FovDegrees * 0.5f / 180.f * 3.14159f);
	mat4x4 matrix;

	matrix.m[0][0] = AspectRatio * FovRad;
	matrix.m[1][1] = FovRad;
	matrix.m[2][2] = Far / (Far - Near);
	matrix.m[3][2] = (-Far * Near) / (Far - Near);
	matrix.m[2][3] = 1.f;
	matrix.m[3][3] = 0.f;

	return matrix;
}

mat4x4 Matrix_MultiplyMatrix(mat4x4& m1, mat4x4& m2)
{
	mat4x4 matrix;
	for (int c = 0; c < 4; c++)
	{
		for (int r = 0; r < 4; r++)
		{
			matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] +
							 m1.m[r][1] * m2.m[1][c] +
							 m1.m[r][2] * m2.m[2][c] +
							 m1.m[r][3] * m2.m[3][c];
		}
	}

	return matrix;
}

vec3d Vector_Add(vec3d& v1, vec3d& v2)
{
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

vec3d Vector_Sub(vec3d& v1, vec3d& v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

vec3d Vector_Mul(vec3d& v1, float k)
{
	return { v1.x * k, v1.y * k, v1.z * k };
}

vec3d Vector_Div(vec3d& v1, float k)
{
	return { v1.x / k, v1.y / k, v1.z / k };
}

float Vector_DotProduct(vec3d& v1, vec3d& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Vector_Length(vec3d& v)
{
	return sqrtf(Vector_DotProduct(v, v));
}

vec3d Vector_Normalize(vec3d& v)
{
	float l = Vector_Length(v);
	return { v.x / l, v.y / l, v.z / l };
}

vec3d Vector_CrossProduct(vec3d& v1, vec3d& v2)
{
	vec3d v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;

	return v;
}

mat4x4 Matrix_PointAt(vec3d& pos, vec3d& target, vec3d& up)
{
	// Calculate new forward direction
	vec3d newForward = Vector_Sub(target, pos);
	newForward = Vector_Normalize(newForward);

	// Calculate new Up direction
	vec3d a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
	vec3d newUp = Vector_Sub(up, a);
	newUp = Vector_Normalize(newUp);

	// New Right direction is easy, its just cross product
	vec3d newRight = Vector_CrossProduct(newUp, newForward);

	// Construct Dimensioning and Translation Matrtix
	mat4x4 matrix;
	matrix.m[0][0] = newRight.x;
	matrix.m[1][0] = newUp.x;
	matrix.m[2][0] = newForward.x;
	matrix.m[3][0] = pos.x;

	matrix.m[0][1] = newRight.y;
	matrix.m[1][1] = newUp.y;
	matrix.m[2][1] = newForward.y;
	matrix.m[3][1] = pos.y;

	matrix.m[0][2] = newRight.z;
	matrix.m[1][2] = newUp.z;
	matrix.m[2][2] = newForward.z;
	matrix.m[3][2] = pos.z;

	return matrix;
}

void onCreate(const unsigned int w, const unsigned int h)
{
	meshCube.LoadFromObjectFile("spaceshit.obj");

	// Projection Matrix
	matProj = Matrix_MakeProjection(90.f, (float)h / (float)w, 0.1f, 1000.f);
}

void onUpdate(sf::RenderWindow& win)
{
	const unsigned static int w = win.getSize().x;
	const unsigned static int h = win.getSize().y;

	mat4x4 matRotZ, matRotX;
	//fTheta += 1.0f * 75.f / 1000.f / 10.f;

	matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
	matRotX = Matrix_MakeRotationX(fTheta);

	mat4x4 matTrans;
	matTrans = Matrix_MakeTranslation(0.f, 0.f, 16.f);

	mat4x4 matWorld;
	matWorld = Matrix_MakeIdentity();
	matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX);
	matWorld = Matrix_MultiplyMatrix(matWorld, matTrans);

	vLookDir = { 0.f, 0.f, 1.f };
	vec3d vUp = { 0.f, 1.f, 0.f };
	vec3d vTarget = Vector_Add(vCamera, vLookDir);

	mat4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);

	// Make view matrix from camera
	mat4x4 matView = Matrix_QuickInverse(matCamera);

	// Store triangles for rastering later
	vector<triangle> vecTrianglesToRaster;

	// Draw Triangles
	for (auto& tri : meshCube.tris)
	{
		triangle triProjected, triTransformed, triViewed;

		triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
		triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
		triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);

		// Calculate triangle Normal
		vec3d normal, line1, line2;

		// Get lines either side of triangle
		line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
		line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);

		// Take cross product of lines to get normal to triangle surface
		normal = Vector_CrossProduct(line1, line2);

		// You normally need to normalise a normal
		normal = Vector_Normalize(normal);

		// Get ray form triangle to camera
		vec3d vCameraRay = Vector_Sub(triTransformed.p[0], vCamera);

		// If ray is aligned with normal, then triangle is visible
		if (Vector_DotProduct(normal, vCameraRay) < 0.f)
		{
			// Illumination
			vec3d light_direction = { 0.f, 1.f, -1.f };
			light_direction = Vector_Normalize(light_direction);

			// How "aligned" are light direction and triangle surface noraml?
			float dp = max(0.1f, Vector_DotProduct(light_direction, normal));
			triProjected.col = dp * 255;

			// Covert World Space -> View Space
			triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
			triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
			triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);

			// Project triangles 3D -> 2D
			triProjected.p[0] = Matrix_MultiplyVector(matProj, triViewed.p[0]);
			triProjected.p[1] = Matrix_MultiplyVector(matProj, triViewed.p[1]);
			triProjected.p[2] = Matrix_MultiplyVector(matProj, triViewed.p[2]);

			// Scale into view
			triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
			triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
			triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

			// Offset verts into visible normalides space
			vec3d vOffsetView = { 1, 1, 0 };
			triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
			triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
			triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);

			triProjected.p[0].x *= 0.5f * (float)w;
			triProjected.p[1].x *= 0.5f * (float)w;
			triProjected.p[2].x *= 0.5f * (float)w;

			triProjected.p[0].y *= 0.5f * (float)h;
			triProjected.p[1].y *= 0.5f * (float)h;
			triProjected.p[2].y *= 0.5f * (float)h;

			// Store triangle for sorting
			vecTrianglesToRaster.push_back(triProjected);
		}
	}

	// Sort triangles from back to front
	sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
		{
			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.f;
			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.f;

			return z1 > z2;
		});

	for (auto& triProjected : vecTrianglesToRaster)
	{
		//=========================================================
		// Draw triangles

		sf::VertexArray triangle(sf::Triangles, 3);

		triangle[0].position = sf::Vector2f(triProjected.p[0].x, triProjected.p[0].y);
		triangle[1].position = sf::Vector2f(triProjected.p[1].x, triProjected.p[1].y);
		triangle[2].position = sf::Vector2f(triProjected.p[2].x, triProjected.p[2].y);

		const unsigned short int c = triProjected.col;
		triangle[0].color = sf::Color(c, c, c);
		triangle[1].color = sf::Color(c, c, c);
		triangle[2].color = sf::Color(c, c, c);

		win.draw(triangle);

		if (drawLines)
		{
			//=========================================================
			// Draw triangle lines

			sf::Vertex triangle_line1[] =
			{
				sf::Vertex(sf::Vector2f(triProjected.p[0].x, triProjected.p[0].y)),
				sf::Vertex(sf::Vector2f(triProjected.p[1].x, triProjected.p[1].y))
			};

			sf::Vertex triangle_line2[] =
			{
				sf::Vertex(sf::Vector2f(triProjected.p[1].x, triProjected.p[1].y)),
				sf::Vertex(sf::Vector2f(triProjected.p[2].x, triProjected.p[2].y))
			};

			sf::Vertex triangle_line3[] =
			{
				sf::Vertex(sf::Vector2f(triProjected.p[2].x, triProjected.p[2].y)),
				sf::Vertex(sf::Vector2f(triProjected.p[0].x, triProjected.p[0].y))
			};

			triangle_line1[0].color = sf::Color::Black;
			triangle_line1[1].color = sf::Color::Black;

			triangle_line2[0].color = sf::Color::Black;
			triangle_line2[1].color = sf::Color::Black;

			triangle_line3[0].color = sf::Color::Black;
			triangle_line3[1].color = sf::Color::Black;

			win.draw(triangle_line1, 2, sf::Lines);
			win.draw(triangle_line2, 2, sf::Lines);
			win.draw(triangle_line3, 2, sf::Lines);

			//=========================================================
		}
	}
}
