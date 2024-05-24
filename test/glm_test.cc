
#include <iostream>

// #include "glm/vec3.hpp" // glm::vec3
// #include "glm/vec4.hpp" // glm::vec4
// #include "glm/mat4x4.hpp" // glm::mat4
// #include "glm/ext/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale
// #include "glm/ext/matrix_clip_space.hpp" // glm::perspective
// #include "glm/ext/scalar_constants.hpp" // glm::pi

#include "glm/glm.hpp" //vec2 vec3 vec4 radians
#include "glm/ext.hpp" //perspective translate rotate



glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}

bool TestVectorDotMultiply()
{
    glm::vec2 v1(1,2);
    glm::vec2 v2(2,3);

    return glm::dot(v1, v2) == 8.0f;
}

bool TestVectorAdd()
{
    glm::vec2 v1(1,2);
    glm::vec2 v2(2,3);

    glm::vec2 expect(3,5);

    return expect == v1 + v2;
}

bool TestVectorClamp()
{
    glm::vec3 v(-1.3f, 0.5f, 2.5f);
    glm::vec3 expect(-1.0f, 0.5f, 1.0f);

    return clamp(v, -1.0f, 1.0f) == expect;
}

bool TestVectorCrossMultiply()
{
    glm::vec3 v1(1.1f, 0.5f, 2.5f);
    glm::vec3 v2(1.0f, 0.8f, 1.0f);

    glm::vec3 expect(-1.5f, 1.4f, 0.3800f);

    glm::vec3 cross = glm::cross(v1, v2);

    glm::bvec3 equal = glm::equal(cross, expect, 0.00001f);

    return equal.x == true && equal.y == true && equal.z == true;
}

int Pass()
{
    std::cout << "thirdparts library glm_test Pass" << std::endl;
    return 0;
}

int FailPass()
{
     std::cout << "thirdparts library glm_test Can't Pass" << std::endl;
    return -1;
}

int main(int argc, char** argv)
{
    if(!TestVectorDotMultiply())
    {
       return FailPass();
    }
    
    if(!TestVectorAdd())
    {
       return FailPass();
    }

    if(!TestVectorClamp())
    {
        return FailPass();
    }

    if(!TestVectorCrossMultiply())
    {
        return FailPass();
    }

    return Pass();
}