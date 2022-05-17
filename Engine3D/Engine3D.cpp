#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <list>

using namespace std;

void clipDebug();
void showLines();
void onUpdate(sf::RenderWindow&);
void onCreate(const unsigned int, const unsigned int);
void moveCamera(int, float);

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "3D Engine", sf::Style::Close);
	sf::Clock clock;

	onCreate(window.getSize().x, window.getSize().y);

    while (window.isOpen())
    {
		sf::Time elapsed = clock.restart();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

			// show triangle lines
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::L) showLines();

			// clipping debug
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::C) clipDebug();
        }

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) moveCamera(0, elapsed.asSeconds());
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) moveCamera(1, elapsed.asSeconds());

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveCamera(2, elapsed.asSeconds());
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveCamera(3, elapsed.asSeconds());
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveCamera(4, elapsed.asSeconds());
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveCamera(5, elapsed.asSeconds());

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
	float col = 0;
	float r, g, b;

	void setColor(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

	void setColor(float dp)
	{
		this->r = dp;
		this->g = dp;
		this->b = dp;
		this->col = dp;
	}
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

float Yaw;

bool drawLines = false;
bool colorClipping = false;
float fTheta;

void showLines()
{
	drawLines = !drawLines;
}

void clipDebug()
{
	colorClipping = !colorClipping;
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
	matrix.m[3][3] = 1.f;

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

vec3d Vector_IntersectPlane(vec3d& plane_p, vec3d& plane_n, vec3d& lineStart, vec3d& lineEnd)
{
	plane_n = Vector_Normalize(plane_n);
	float plane_d = -Vector_DotProduct(plane_n, plane_p);
	float ad = Vector_DotProduct(lineStart, plane_n);
	float bd = Vector_DotProduct(lineEnd, plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	vec3d lineStartToEnd = Vector_Sub(lineEnd, lineStart);
	vec3d lineToIntersect = Vector_Mul(lineStartToEnd, t);

	return Vector_Add(lineStart, lineToIntersect);
}

int Triangle_ClipAgainstPlane(vec3d plane_p, vec3d plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2)
{
	// Make sure plane normal is indeed normal
	plane_n = Vector_Normalize(plane_n);

	// Return signed shortest distance from point to plane, plane normal must be normalized
	auto dist = [&](vec3d& p)
	{
		vec3d n = Vector_Normalize(p);
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector_DotProduct(plane_n, plane_p));
	};

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	vec3d* inside_points[3]; int nInsidePointCount = 0;
	vec3d* outside_points[3]; int nOutPointCount = 0;

	// Get signed distance of each point in triangle to plane
	float d0 = dist(in_tri.p[0]);
	float d1 = dist(in_tri.p[1]);
	float d2 = dist(in_tri.p[2]);

	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; }
	else { outside_points[nOutPointCount++] = &in_tri.p[0]; }

	if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[1]; }
	else { outside_points[nOutPointCount++] = &in_tri.p[1]; }

	if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[2]; }
	else { outside_points[nOutPointCount++] = &in_tri.p[2]; }

	// Now classify triangle points, and break the input triangle into
	// smaller output triangles if required. There are four possible outcomes...

	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		return 0; // No returned triangles are valid
	}

	if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simpy pass through
		out_tri1 = in_tri;

		return 1; // Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutPointCount == 2)
	{
		// Triangle shoub be clipped. As two points lie outside the plane,
		// the triangle simply becomes a smaller triangle

		// Copy appearance info to new triangle
		colorClipping ? out_tri1.setColor(in_tri.col, 0.f, 0.f) : out_tri1.setColor(in_tri.col);

		// The inside point is valid, so keep that...
		out_tri1.p[0] = *inside_points[0];

		// but the two new points are at the locations where
		// the original sides of the triangle (lines) intersect with the plane
		out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
		out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

		return 1; // Return the newly formed singel triangle
	}

	if (nInsidePointCount == 2 && nOutPointCount == 1)
	{
		// Triangle shoub be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles

		// copy appearance info to new triangles
		colorClipping ? out_tri1.setColor(0.f, in_tri.col, 0.f) : out_tri1.setColor(in_tri.col);
		colorClipping ? out_tri2.setColor(0.f, 0.f, in_tri.col) : out_tri2.setColor(in_tri.col);

		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1.p[0] = *inside_points[0];
		out_tri1.p[1] = *inside_points[1];
		out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

		// The secod triangle is composed of one of he inside points,
		// a new point determined by the intersection of the other side of the
		// triangle and the plane, and the newly created point above
		out_tri2.p[0] = *inside_points[1];
		out_tri2.p[1] = out_tri1.p[2];
		out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

		return 2; // Return two newly formed triangles which form a quad
	}
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

void moveCamera(int dir, float elapsedTime)
{
	float stepY = 8.f * elapsedTime;
	float stepX = 2.f * elapsedTime;
	vec3d vForward = Vector_Mul(vLookDir, 8.f * elapsedTime); 

	switch (dir)
	{
	case 0:
		vCamera.y -= stepY;
		break;

	case 1:
		vCamera.y += stepY;
		break;

	case 2:
		Yaw -= stepX;
		break;

	case 3:
		Yaw += stepX;
		break;

	case 4:
		vCamera = Vector_Add(vCamera, vForward);
		break;

	case 5:
		vCamera = Vector_Sub(vCamera, vForward);
		break;

	default:
		break;
	}
}

void onCreate(const unsigned int w, const unsigned int h)
{
	meshCube.LoadFromObjectFile("mountains.obj");

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
	matTrans = Matrix_MakeTranslation(0.f, 0.f, 5.f);

	mat4x4 matWorld;
	matWorld = Matrix_MakeIdentity();
	matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX);
	matWorld = Matrix_MultiplyMatrix(matWorld, matTrans);

	vec3d vUp = { 0.f, 1.f, 0.f };
	vec3d vTarget = { 0.f, 0.f, 1.f};
	mat4x4 matCameraRot = Matrix_MakeRotationY(Yaw);
	vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
	vTarget = Vector_Add(vCamera, vLookDir);

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
			triTransformed.setColor(dp * 255.f);

			// Covert World Space -> View Space
			triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
			triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
			triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);
			triViewed.setColor(triTransformed.col);

			// Clip Viewed Triangle against near plane, this could form two
			// additional triangles
			int nClippedTriangles = 0;
			triangle clipped[2];
			nClippedTriangles = Triangle_ClipAgainstPlane({ 0.f, 0.f, 0.1f }, { 0.f, 0.f, 1.f }, triViewed, clipped[0], clipped[1]);
			
			for (int n = 0; n < nClippedTriangles; n++)
			{

				// Project triangles 3D -> 2D
				triProjected.p[0] = Matrix_MultiplyVector(matProj, clipped[n].p[0]);
				triProjected.p[1] = Matrix_MultiplyVector(matProj, clipped[n].p[1]);
				triProjected.p[2] = Matrix_MultiplyVector(matProj, clipped[n].p[2]);
				triProjected.setColor(clipped[n].col);

				// Scale into view
				triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
				triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
				triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

				// X/Y are inverted so put them back
				triProjected.p[0].x *= -1.f;
				triProjected.p[1].x *= -1.f;
				triProjected.p[2].x *= -1.f;
				triProjected.p[0].y *= -1.f;
				triProjected.p[1].y *= -1.f;
				triProjected.p[2].y *= -1.f;

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
	}

	// Sort triangles from back to front
	sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
		{
			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.f;
			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.f;

			return z1 > z2;
		});

	for (auto& triToRaster : vecTrianglesToRaster)
	{
		// Clip triangles against all four screen edges, this could yeild
		// a bunch o triangles
		triangle clipped[2];
		list<triangle> listTriangles;
		listTriangles.push_back(triToRaster);
		int nNewTriangles = 1;

		for (size_t p = 0; p < 4; p++)
		{
			int nTrisToAdd = 0;
			while (nNewTriangles)
			{
				triangle test = listTriangles.front();
				listTriangles.pop_front();
				nNewTriangles--;

				switch (p)
				{
				case 0: nTrisToAdd = Triangle_ClipAgainstPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, test, clipped[0], clipped[1]); break;
				case 1: nTrisToAdd = Triangle_ClipAgainstPlane({ 0.f, (float)h - 1, 0.f}, { 0.f, -1.f, 0.f}, test, clipped[0], clipped[1]); break;
				case 2: nTrisToAdd = Triangle_ClipAgainstPlane({ 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, test, clipped[0], clipped[1]); break;
				case 3: nTrisToAdd = Triangle_ClipAgainstPlane({ (float)w - 1, 0.f, 0.f}, { -1.f, 0.f, 0.f}, test, clipped[0], clipped[1]); break;
				}

				for (int _w = 0; _w < nTrisToAdd; _w++) listTriangles.push_back(clipped[_w]);
			}

			nNewTriangles = listTriangles.size();
		}

		for (auto& t : listTriangles)
		{
			//=========================================================
			// Draw triangles

			sf::VertexArray triangle(sf::Triangles, 3);

			triangle[0].position = sf::Vector2f(t.p[0].x, t.p[0].y);
			triangle[1].position = sf::Vector2f(t.p[1].x, t.p[1].y);
			triangle[2].position = sf::Vector2f(t.p[2].x, t.p[2].y);

			for (size_t i = 0; i < 3; i++)
			{
				triangle[i].color = sf::Color(t.r, t.g, t.b);
			}

			win.draw(triangle);

			if (drawLines)
			{
				//=========================================================
				// Draw triangle lines

				sf::Vertex triangle_line1[] =
				{
					sf::Vertex(sf::Vector2f(t.p[0].x, t.p[0].y)),
					sf::Vertex(sf::Vector2f(t.p[1].x, t.p[1].y))
				};

				sf::Vertex triangle_line2[] =
				{
					sf::Vertex(sf::Vector2f(t.p[1].x, t.p[1].y)),
					sf::Vertex(sf::Vector2f(t.p[2].x, t.p[2].y))
				};

				sf::Vertex triangle_line3[] =
				{
					sf::Vertex(sf::Vector2f(t.p[2].x, t.p[2].y)),
					sf::Vertex(sf::Vector2f(t.p[0].x, t.p[0].y))
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
}
