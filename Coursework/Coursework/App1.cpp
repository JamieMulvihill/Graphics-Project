// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Initalise scene variables.
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	waveShader = new WaveShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shader = new TessellationShader(renderer->getDevice(), hwnd);
	basicDepthShader = new BasicDepthShader(renderer->getDevice(), hwnd);
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	godRayShader = new GodRayShader(renderer->getDevice(), hwnd);

	sphereMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	skybox = new SkyBox(renderer->getDevice(), renderer->getDeviceContext());
	waterMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	planeMesh = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext(), 100);
	model = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/Tent.obj");
	treeSegment = new TreeSegmentMesh(renderer->getDevice(), renderer->getDeviceContext());
	fireLight = new TriangleMesh(renderer->getDevice(), renderer->getDeviceContext());

	cylinder = new CylinderMesh(renderer->getDevice(), renderer->getDeviceContext(), 10, 3, 20);

	camera->setPosition(45, 8, 95);
	camera->setRotation(0, 160, 0);

	int shadowmapWidth = 4096;
	int shadowmapHeight = 4096;
	int sceneWidth = 100;
	int sceneHeight = 100;
	space = false;


	ortho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0.f);
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0.f, 0.f);	// Full screen size
	orthoMesh2 = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 2, screenHeight, screenWidth / 4, 0.f);	// Full screen size

	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	spotLightShadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	renderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	blackRenderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	textureMgr->loadTexture(L"SkyBox", L"res/spaceSky.png");
	textureMgr->loadTexture(L"Bunny", L"res/bunny.png");
	textureMgr->loadTexture(L"Bark", L"res/bark.jpg");
	textureMgr->loadTexture(L"foam", L"res/foam2.png");
	textureMgr->loadTexture(L"texture_map", L"res/texture.jpg");
	textureMgr->loadTexture(L"texture_map_height", L"res/textureHeight2.png");
	textureMgr->loadTexture(L"tenttex", L"res/cloth.jpg");
	textureMgr->loadTexture(L"treeTex", L"res/bush_tex.png");
	textureMgr->loadTexture(L"treeTexHeight", L"res/bush_height_2.png");
	textureMgr->loadTexture(L"black", L"res/black.png");
	textureMgr->loadTexture(L"white", L"res/Untitled.png");
	textureMgr->loadTexture(L"Moon", L"res/Moon.png");
	textureMgr->loadTexture(L"Fire", L"res/fireTex.png");

	lightX = 0.f;
	lightY = -.486f;
	lightZ = -1.f;

	lightPosX = 50.f;
	lightPosY = 37.5f;
	lightPosZ = 95.77f;

	pointPosX = 50.f;
	pointPosY = 37.5f;
	pointPosZ = 95.77f;

	light = new Light;
	light->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(lightX, lightY, lightZ);
	light->setPosition(50.f, 39.f, -17.f);
	light->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	pointLight = new Light;
	pointLight->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	pointLight->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	pointLight->setPosition(50.f, 39.f, -17.f);
	pointLight->setSpecularColour(.6f, .6f, .6f, 1.0f);
	pointLight->setSpecularPower(128);

	pLight = new Light;
	pLight->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	pLight->setDiffuseColour(.886f, .345f, .133f, 1.0f);
	pLight->setPosition(63.5f, 4.5f, 65.5f);
	pLight->setSpecularColour(.6f, .6f, .6f, 1.0f);
	pLight->setSpecularPower(128);

	nearPlane = 1.5f;
	farPlane = 50.f;

	spotLight = new Light;
	spotLight->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	spotLight->setDiffuseColour(1.f, 1.f, .4f, 1.f);
	spotLight->setPosition(0, 0, 0);
	spotLight->setDirection(0.f, 0.f, 1.f);
	spotLight->generateProjectionMatrix(nearPlane, farPlane);

	isPointLight = true;
	isDiretLight = false;
	isSpotLight = false;

	scaleX = 1;
	scaleY = 1;
	scaleZ = 1;

	waterX = 23.1f;
	waterY = 4.5f;
	waterZ = 70.7f;

	waterScaleX = .330f;
	waterscaleZ = .286f;

	time = 0.f;
	amplitude = .05f;
	frequency = 36.5f;
	speed = 1.f;

	wDX = .488;
	wDZ = .306;

	constant_factor = .5f;
	linear_factor = .125f;
	quadratic_factor = 0.1f;

	shadowBias = 0.019;
	displacement = 2.4;
	non_displacement = 1.f;

	rotation = 0;
	lightSetting = 4;
	tick_ = 0.f;
	maxFireQuad = false;
	maxFireLine = false;
	maxFireConst = false;

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(50.f, 0.f, 0.f);
	rotationMatrix = XMMatrixRotationRollPitchYaw(rotation, 0.f, 0.f);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);

	guiOpen = true;

	post_process = true;
	decay_ = 0.96815;
	exposure_ = 0.2;
	density_ = 0.926;
	weight_ = 0.58767;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.	
}

bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	time += timer->getTime();

	// values for controlling the light behaviour
	FireBehaviourCalculation();
	pointLight->setPosition(pointPosX, pointPosY, pointPosZ);
	light->setPosition(lightPosX, lightPosY, lightPosZ);
	light->setDirection(lightX, lightY, lightZ);
	spotLight->setDiffuseColour(spotlightColour[0], spotlightColour[1], spotlightColour[2], spotlightColour[3]);
	SetSpotLighttoCameraPostion();
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}
	
	return true;
}

void App1::SetSpotLighttoCameraPostion()
{
	XMVECTOR up, positionv, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// Setup the vectors
	up = XMVectorSet(0.0f, 1.0, 0.0, 1.0f);

	positionv = XMLoadFloat3(&camera->getPosition());
	lookAt = XMVectorSet(0.0, 0.0, 1.0f, 1.0f);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = XMConvertToRadians(camera->getRotation().x);
	yaw = XMConvertToRadians(camera->getRotation().y);
	roll = XMConvertToRadians(camera->getRotation().z);

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	XMFLOAT3 tempVector;
	tempVector.x = XMVectorGetX(lookAt);
	tempVector.y = XMVectorGetY(lookAt);
	tempVector.z = XMVectorGetZ(lookAt);
	
	spotLight->setPosition(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z + (tempVector.z * -2.f));
	spotLight->setDirection(tempVector.x, tempVector.y, tempVector.z);
	spotLight->generateProjectionMatrix(nearPlane, farPlane);
}

void App1::DepthPass(){

	// Set the render target to be the render to texture.
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	light->generateViewMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	planeMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"texture_map_height"), XMFLOAT4(1, 1, 1, 1));
	depthShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	//urgh
	for (int i = 0; i < 4; i++) {
		cylinder->sendData(renderer->getDeviceContext());
		basicDepthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.3, 1, .3)), XMMatrixTranslation(treex[i], treey[i], treez[i])), lightViewMatrix, lightProjectionMatrix, non_displacement);
		basicDepthShader->render(renderer->getDeviceContext(), cylinder->getIndexCount());
	}

	for (int i = 0; i < 4; i++) {
		worldMatrix = renderer->getWorldMatrix();
		//bottom segment
		treeSegment->sendData(renderer->getDeviceContext());
		basicDepthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.5, .5, .5)), XMMatrixTranslation(treex[i], treey[i] + .25, treez[i])), lightViewMatrix, lightProjectionMatrix, displacement);
		basicDepthShader->render(renderer->getDeviceContext(), treeSegment->getIndexCount());
		//middle segment
		worldMatrix = renderer->getWorldMatrix();
		treeSegment->sendData(renderer->getDeviceContext());
		basicDepthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.3, .3, .3)), XMMatrixTranslation(treex[i], treey[i] + .75, treez[i])), lightViewMatrix, lightProjectionMatrix, displacement);
		basicDepthShader->render(renderer->getDeviceContext(), treeSegment->getIndexCount());
		//top segment
		worldMatrix = renderer->getWorldMatrix();
		treeSegment->sendData(renderer->getDeviceContext());
		basicDepthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.2, .2, .2)), XMMatrixTranslation(treex[i], treey[i] + 1.10, treez[i])), lightViewMatrix, lightProjectionMatrix, displacement);
		basicDepthShader->render(renderer->getDeviceContext(), treeSegment->getIndexCount());
	}

	// Render model
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX translateMatrix = XMMatrixTranslation(57.4f, 5.f, 59.f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);
	model->sendData(renderer->getDeviceContext());
	basicDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, non_displacement);
	basicDepthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::SpotLightDepthPass()
{

	// Set the render target to be the render to texture.
	spotLightShadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	spotLight->generateViewMatrix();
	XMMATRIX lightViewMatrix = spotLight->getViewMatrix();
	XMMATRIX lightProjectionMatrix = spotLight->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	planeMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"texture_map_height"), XMFLOAT4(1, 1, 1, 1));
	depthShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());


	//urgh
	for (int i = 0; i < 4; i++) {
		cylinder->sendData(renderer->getDeviceContext());
		basicDepthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.3, 1, .3)), XMMatrixTranslation(treex[i], treey[i], treez[i])), lightViewMatrix, lightProjectionMatrix, non_displacement);
		basicDepthShader->render(renderer->getDeviceContext(), cylinder->getIndexCount());
	}

	for (int i = 0; i < 4; i++) {
		worldMatrix = renderer->getWorldMatrix();
		//bottom segment
		treeSegment->sendData(renderer->getDeviceContext());
		basicDepthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.5, .5, .5)), XMMatrixTranslation(treex[i], treey[i] + .25, treez[i])), lightViewMatrix, lightProjectionMatrix, displacement);
		basicDepthShader->render(renderer->getDeviceContext(), treeSegment->getIndexCount());
		//middle segment
		worldMatrix = renderer->getWorldMatrix();
		treeSegment->sendData(renderer->getDeviceContext());
		basicDepthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.3, .3, .3)), XMMatrixTranslation(treex[i], treey[i] + .75, treez[i])), lightViewMatrix, lightProjectionMatrix, displacement);
		basicDepthShader->render(renderer->getDeviceContext(), treeSegment->getIndexCount());
		//top segment
		worldMatrix = renderer->getWorldMatrix();
		treeSegment->sendData(renderer->getDeviceContext());
		basicDepthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.2, .2, .2)), XMMatrixTranslation(treex[i], treey[i] + 1.10, treez[i])), lightViewMatrix, lightProjectionMatrix, displacement);
		basicDepthShader->render(renderer->getDeviceContext(), treeSegment->getIndexCount());
	}

	// Render model
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX translateMatrix = XMMatrixTranslation(57.4f, 5.f, 59.f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);
	model->sendData(renderer->getDeviceContext());
	basicDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, non_displacement);
	basicDepthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();


}

void App1::FirstPass()
{
	// Set the render target to be the render to texture and clear it
	renderTexture->setRenderTarget(renderer->getDeviceContext());
	renderTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX ScaleMatrix = XMMatrixScaling(10, 10, 10);

	// Render shape with simple lighting shader set.
	//sky box needs a pixel shader without lighting calculaions
	renderer->setZBuffer(false);
	skybox->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, XMMatrixTranslation(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"SkyBox"), NULL, light, non_displacement);
	textureShader->render(renderer->getDeviceContext(), skybox->getIndexCount());
	renderer->setZBuffer(true);

	// Send geometry data, set shader parameters, render object with shader
	planeMesh->sendData(renderer->getDeviceContext());
	shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 1), time, textureMgr->getTexture(L"texture_map"), textureMgr->getTexture(L"texture_map_height"), light, pLight, spotLight, lightX, lightY, lightZ, shadowMap->getDepthMapSRV(), spotLightShadowMap->getDepthMapSRV(), shadowBias, lightSetting, constant_factor, linear_factor, quadratic_factor);
	shader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// Send geometry data, set shader parameters, render object with shader
	waterMesh->sendData(renderer->getDeviceContext());
	waveShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(waterScaleX, 1, waterscaleZ)), XMMatrixTranslation(waterX, waterY, waterZ)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"foam"), pointLight, time, amplitude, frequency, speed, wDX, wDZ, camera);
	waveShader->render(renderer->getDeviceContext(), waterMesh->getIndexCount());

	sphereMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, ScaleMatrix), XMMatrixTranslation(pointLight->getPosition().x, pointLight->getPosition().y, pointLight->getPosition().z)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"Moon"), textureMgr->getTexture(L"Moon"), pointLight, 0);
	textureShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	fireLight->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, XMMatrixTranslation(pLight->getPosition().x, pLight->getPosition().y, pLight->getPosition().z)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"Fire"), textureMgr->getTexture(L"Moon"), pLight, non_displacement);
	textureShader->render(renderer->getDeviceContext(), fireLight->getIndexCount());

	//urgh
	for (int i = 0; i < 4; i++) {
		cylinder->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.3, 1, .3)), XMMatrixTranslation(treex[i], treey[i], treez[i])), viewMatrix, projectionMatrix, textureMgr->getTexture(L"Bark"), textureMgr->getTexture(L"black"), light, pLight, spotLight, camera, displacement, XMFLOAT3(0, -1, 0), lightSetting, constant_factor, linear_factor, quadratic_factor);
		lightShader->render(renderer->getDeviceContext(), cylinder->getIndexCount());
	}

	for (int i = 0; i < 4; i++) {
		worldMatrix = renderer->getWorldMatrix();
		//bottom segment
		treeSegment->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.5, .5, .5)), XMMatrixTranslation(treex[i], treey[i] + .25, treez[i])), viewMatrix, projectionMatrix, textureMgr->getTexture(L"treeTex"), textureMgr->getTexture(L"treeTexHeight"), light, pLight, spotLight, camera, displacement,  XMFLOAT3(0, -1, 0), lightSetting, constant_factor, linear_factor, quadratic_factor);
		lightShader->render(renderer->getDeviceContext(), treeSegment->getIndexCount());
		//middle segment
		worldMatrix = renderer->getWorldMatrix();
		treeSegment->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.3, .3, .3)), XMMatrixTranslation(treex[i], treey[i] + .75, treez[i])), viewMatrix, projectionMatrix, textureMgr->getTexture(L"treeTex"), textureMgr->getTexture(L"treeTexHeight"), light, pLight, spotLight, camera, displacement, XMFLOAT3(0, -1, 0), lightSetting, constant_factor, linear_factor, quadratic_factor);
		lightShader->render(renderer->getDeviceContext(), treeSegment->getIndexCount());
		//top segment
		worldMatrix = renderer->getWorldMatrix();
		treeSegment->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.2, .2, .2)), XMMatrixTranslation(treex[i], treey[i] + 1.10, treez[i])), viewMatrix, projectionMatrix, textureMgr->getTexture(L"treeTex"), textureMgr->getTexture(L"treeTexHeight"), light, pLight, spotLight, camera, displacement, XMFLOAT3(0, -1, 0), lightSetting, constant_factor, linear_factor, quadratic_factor);
		lightShader->render(renderer->getDeviceContext(), treeSegment->getIndexCount());
	}

	// Render model
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX translateMatrix = XMMatrixTranslation(57.4f, 5.f, 59.f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);
	model->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"tenttex"), textureMgr->getTexture(L"black"), light, pLight, spotLight, camera, displacement, XMFLOAT3(0, -1, 0), lightSetting, constant_factor, linear_factor, quadratic_factor);
	lightShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::BlackPass()
{
	// Set the render target to be the render to texture and clear it
	blackRenderTexture->setRenderTarget(renderer->getDeviceContext());
	blackRenderTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.f, 1.0f);

	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX ScaleMatrix = XMMatrixScaling(10, 10, 10);

	// Send geometry data, set shader parameters, render object with shader
	planeMesh->sendData(renderer->getDeviceContext());
	shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 1), time, NULL, textureMgr->getTexture(L"texture_map_height"), light, pLight, spotLight, lightX, lightY, lightZ, shadowMap->getDepthMapSRV(), spotLightShadowMap->getDepthMapSRV(), shadowBias, lightSetting, constant_factor, linear_factor, quadratic_factor);
	shader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// Send geometry data, set shader parameters, render object with shader
	waterMesh->sendData(renderer->getDeviceContext());
	waveShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(waterScaleX, 1, waterscaleZ)), XMMatrixTranslation(waterX, 5, waterZ)), viewMatrix, projectionMatrix, NULL, pointLight, time, amplitude, frequency, speed, wDX, wDZ, camera);
	waveShader->render(renderer->getDeviceContext(), waterMesh->getIndexCount());

	//urgh
	for (int i = 0; i < 4; i++) {
		cylinder->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.3, 1, .3)), XMMatrixTranslation(treex[i], treey[i], treez[i])), viewMatrix, projectionMatrix, NULL, textureMgr->getTexture(L"black"), light, pLight, spotLight, camera, displacement, XMFLOAT3(0, -1, 0), lightSetting, constant_factor, linear_factor, quadratic_factor);
		lightShader->render(renderer->getDeviceContext(), cylinder->getIndexCount());
	}

	for (int i = 0; i < 4; i++) {
		worldMatrix = renderer->getWorldMatrix();
		//bottom segment
		treeSegment->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.5, .5, .5)), XMMatrixTranslation(treex[i], treey[i] + .25, treez[i])), viewMatrix, projectionMatrix, NULL, textureMgr->getTexture(L"treeTexHeight"), light, pLight, spotLight, camera, displacement, XMFLOAT3(0, -1, 0), lightSetting, constant_factor, linear_factor, quadratic_factor);
		//middle segment
		worldMatrix = renderer->getWorldMatrix();
		treeSegment->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.3, .3, .3)), XMMatrixTranslation(treex[i], treey[i] + .75, treez[i])), viewMatrix, projectionMatrix, NULL, textureMgr->getTexture(L"treeTexHeight"), light, pLight, spotLight, camera, displacement, XMFLOAT3(0, -1, 0), lightSetting, constant_factor, linear_factor, quadratic_factor);
		lightShader->render(renderer->getDeviceContext(), treeSegment->getIndexCount());
		//top segment
		worldMatrix = renderer->getWorldMatrix();
		treeSegment->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, XMMatrixScaling(.2, .2, .2)), XMMatrixTranslation(treex[i], treey[i] + 1.10, treez[i])), viewMatrix, projectionMatrix, NULL, textureMgr->getTexture(L"treeTexHeight"), light, pLight, spotLight, camera, displacement, XMFLOAT3(0, -1, 0), lightSetting, constant_factor, linear_factor, quadratic_factor);
		lightShader->render(renderer->getDeviceContext(), treeSegment->getIndexCount());
	}

	// Render model
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX translateMatrix = XMMatrixTranslation(57.4f, 5.f, 59.f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);
	model->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, NULL, textureMgr->getTexture(L"black"), light, pLight, spotLight, camera, displacement, XMFLOAT3(0, -1, 0), lightSetting, constant_factor, linear_factor, quadratic_factor);
	lightShader->render(renderer->getDeviceContext(), model->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	
	sphereMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(XMMatrixMultiply(worldMatrix, ScaleMatrix), XMMatrixTranslation(pointLight->getPosition().x, pointLight->getPosition().y, pointLight->getPosition().z)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"Moon"), textureMgr->getTexture(L"Moon"), pointLight, 0);
	textureShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());


	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	if (post_process) {
		orthoMesh->sendData(renderer->getDeviceContext());
		godRayShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, blackRenderTexture->getShaderResourceView(), renderTexture->getShaderResourceView(), pointLight, XMFLOAT2(1024, 1024), viewMatrix, projectionMatrix, decay_, density_, exposure_, weight_);
		godRayShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	}

	else if (!post_process){
		orthoMesh->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, renderTexture->getShaderResourceView(), NULL, light, 0);
		textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	}

	renderer->setZBuffer(true);

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}

void App1::FireBehaviourCalculation()
{
	if (quadratic_factor < .2 && !maxFireQuad) {
		quadratic_factor += 0.0016;
	}
	if (quadratic_factor >= .2) {
		quadratic_factor -= 0.0016;
		maxFireQuad = true;
	}
	if (quadratic_factor < .2 && maxFireQuad) {
		quadratic_factor -= 0.0016;
	}
	if (quadratic_factor <= 0.01 && maxFireQuad) {
		quadratic_factor += 0.0016;
		maxFireQuad = false;
	}
	
	if (constant_factor < .2 && !maxFireConst) {
		constant_factor += 0.0016;
	}
	if (constant_factor >= .2) {
		constant_factor -= 0.0016;
		maxFireConst = true;
	}
	if (constant_factor < .2 && maxFireConst) {
		constant_factor -= 0.0016;
	}
	if (constant_factor <= 0.01 && maxFireConst) {
		constant_factor += 0.0016;
		maxFireConst = false;
	}

	if (linear_factor < .2 && !maxFireLine) {
		linear_factor += 0.0016;
	}
	if (linear_factor >= .2) {
		linear_factor -= 0.0016;
		maxFireLine = true;
	}
	if (linear_factor < .2 && maxFireLine) {
		linear_factor -= 0.0016;
	}
	if (linear_factor <= 0.01 && maxFireLine) {
		linear_factor += 0.0016;
		maxFireLine = false;
	}


}

bool App1::render(){
	
	DepthPass();
	SpotLightDepthPass();
	FirstPass();
	BlackPass();
	renderer->setWireframeMode(false);
	finalPass();
	renderer->setWireframeMode(wireframeToggle);

	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	if (ImGui::CollapsingHeader("LightControls")){
		ImGui::SliderInt("LightSetting", &lightSetting, 1, 4);
		if (ImGui::CollapsingHeader("MoonLightControls")){
			ImGui::SliderFloat("MoonPosX", &pointPosX, -50, 150);
			ImGui::SliderFloat("MoonPosY", &pointPosY, -50, 150);
			ImGui::SliderFloat("MoonPosZ", &pointPosZ, -50, 150);
		}
		if (ImGui::CollapsingHeader("Attenuation")){
			ImGui::SliderFloat("ConstantFactor", &constant_factor, 0, 1);
			ImGui::SliderFloat("LinearFactor", &linear_factor, 0, 1);
			ImGui::SliderFloat("QuadraticFactor", &quadratic_factor, 0, 1);
		}
		if (ImGui::CollapsingHeader("SpotLightControls")){
			ImGui::ColorEdit4("Spotlight", spotlightColour);
			ImGui::SliderFloat("NearPlane", &nearPlane, .01f, 15.f);
			ImGui::SliderFloat("FarPlane", &farPlane, 10.f, 350.f);
			ImGui::SliderFloat("AllSadowBias", &shadowBias, 0, .2);
		}
		if (ImGui::CollapsingHeader("DirectionalLightControls")){
			ImGui::SliderFloat("DirectionalLightDirectX", &lightX, -1, 1);
			ImGui::SliderFloat("DirectionalLightDirectY", &lightY, -1, 1);
			ImGui::SliderFloat("DirectionalLightDirectZ", &lightZ, -1, 1);
			ImGui::SliderFloat("DirectionalLightPosX", &lightPosX, -50, 150);
			ImGui::SliderFloat("DirectionalLightPosY", &lightPosY, -50, 150);
			ImGui::SliderFloat("DirectionalLightPosZ", &lightPosZ, -50, 150);
		}
	}
	if (ImGui::CollapsingHeader("Post-Processing")) {
		ImGui::Checkbox("God-Rays", &post_process);
		ImGui::SliderFloat("Decay", &decay_, 0, 1);
		ImGui::SliderFloat("Density", &density_, 0, 1);
		ImGui::SliderFloat("Exposure", &exposure_, 0, 1);
		ImGui::SliderFloat("Weight", &weight_, 0, 1);
	}

	if (ImGui::CollapsingHeader("Displacement")) {
		ImGui::SliderFloat("Bushiness", &displacement, 1, 5);
		ImGui::SliderFloat("Ampliude", &amplitude, 0, .75f);
	}


	

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

