#include <vector>
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

using namespace std;

struct vec3d;
struct triangle;
struct mesh;
struct mat4x4;

void MultiplyMatrixVector(vec3d&, vec3d&, mat4x4&);
void onUpdate(sf::RenderWindow&);
void onCreate(const unsigned int, const unsigned int);

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
        }

        window.clear();

        onUpdate(window);

        window.display();
    }

    return 0;
}

struct vec3d
{
    float x, y, z;
};

struct triangle
{
    vec3d p[3];
};

struct mesh
{
    vector<triangle> tris;
};

struct mat4x4
{
    float m[4][4] = { 0 };
};

mesh meshCube;
mat4x4 matProj;

float fTheta;

void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
{
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f)
	{
		o.x /= w;
		o.y /= w;
		o.z /= w;
	}
}

void onCreate(const unsigned int w, const unsigned int h)
{
        meshCube.tris = {

            // SOUTH
            { 0.0f, 0.0f, 0.0f,      0.0f, 1.0f, 0.0f,       1.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 0.0f, 0.0f },

            // EAST
            { 1.0f, 0.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 1.0f },

            // WEST
            { 0.0f, 0.0f, 1.0f,      0.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f,      0.0f, 1.0f, 0.0f,       0.0f, 0.0f, 0.0f },

            // TOP
            { 0.0f, 1.0f, 0.0f,      0.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f,      1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 0.0f },

            // BOTTOM
            { 1.0f, 0.0f, 1.0f,      0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f },

        };

	// Projection Matrix
	float fNear = 0.1f;
	float fFar = 1000.0f;
	float fFov = 90.0f;
	float fAspectRatio = (float) h / (float) w;
	float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;
}

void onUpdate(sf::RenderWindow& win)
{
	const unsigned int w = win.getSize().x;
	const unsigned int h = win.getSize().y;

	mat4x4 matRotZ, matRotX;
	fTheta += 1.0f * 75.f / 1000.f / 100.f;

	// Rotation Z
	matRotZ.m[0][0] = cosf(fTheta);
	matRotZ.m[0][1] = sinf(fTheta);
	matRotZ.m[1][0] = -sinf(fTheta);
	matRotZ.m[1][1] = cosf(fTheta);
	matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;

	// Rotation X
	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cosf(fTheta * 0.5f);
	matRotX.m[1][2] = sinf(fTheta * 0.5f);
	matRotX.m[2][1] = -sinf(fTheta * 0.5f);
	matRotX.m[2][2] = cosf(fTheta * 0.5f);
	matRotX.m[3][3] = 1;

	// Draw Triangles
	for (auto tri : meshCube.tris)
	{
		triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

		// Rotate Z-Axis
		MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
		MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
		MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

		// Rotate X-Axis
		MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
		MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
		MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

		// Offset
		triTranslated = triRotatedZX;
		triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
		triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
		triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

		// Project triangles 3D -> 2D
		MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
		MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
		MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

		// Scale into view
		triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
		triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
		triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

		triProjected.p[0].x *= 0.5f * (float) w;
		triProjected.p[1].x *= 0.5f * (float) w;
		triProjected.p[2].x *= 0.5f * (float) w;

		triProjected.p[0].y *= 0.5f * (float) h;
		triProjected.p[1].y *= 0.5f * (float) h;
		triProjected.p[2].y *= 0.5f * (float) h;

		sf::Vertex line1[] =
		{
			sf::Vertex(sf::Vector2f(triProjected.p[0].x, triProjected.p[0].y)),
			sf::Vertex(sf::Vector2f(triProjected.p[1].x, triProjected.p[1].y))
		};

		sf::Vertex line2[] =
		{
			sf::Vertex(sf::Vector2f(triProjected.p[1].x, triProjected.p[1].y)),
			sf::Vertex(sf::Vector2f(triProjected.p[2].x, triProjected.p[2].y))
		};

		sf::Vertex line3[] =
		{
			sf::Vertex(sf::Vector2f(triProjected.p[2].x, triProjected.p[2].y)),
			sf::Vertex(sf::Vector2f(triProjected.p[0].x, triProjected.p[0].y))
		};

		win.draw(line1, 2, sf::Lines);
		win.draw(line2, 2, sf::Lines);
		win.draw(line3, 2, sf::Lines);
	}
}
