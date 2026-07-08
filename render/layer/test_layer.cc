
#include "test_layer.h"

#include "renderer.h"

#include "shader/lambert_light_shader.h"
#include "model.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "perspective_camera.h"

void TestLayer::Init()
{
    InitShader();

    InitModel();
}

void TestLayer::Destroy()
{
    if(nullptr != lightShader_)
    {
        delete lightShader_;
        lightShader_ = nullptr;
    }

    if(nullptr != model)
    {
        delete model;
        model = nullptr;
    }
}

void TestLayer::Update()
{
    if(lightShader_ == nullptr || model == nullptr || !model->IsLoaded() || renderer_->GetCamera() == nullptr)
    {
        return;
    }

    lightShader_->view_matrix = renderer_->GetCamera()->GetViewMatrix();
	lightShader_->project_matrix = renderer_->GetCamera()->GetProjectionMatrix();
}

void TestLayer::Render()
{
    if(lightShader_ == nullptr || model == nullptr || !model->IsLoaded())
    {
        return;
    }

    renderer_->UseProgram(lightShader_);
    model->Draw(lightShader_);
}

void TestLayer::InitShader()
{
    if(lightShader_ != nullptr)
    {
        return;
    }

    lightShader_ = new LambertLightShader();
	lightShader_->directional_light_.color = { 1.0f, 1.0f, 1.0f };
	lightShader_->directional_light_.direction = { -1.0f, -0.5f, -0.7f };
	lightShader_->environment_light_.color = { 0.5f, 0.5f, 0.5f };
}

void TestLayer::InitModel()
{
    if(model != nullptr)
    {
        return;
    }

    const char* model_path = "assets/model/dinosaur/source/Rampaging T-Rex.glb";
    model = new Model(renderer_);
	if(!model->Read(model_path))
    {
        delete model;
        model = nullptr;
        return;
    }

    auto rotateMatrix = glm::rotate(glm::identity<glm::mat4>(), 0.0f , glm::vec3(0.0f, 1.0f, 0.0f));
	auto translateMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, -5.0f));
	auto modelMatrix = glm::scale(translateMatrix * rotateMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
	model->SetModelMatrix(modelMatrix);
}
