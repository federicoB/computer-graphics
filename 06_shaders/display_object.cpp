//
// Created by fede on 29/09/20.
//

//Shaders Uniforms
static vector<LightShaderUniform> light_uniforms; // for shaders with light
static vector<BaseShaderUniform> base_uniforms; // for ALL shaders
static GLuint time_now; // time in sine wave

void display_object(Object object) {
    //Shader selection
    switch (object.shading) {
        case ShadingType::GOURAUD:
        case ShadingType::PHONG:
        case ShadingType::BLINN:
        case ShadingType::TOON:
            glUseProgram(shaders_IDs[object.shading]);
            // Caricamento matrice trasformazione del modello
            glUniformMatrix4fv(base_uniforms[object.shading].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
            //Material loading
            glUniform3f(light_uniforms[object.shading].light_position_pointer, light.position.x, light.position.y, light.position.z); // Added light update
            glUniform3fv(light_uniforms[object.shading].material_ambient, 1, glm::value_ptr(materials[object.material].ambient));
            glUniform3fv(light_uniforms[object.shading].material_diffuse, 1, glm::value_ptr(materials[object.material].diffuse));
            glUniform3fv(light_uniforms[object.shading].material_specular, 1, glm::value_ptr(materials[object.material].specular));
            glUniform1f(light_uniforms[object.shading].material_shininess, materials[object.material].shininess);
            break;
        case ShadingType::TEXTURE_PHONG:
            glUseProgram(shaders_IDs[TEXTURE_PHONG]);
            // Caricamento matrice trasformazione del modello
            glUniformMatrix4fv(base_uniforms[TEXTURE_PHONG].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
            glActiveTexture(GL_TEXTURE0); // this addresses the first sampler2D uniform in the shader
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glBindTexture(GL_TEXTURE_2D, loadTexture(textures_data[object.textureID]));
            //Material loading
            glUniform3f(light_uniforms[TEXTURE_PHONG].light_position_pointer, light.position.x, light.position.y, light.position.z); // Added light update
            glUniform3fv(light_uniforms[TEXTURE_PHONG].material_ambient, 1, glm::value_ptr(materials[object.material].ambient));
            glUniform3fv(light_uniforms[TEXTURE_PHONG].material_diffuse, 1, glm::value_ptr(materials[object.material].diffuse));
            glUniform3fv(light_uniforms[TEXTURE_PHONG].material_specular, 1, glm::value_ptr(materials[object.material].specular));
            glUniform1f(light_uniforms[TEXTURE_PHONG].material_shininess, materials[object.material].shininess);
            break;
        case ShadingType::WAVE:
            glUseProgram(shaders_IDs[WAVE]);
            // Caricamento matrice trasformazione del modello
            glUniformMatrix4fv(base_uniforms[WAVE].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
            // Material loading
            glUniform3f(light_uniforms[WAVE].light_position_pointer, light.position.x, light.position.y, light.position.z); // Added light update
            glUniform3fv(light_uniforms[WAVE].material_ambient, 1, glm::value_ptr(materials[object.material].ambient));
            glUniform3fv(light_uniforms[WAVE].material_diffuse, 1, glm::value_ptr(materials[object.material].diffuse));
            glUniform3fv(light_uniforms[WAVE].material_specular, 1, glm::value_ptr(materials[object.material].specular));
            glUniform1f(light_uniforms[WAVE].material_shininess, materials[object.material].shininess);
            glActiveTexture(GL_TEXTURE0); // this addresses the first sampler2D uniform in the shader
            glBindTexture(GL_TEXTURE_2D, textures[object.textureID]);
            glUniform1f(time_now, (float) glutGet(GLUT_ELAPSED_TIME));
            // there is a wave object in the shane call redisplay
            glutPostRedisplay();
            break;
        case ShadingType::TEXTURE_ONLY:
            glUseProgram(shaders_IDs[TEXTURE_ONLY]);
            // Caricamento matrice trasformazione del modello
            glUniformMatrix4fv(base_uniforms[TEXTURE_ONLY].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
            glActiveTexture(GL_TEXTURE0); // this addresses the first sampler2D uniform in the shader
            glBindTexture(GL_TEXTURE_2D, loadTexture(textures_data[object.textureID]));
            break;
        case ShadingType::PASS_THROUGH:
            glUseProgram(shaders_IDs[PASS_THROUGH]);
            // Caricamento matrice trasformazione del modello
            glUniformMatrix4fv(base_uniforms[PASS_THROUGH].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
            break;
        default:
            break;
    }
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(object.mesh.vertexArrayObjID);
    glDrawArrays(GL_TRIANGLES, 0, object.mesh.vertices.size());

    // Disable Vertex array to avoid invalid pointers errors
    // https://stackoverflow.com/questions/12427880/is-it-important-to-call-gldisablevertexattribarray
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}