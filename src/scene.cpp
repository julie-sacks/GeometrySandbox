#include "scene.h"


void Scene::Load()
{
}

void Scene::Init()
{
}

void Scene::PreRender(float dt)
{
}

void Scene::Render(float dt)
{
}

void Scene::PostRender(float dt)
{
}

void Scene::Shutdown()
{
}

void Scene::Unload()
{
}

void Scene::SetWindowSize(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
}

void Scene::RunFrame(float dt)
{
    PreRender(dt);
    Render(dt);
    PostRender(dt);
}

void Scene::Reload()
{
    Shutdown();
    Unload();
    Load();
    Init();
}

void Scene::Restart()
{
    Shutdown();
    Init();
}