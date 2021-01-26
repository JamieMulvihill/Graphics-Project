// Application.h
#pragma once

// Includes
#include "DXF.h"
#include "TextureShader.h"
#include "SkyBox.h"
#include "WaveShader.h"
#include "TessellationShader.h"
#include "TessellationPlane.h"
#include "DepthShader.h"
#include "BasicDepthShader.h"
#include "ShadowShader.h"
#include "CylinderMesh.h"
#include "TreeSegmentMesh.h"
#include "LightShader.h"
#include "GodRayShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

	void SetSpotLighttoCameraPostion();

protected:
	bool render();
	void FirstPass();
	void BlackPass();
	void DepthPass();
	void SpotLightDepthPass();
	void finalPass();
	void FireBehaviourCalculation();
	void gui();

private:

	TextureShader* textureShader;
	WaveShader* waveShader;
	TessellationShader* shader;
	DepthShader* depthShader;
	BasicDepthShader* basicDepthShader;
	ShadowShader* shadowShader;
	LightShader* lightShader;
	GodRayShader* godRayShader;

	OrthoMesh* ortho;
	OrthoMesh* orthoMesh;
	OrthoMesh* orthoMesh2;

	ShadowMap* shadowMap;
	ShadowMap* spotLightShadowMap;

	RenderTexture* renderTexture;
	RenderTexture* blackRenderTexture;

	SphereMesh* sphereMesh;
	TriangleMesh* fireLight;
	CylinderMesh* cylinder;
	TreeSegmentMesh* treeSegment;
	Model* model;
	PlaneMesh* waterMesh;
	TessellationPlane* planeMesh;
	SkyBox* skybox;

	Light* light;
	Light* pointLight;
	Light* pLight;
	Light* spotLight;

	XMMATRIX rotationMatrix;
	float rotation;

	float time, amplitude, frequency, speed, wDX, wDZ;
	float lightX, lightY, lightZ;
	float lightPosX, lightPosY, lightPosZ;
	float pointPosX, pointPosY, pointPosZ;
	float scaleX, scaleY, scaleZ;
	float waterX, waterY, waterZ, waterScaleX, waterscaleZ;
	float constant_factor, linear_factor, quadratic_factor;
	float shadowBias, displacement, non_displacement;
	float decay_, density_, exposure_, weight_;
	int lightSetting;

	float treex[4] = { 71.f, 68.f, 51.f, 58.f };
	float treey[4] = { 7.f, 6.5f, 9.7f, 8.f };
	float treez[4] = { 67.f, 56.f, 49.f, 72.5f };
	float spotlightColour[4] = { .7f, .7, .1f, 1.f };
	float nearPlane, farPlane;
	
	float tick_;
	bool post_process;
	bool maxFireConst, maxFireLine, maxFireQuad;
	bool space, orbit, inShadow, guiOpen;
	bool isDiretLight, isPointLight, isSpotLight;
};
