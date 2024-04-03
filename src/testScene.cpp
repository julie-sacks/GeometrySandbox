#include "testScene.h"
#include "stdio.h"

void TestScene::Load()
{
    printf("loading test scene\n");
}

void TestScene::Init()
{
    printf("initializing test scene\n");
}

void TestScene::PreRender(float dt)
{
    printf("pre-render test scene\n");
    frameCount++;
}

void TestScene::Render(float dt)
{
    printf("rendering  test scene\n");
}

void TestScene::PostRender(float dt)
{
    printf("post-render test scene\n");
    if(frameCount >= 3) glfwSetWindowShouldClose(window, GL_TRUE);
}

void TestScene::Shutdown()
{
    printf("shutting down test scene\n");
}

void TestScene::Unload()
{
    printf("unloading test scene\n");
}
