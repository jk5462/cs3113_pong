#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    speed = 0;
	width = 0;
	height = 0;
}

void Entity::Update(float deltaTime)
{
    position += movement * speed * deltaTime;
}

void Entity::Render(ShaderProgram *program, float vertices[], float color[]) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    program->SetModelMatrix(modelMatrix);
	program->SetColor(color[0], color[1], color[2], color[3]);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6); 
    glDisableVertexAttribArray(program->positionAttribute);
}

