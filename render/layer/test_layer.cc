
#include "test_layer.h"

#include "renderer.h"

#include "shader/lambert_light_shader.h"
#include "model.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "perspective_camera.h"

void TestLayer::Init()
{
    lightShader_ = new LambertLightShader();
	lightShader_->directional_light_.color = { 1.0f, 1.0f, 1.0f };
	lightShader_->directional_light_.direction = { -1.0f, -0.5f, -0.7f };
	lightShader_->environment_light_.color = { 0.5f, 0.5f, 0.5f };

    model = new Model(renderer_);
	model->Read("assets/model/dinosaur/source/Rampaging T-Rex.glb");

    auto rotateMatrix = glm::rotate(glm::identity<glm::mat4>(), 0.0f , glm::vec3(0.0f, 1.0f, 0.0f));
	auto translateMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, -5.0f));
	auto m = translateMatrix * rotateMatrix;

	m = glm::scale(m, glm::vec3(0.1f, 0.1f, 0.1f));
	model->SetModelMatrix(m);
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

}

void TestLayer::Render()
{
    lightShader_->view_matrix = renderer_->GetCamera()->GetViewMatrix();
	lightShader_->project_matrix = renderer_->GetCamera()->GetProjectionMatrix();

    renderer_->UseProgram(lightShader_);
    model->Draw(lightShader_);
}