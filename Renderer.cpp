#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent),
    time(0.0f),
	newposition(0.0f),           //initialize
	newdelta(0.00025),
    newb(false){
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}

Renderer::~Renderer(void)	{
	renderObjects.clear();
}

void	Renderer::RenderScene() {
	for(vector<RenderObject*>::iterator i = renderObjects.begin(); i != renderObjects.end(); ++i ) {
		Render(*(*i));
	}
}

void	Renderer::Render(const RenderObject &o) {
	modelMatrix = o.GetWorldTransform();

	if(o.GetShader() && o.GetMesh()) {
		GLuint program = o.GetShader()->GetShaderProgram();
		
		glUseProgram(program);

		UpdateShaderMatrices(program);
		ApplyShaderLight(program);
		
		Matrix3 r = Matrix3(viewMatrix);
		Vector3 icP = viewMatrix.GetPositionVector();
		Vector3 cP = r * -icP;
		glUniform3fv(glGetUniformLocation(program, "cameraPos"), 1, (float *)&cP);
		glUniform1f(glGetUniformLocation(program, "animPosition"), newposition);
		glUniform1i(glGetUniformLocation(program, "objectTextures[0]"), 0);
		glUniform1i(glGetUniformLocation(program, "objectTextures[1]"), 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, o.GetTexture(0));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, o.GetTexture(1));

		o.Draw();
	}

	for(vector<RenderObject*>::const_iterator i = o.GetChildren().begin(); i != o.GetChildren().end(); ++i ) {
		Render(*(*i));
	}
}

void	Renderer::UpdateScene(float msec) {
	if (newb) {
		if (newposition < 1.0) {                      //Calculate new position
			float d = msec * newdelta;
			newposition += d;
		}
		else {
			nS();
		}
	}
	time += msec;
	for(vector<RenderObject*>::iterator i = renderObjects.begin(); i != renderObjects.end(); ++i ) {
		(*i)->Update(msec);
	}
}

GLuint Renderer::LoadTexture(const string& filename) {
	GLuint texName = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (texName == 0) {
		cout << "Texture file " << filename << " failed to load!" << std::endl;
	}

	return texName;
}
void Renderer::SetShaderLight(int i, const Vector3 &position, const Vector3 &colour, float radius)
{
	light[i].position = position;
	light[i].radius = radius;
	light[i].colour = colour;
}

void Renderer::ApplyShaderLight(GLuint program)
{
	glUniform3fv(glGetUniformLocation(program, "lightPos[0]"), 1, (float *)&(light[0].position));
	glUniform1f(glGetUniformLocation(program, "lightRadius[0]"), light[0].radius);
	glUniform3fv(glGetUniformLocation(program, "lightColour[0]"), 1, (float *)&(light[0].colour));

	glUniform3fv(glGetUniformLocation(program, "lightPos[1]"), 1, (float *)&(light[1].position));
	glUniform1f(glGetUniformLocation(program, "lightRadius[1]"), light[1].radius);
	glUniform3fv(glGetUniformLocation(program, "lightColour[1]"), 1, (float *)&(light[1].colour));
}

void Renderer::nS() {   //start
	newb = false;
}

void Renderer::nP() {
	if (newposition < newdelta || newposition > 1.0 - newdelta)
		return;                    //pause
	newb = !newb;
}

void Renderer::nG(){
	newposition = 0.0f;         //reset
	newb = true;
}