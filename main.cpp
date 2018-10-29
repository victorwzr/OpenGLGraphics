#include "Renderer.h"
#include "RenderObject.h"

#pragma comment(lib, "nclgl.lib")
#define nSn 8
Shader *Nshader[nSn];

void lShader()            //load shader
{
	for (int i = 0; i < nSn; i++)
		Nshader[i] = NULL;

	Nshader[0] = new Shader("bv.glsl", "bf.glsl");   
	Nshader[1] = new Shader("sv.glsl", "bf.glsl");  //shrink
	Nshader[2] = new Shader("bv.glsl", "tf.glsl");   //texture
	Nshader[3] = new Shader("bv.glsl", "ff.glsl");    //transparency
	Nshader[4] = new Shader("nv.glsl", "bf.glsl", "sg.glsl");  //fall to pieces
	Nshader[5] = new Shader("lv.glsl", "lf.glsl");   //light laser
	Nshader[6] = new Shader("nv.glsl", "lf.glsl", "", "d.glsl", "dt.glsl");   //heightmap

	int f = 0;
	for (int i = 0; i < nSn; i++)
	{
		if (Nshader[i] != NULL && Nshader[i]->UsingDefaultShader())
		{
			f++;
			cout << "Shader " << i << " failed" << endl;
		}
	}

	if (f == 0)
		cout << "successfully" << endl;
}

void dShader()              //delete shader
{
	for (int i = 0; i < nSn; i++)
	{
		if (Nshader[i] != NULL)
			delete Nshader[i];
		Nshader[i] = NULL;
	}
}

void main(void) {
	Window w = Window(800, 600);
	Renderer r(w);

	Mesh* m	= Mesh::LoadMeshFile("cube.asciimesh");
	GLuint oriT = r.LoadTexture("1111.png");
	GLuint desT = r.LoadTexture("2222.png");          //load texture
	GLuint hT = r.LoadTexture("3333.png");
	RenderObject cube(m, Nshader[0], oriT);      //create cube
	cube.SetTexture(1, desT);       //change cube texture
	cube.SetTexture(2, hT);
	Mesh *laser = Mesh::GenerateLine(Vector3(-2.0, 0.0, 10.0), Vector3(0.0, 0.0, -10.0));      //laser mesh
	Mesh *mlaser = Mesh::GenerateLine(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 10.0));     //move laser mesh
	Shader *laserS = new Shader("bv.glsl", "nf.glsl");
	RenderObject slaser(laser, laserS);           //create laser
	RenderObject mmlaser(mlaser, laserS);            //create move laser
	cube.AddChild(mmlaser);
	lShader();        //load all shader
	cube.SetModelMatrix(Matrix4::Translation(Vector3(0.0, 0.0, -10.0)));        
	slaser.SetModelMatrix(Matrix4::Translation(Vector3(0.0, 1000.0, 0.0)));
	mmlaser.SetModelMatrix(Matrix4::Translation(Vector3(0.0, 1000.0, 0.0)));
	r.AddRenderObject(cube);
	r.AddRenderObject(slaser);           //render cube and laser
	r.SetProjectionMatrix(Matrix4::Perspective(1, 100, 1.33f, 45.0f));
	r.SetViewMatrix(Matrix4::BuildViewMatrix(Vector3(0, 0, 0), Vector3(0, 0, -10)));
	r.SetShaderLight(0, Vector3(5.0f, 10.0f, 10.0f), Vector3(1, 1, 1), 1000.0f);
	r.SetShaderLight(1, Vector3(0.0f, 1000.0f, 0.0f), Vector3(0, 0, 0), 0.0f);
	cube.SetShader(Nshader[0]);
	//keyboard function descrption
	cout << endl
		<< "basic: r for reset, s for stop, p for pause, o for restart, +-can zoom" << endl
		<< "function: a for shrink, b for change texture, c for transparency" << endl
		<< "advanced function: d for fall to pieces, e for laser lighting, f for move laser, g for heightmap" << endl;
	bool rb = true;
	bool ds = false;

	while(w.UpdateWindow()) {
		float msec = w.GetTimer()->GetTimedMS();
		if (Keyboard::KeyTriggered(KEY_O))
		{
			dShader();
			lShader();
			cube.SetShader(Nshader[0]);            //reset shader
		}

		if (Keyboard::KeyTriggered(KEY_S)) {       //stop move
			r.nP();
		}

		if (Keyboard::KeyDown(KEY_P)) {       //pause
			rb = !rb;
		}

		if (Keyboard::KeyTriggered(KEY_R))        //reset all value and object
		{
			r.SetShaderLight(1, Vector3(0.0f, 1000.0f, 0.0f), Vector3(0, 0, 0), 0.0f);
			slaser.SetModelMatrix(Matrix4::Translation(Vector3(0.0, 1000.0, 0.0)));
			mmlaser.SetModelMatrix(Matrix4::Translation(Vector3(0.0, 1000.0, 0.0)));
			m->type = GL_TRIANGLES;
			glEnable(GL_DEPTH_TEST);
			r.nS();
			cube.SetShader(Nshader[0]);
		}

		if (Keyboard::KeyTriggered(KEY_PLUS))      //+
		{
			cube.SetModelMatrix(Matrix4::Translation(Vector3(0.0f, 0.0f, 1.0f)) * cube.GetModelMatrix());
		}

		if (Keyboard::KeyTriggered(KEY_MINUS))          //-
		{
			cube.SetModelMatrix(Matrix4::Translation(Vector3(0.0f, 0.0f, -1.0f)) * cube.GetModelMatrix());
		}

		if (Keyboard::KeyTriggered(KEY_A))        //shrink
		{
			cube.SetShader(Nshader[1]);
			r.nG();
		}

		if (Keyboard::KeyTriggered(KEY_B))        //texture
		{
			cube.SetShader(Nshader[2]);
			r.nG();
		}

		if (Keyboard::KeyTriggered(KEY_C))          //transparency
		{
			ds = true;
			cube.SetShader(Nshader[3]);
			r.nG();
		}

		if (Keyboard::KeyTriggered(KEY_D))    //fall to pieces
		{
			cube.SetShader(Nshader[4]);
			r.nG();
		}

		if (Keyboard::KeyTriggered(KEY_E))         //laser
		{
			cube.SetShader(Nshader[5]);
			mmlaser.SetModelMatrix(Matrix4::Translation(Vector3(0.0, 0.0, 0.0)));
		}

		if (Keyboard::KeyTriggered(KEY_F))         //move laser
		{
			cube.SetShader(Nshader[5]);
			slaser.SetModelMatrix(Matrix4::Translation(Vector3(0.0, 0.0, 0.0)));
			r.SetShaderLight(1, Vector3(-2.0f, 0.0f, 10.0f), Vector3(1, 0, 0), 50.0f);
		}

		if (Keyboard::KeyHeld(KEY_G))
		{
			m->type = GL_PATCHES;
			cube.SetShader(Nshader[6]);             //heightmap
			
		}

		if (ds && glIsEnabled(GL_DEPTH_TEST) && r.getPosition() > 0.25)   //transparency disable depth
		{
			glDisable(GL_DEPTH_TEST);
			ds = false;
		}

		if (rb) {
			cube.SetModelMatrix(cube.GetModelMatrix() * Matrix4::Rotation(0.1f * msec, Vector3(0, 1, 1)));
		}                                         //rotate
		
		r.UpdateScene(msec);
		r.ClearBuffers();
		r.RenderScene();
		r.SwapBuffers();
	}

}