//
// Created by fede on 29/09/20.
//

//Shaders Uniforms
static vector<LightShaderUniform> light_uniforms; // for shaders with light
static vector<BaseShaderUniform> base_uniforms; // for ALL shaders

void display_object(Object object) {
    //Shader selection
    switch (object.shading) {
        case ShadingType::GOURAUD:
            glUseProgram(shaders_IDs[GOURAUD]);
            // Caricamento matrice trasformazione del modello
            glUniformMatrix4fv(base_uniforms[GOURAUD].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
            //Material loading
            glUniform3fv(light_uniforms[GOURAUD].material_ambient, 1, glm::value_ptr(materials[object.material].ambient));
            glUniform3fv(light_uniforms[GOURAUD].material_diffuse, 1, glm::value_ptr(materials[object.material].diffuse));
            glUniform3fv(light_uniforms[GOURAUD].material_specular, 1, glm::value_ptr(materials[object.material].specular));
            glUniform1f(light_uniforms[GOURAUD].material_shininess, materials[object.material].shininess);
            break;
        case ShadingType::PHONG:
            //TODO
            break;
        case ShadingType::BLINN:
            glUseProgram(shaders_IDs[BLINN]);
            // Caricamento matrice trasformazione del modello
            glUniformMatrix4fv(base_uniforms[BLINN].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
            //Material loading
            glUniform3fv(light_uniforms[BLINN].material_ambient, 1, glm::value_ptr(materials[object.material].ambient));
            glUniform3fv(light_uniforms[BLINN].material_diffuse, 1, glm::value_ptr(materials[object.material].diffuse));
            glUniform3fv(light_uniforms[BLINN].material_specular, 1, glm::value_ptr(materials[object.material].specular));
            glUniform1f(light_uniforms[BLINN].material_shininess, materials[object.material].shininess);
            break;
        case ShadingType::TEXTURE_ONLY:
            glUseProgram(shaders_IDs[TEXTURE_ONLY]);
            // Caricamento matrice trasformazione del modello
            glUniformMatrix4fv(base_uniforms[TEXTURE_ONLY].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
            glActiveTexture(GL_TEXTURE0); // this addresses the first sampler2D uniform in the shader
            glBindTexture(GL_TEXTURE_2D, object.textureID);
            break;
        case ShadingType::PASS_THROUGH:
            glUseProgram(shaders_IDs[PASS_THROUGH]);
            // Caricamento matrice trasformazione del modello
            glUniformMatrix4fv(base_uniforms[PASS_THROUGH].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
            break;
        case ShadingType::TOON:
            break;
        case ShadingType::WAVE:
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