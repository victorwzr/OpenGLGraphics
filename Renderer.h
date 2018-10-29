#pragma once
#include "../nclgl/OGLRenderer.h"

#include "RenderObject.h"

#include <vector>

using std::vector;

struct Light
{
	Vector3 position;
	float radius;
	Vector3 colour;
};

class Renderer : public OGLRenderer	{
public:
	static const int nl = 5;
	static const int nt = 5;

	Renderer(Window &parent);
	~Renderer(void);

	virtual void	RenderScene();

	virtual void	Render(const RenderObject &o);

	virtual void	UpdateScene(float msec);

	void	AddRenderObject(RenderObject &r) {
		renderObjects.push_back(&r);
	}

	GLuint LoadTexture(const string& filename);

	void SetShaderLight(int i, const Vector3 &position, const Vector3 &colour, float radius);

	void nG();
	void nP();
	void nS();

	float getPosition() {
		return newposition;
	}


protected:
	void ApplyShaderLight(GLuint program);
	float time;
	float newposition;
	float newdelta;
	bool newb;
	Light light[nl];
	vector<RenderObject*> renderObjects;
};

//final

