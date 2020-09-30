//
// Created by fede on 29/09/20.
//

void material_setup(){
    // Materials setup
    materials.resize(5);
    materials[MaterialType::RED_PLASTIC].name = "Red Plastic";
    materials[MaterialType::RED_PLASTIC].ambient = red_plastic_ambient;
    materials[MaterialType::RED_PLASTIC].diffuse = red_plastic_diffuse;
    materials[MaterialType::RED_PLASTIC].specular = red_plastic_specular;
    materials[MaterialType::RED_PLASTIC].shininess = red_plastic_shininess;

    materials[MaterialType::EMERALD].name = "Emerald";
    materials[MaterialType::EMERALD].ambient = emerald_ambient;
    materials[MaterialType::EMERALD].diffuse = emerald_diffuse;
    materials[MaterialType::EMERALD].specular = emerald_specular;
    materials[MaterialType::EMERALD].shininess = emerald_shininess;

    materials[MaterialType::BRASS].name = "Brass";
    materials[MaterialType::BRASS].ambient = brass_ambient;
    materials[MaterialType::BRASS].diffuse = brass_diffuse;
    materials[MaterialType::BRASS].specular = brass_specular;
    materials[MaterialType::BRASS].shininess = brass_shininess;

    materials[MaterialType::SLATE].name = "Slate";
    materials[MaterialType::SLATE].ambient = slate_ambient;
    materials[MaterialType::SLATE].diffuse = slate_diffuse;
    materials[MaterialType::SLATE].specular = slate_specular;
    materials[MaterialType::SLATE].shininess = slate_shiness;

    materials[MaterialType::NO_MATERIAL].name = "NO_MATERIAL";
    materials[MaterialType::NO_MATERIAL].ambient = glm::vec3(1,1,1);
    materials[MaterialType::NO_MATERIAL].diffuse = glm::vec3(0, 0, 0);
    materials[MaterialType::NO_MATERIAL].specular = glm::vec3(0, 0, 0);
    materials[MaterialType::NO_MATERIAL].shininess = 1.f;
}

void shaders_setup() {
    // SHADERS configuration section
    shaders_IDs.resize(NUM_SHADERS);
    light_uniforms.resize(NUM_SHADERS); // allocate space for uniforms of PHONG, BLINN and GOURAND + TOON
    base_uniforms.resize(NUM_SHADERS); // allocate space for uniforms of PHONG,BLINN,GOURAND,TOON,WAVE,TEXTURE_ONLY

    //Gourand Shader loading
    shaders_IDs[GOURAUD] = initShader(ShaderDir + "v_gouraud.glsl", ShaderDir + "f_gouraud.glsl");
    BaseShaderUniform base_unif = {};
    //Otteniamo i puntatori alle variabili uniform per poterle utilizzare in seguito
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[GOURAUD], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[GOURAUD], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[GOURAUD], "M");
    base_uniforms[ShadingType::GOURAUD] = base_unif;
    LightShaderUniform light_unif = {};
    light_unif.material_ambient = glGetUniformLocation(shaders_IDs[GOURAUD], "material.ambient");
    light_unif.material_diffuse = glGetUniformLocation(shaders_IDs[GOURAUD], "material.diffuse");
    light_unif.material_specular = glGetUniformLocation(shaders_IDs[GOURAUD], "material.specular");
    light_unif.material_shininess = glGetUniformLocation(shaders_IDs[GOURAUD], "material.shininess");
    light_unif.light_position_pointer = glGetUniformLocation(shaders_IDs[GOURAUD], "light.position");
    light_unif.light_color_pointer = glGetUniformLocation(shaders_IDs[GOURAUD], "light.color");
    light_unif.light_power_pointer = glGetUniformLocation(shaders_IDs[GOURAUD], "light.power");
    light_uniforms[ShadingType::GOURAUD] = light_unif;
    //Rendiamo attivo lo shader
    glUseProgram(shaders_IDs[GOURAUD]);
    //Shader uniforms initialization
    glUniform3f(light_uniforms[GOURAUD].light_position_pointer, light.position.x, light.position.y, light.position.z);
    glUniform3f(light_uniforms[GOURAUD].light_color_pointer, light.color.r, light.color.g, light.color.b);
    glUniform1f(light_uniforms[GOURAUD].light_power_pointer, light.power);

    //Phong Shader loading
    shaders_IDs[PHONG] = initShader(ShaderDir + "v_phong.glsl", ShaderDir + "f_phong.glsl");
    //Otteniamo i puntatori alle variabili uniform per poterle utilizzare in seguito
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[PHONG], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[PHONG], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[PHONG], "M");
    base_uniforms[ShadingType::PHONG] = base_unif;
    light_unif.material_ambient = glGetUniformLocation(shaders_IDs[PHONG], "material.ambient");
    light_unif.material_diffuse = glGetUniformLocation(shaders_IDs[PHONG], "material.diffuse");
    light_unif.material_specular = glGetUniformLocation(shaders_IDs[PHONG], "material.specular");
    light_unif.material_shininess = glGetUniformLocation(shaders_IDs[PHONG], "material.shininess");
    light_unif.light_position_pointer = glGetUniformLocation(shaders_IDs[PHONG], "light.position");
    light_unif.light_color_pointer = glGetUniformLocation(shaders_IDs[PHONG], "light.color");
    light_unif.light_power_pointer = glGetUniformLocation(shaders_IDs[PHONG], "light.power");
    light_uniforms[ShadingType::PHONG] = light_unif;
    //Rendiamo attivo lo shader
    glUseProgram(shaders_IDs[PHONG]);
    //Shader uniforms initialization
    glUniform3f(light_uniforms[PHONG].light_position_pointer, light.position.x, light.position.y, light.position.z);
    glUniform3f(light_uniforms[PHONG].light_color_pointer, light.color.r, light.color.g, light.color.b);
    glUniform1f(light_uniforms[PHONG].light_power_pointer, light.power);

    //Texture_Phong Shader loading
    shaders_IDs[TEXTURE_PHONG] = initShader(ShaderDir + "v_texture_phong.glsl", ShaderDir + "f_texture_phong.glsl");
    //Otteniamo i puntatori alle variabili uniform per poterle utilizzare in seguito
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "M");
    base_uniforms[ShadingType::TEXTURE_PHONG] = base_unif;
    light_unif.material_ambient = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "material.ambient");
    light_unif.material_diffuse = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "material.diffuse");
    light_unif.material_specular = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "material.specular");
    light_unif.material_shininess = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "material.shininess");
    light_unif.light_position_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "light.position");
    light_unif.light_color_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "light.color");
    light_unif.light_power_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "light.power");
    light_uniforms[ShadingType::TEXTURE_PHONG] = light_unif;
    //Rendiamo attivo lo shader
    glUseProgram(shaders_IDs[TEXTURE_PHONG]);
    //Shader uniforms initialization
    glUniform1i(glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "textureBuffer"), 0);
    glUniform3f(light_uniforms[TEXTURE_PHONG].light_position_pointer, light.position.x, light.position.y, light.position.z);
    glUniform3f(light_uniforms[TEXTURE_PHONG].light_color_pointer, light.color.r, light.color.g, light.color.b);
    glUniform1f(light_uniforms[TEXTURE_PHONG].light_power_pointer, light.power);

    //Blinn Shader loading
    shaders_IDs[BLINN] = initShader(ShaderDir + "v_blinn.glsl", ShaderDir + "f_blinn.glsl");
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[BLINN], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[BLINN], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[BLINN], "M");
    base_uniforms[ShadingType::BLINN] = base_unif;
    light_unif.material_ambient = glGetUniformLocation(shaders_IDs[BLINN], "material.ambient");
    light_unif.material_diffuse = glGetUniformLocation(shaders_IDs[BLINN], "material.diffuse");
    light_unif.material_specular = glGetUniformLocation(shaders_IDs[BLINN], "material.specular");
    light_unif.material_shininess = glGetUniformLocation(shaders_IDs[BLINN], "material.shininess");
    light_unif.light_position_pointer = glGetUniformLocation(shaders_IDs[BLINN], "light.position");
    light_unif.light_color_pointer = glGetUniformLocation(shaders_IDs[BLINN], "light.color");
    light_unif.light_power_pointer = glGetUniformLocation(shaders_IDs[BLINN], "light.power");
    light_uniforms[ShadingType::BLINN] = light_unif;
    //Rendiamo attivo lo shader
    glUseProgram(shaders_IDs[BLINN]);
    //Shader uniforms initialization
    glUniform3f(light_uniforms[BLINN].light_position_pointer, light.position.x, light.position.y, light.position.z);
    glUniform3f(light_uniforms[BLINN].light_color_pointer, light.color.r, light.color.g, light.color.b);
    glUniform1f(light_uniforms[BLINN].light_power_pointer, light.power);

    //Texture Shader loading
    shaders_IDs[TEXTURE_ONLY] = initShader(ShaderDir + "v_texture.glsl", ShaderDir + "f_texture.glsl");
    //Otteniamo i puntatori alle variabili uniform per poterle utilizzare in seguito
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_ONLY], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_ONLY], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_ONLY], "M");
    base_uniforms[ShadingType::TEXTURE_ONLY] = base_unif;

    //Pass-Through Shader loading
    shaders_IDs[PASS_THROUGH] = initShader(ShaderDir + "v_passthrough.glsl", ShaderDir + "f_passthrough.glsl");
    //Otteniamo i puntatori alle variabili uniform per poterle utilizzare in seguito
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[PASS_THROUGH], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[PASS_THROUGH], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[PASS_THROUGH], "M");
    base_uniforms[ShadingType::PASS_THROUGH] = base_unif;
    glUseProgram(shaders_IDs[PASS_THROUGH]);
    glUniform4fv(glGetUniformLocation(shaders_IDs[PASS_THROUGH], "Color"), 1, value_ptr(glm::vec4(1.0, 1.0, 1.0, 1.0)));

    //Toon Shader loading
    shaders_IDs[TOON] = initShader(ShaderDir + "v_toon.glsl", ShaderDir + "f_toon.glsl");
    //Otteniamo i puntatori alle variabili uniform per poterle utilizzare in seguito
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[TOON], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[TOON], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[TOON], "M");
    base_uniforms[ShadingType::TOON] = base_unif;
    light_unif.material_ambient = glGetUniformLocation(shaders_IDs[TOON], "material.ambient");
    light_unif.material_diffuse = glGetUniformLocation(shaders_IDs[TOON], "material.diffuse");
    light_unif.material_specular = glGetUniformLocation(shaders_IDs[TOON], "material.specular");
    light_unif.material_shininess = glGetUniformLocation(shaders_IDs[TOON], "material.shininess");
    light_unif.light_position_pointer = glGetUniformLocation(shaders_IDs[TOON], "light.position");
    light_unif.light_color_pointer = glGetUniformLocation(shaders_IDs[TOON], "light.color");
    light_unif.light_power_pointer = glGetUniformLocation(shaders_IDs[TOON], "light.power");
    light_uniforms[ShadingType::TOON] = light_unif;
    //Rendiamo attivo lo shader
    glUseProgram(shaders_IDs[TOON]);
    //Shader uniforms initialization
    glUniform3f(light_uniforms[TOON].light_position_pointer, light.position.x, light.position.y, light.position.z);
    glUniform3f(light_uniforms[TOON].light_color_pointer, light.color.r, light.color.g, light.color.b);
    glUniform1f(light_uniforms[TOON].light_power_pointer, light.power);

    //Texture_Phong Shader loading
    shaders_IDs[TEXTURE_PHONG] = initShader(ShaderDir + "v_texture_phong.glsl", ShaderDir + "f_texture_phong.glsl");
    //Otteniamo i puntatori alle variabili uniform per poterle utilizzare in seguito
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "M");
    base_uniforms[ShadingType::TEXTURE_PHONG] = base_unif;
    light_unif.material_ambient = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "material.ambient");
    light_unif.material_diffuse = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "material.diffuse");
    light_unif.material_specular = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "material.specular");
    light_unif.material_shininess = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "material.shininess");
    light_unif.light_position_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "light.position");
    light_unif.light_color_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "light.color");
    light_unif.light_power_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "light.power");
    light_uniforms[ShadingType::TEXTURE_PHONG] = light_unif;
    //Rendiamo attivo lo shader
    glUseProgram(shaders_IDs[TEXTURE_PHONG]);
    //Shader uniforms initialization
    glUniform1i(glGetUniformLocation(shaders_IDs[TEXTURE_PHONG], "textureBuffer"), 0);
    glUniform3f(light_uniforms[TEXTURE_PHONG].light_position_pointer, light.position.x, light.position.y, light.position.z);
    glUniform3f(light_uniforms[TEXTURE_PHONG].light_color_pointer, light.color.r, light.color.g, light.color.b);
    glUniform1f(light_uniforms[TEXTURE_PHONG].light_power_pointer, light.power);


    //Texture Shader loading
    shaders_IDs[TEXTURE_ONLY] = initShader(ShaderDir + "v_texture.glsl", ShaderDir + "f_texture.glsl");
    //Otteniamo i puntatori alle variabili uniform per poterle utilizzare in seguito
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_ONLY], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_ONLY], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_ONLY], "M");
    base_uniforms[ShadingType::TEXTURE_ONLY] = base_unif;
    // Rendiamo attivo lo shader
    glUseProgram(shaders_IDs[TEXTURE_ONLY]);
    // Shader uniforms initialization
    glUniform1i(glGetUniformLocation(shaders_IDs[TEXTURE_ONLY], "textureBuffer"), 0);

    //Wave Shader loading
    shaders_IDs[WAVE] = initShader(ShaderDir + "v_wave.glsl", ShaderDir + "f_wave.glsl");
    //Otteniamo i puntatori alle variabili uniform per poterle utilizzare in seguito
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[WAVE], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[WAVE], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[WAVE], "M");
    base_uniforms[ShadingType::WAVE] = base_unif;
    light_unif.material_ambient = glGetUniformLocation(shaders_IDs[WAVE], "material.ambient");
    light_unif.material_diffuse = glGetUniformLocation(shaders_IDs[WAVE], "material.diffuse");
    light_unif.material_specular = glGetUniformLocation(shaders_IDs[WAVE], "material.specular");
    light_unif.material_shininess = glGetUniformLocation(shaders_IDs[WAVE], "material.shininess");
    light_unif.light_position_pointer = glGetUniformLocation(shaders_IDs[WAVE], "light.position");
    light_unif.light_color_pointer = glGetUniformLocation(shaders_IDs[WAVE], "light.color");
    light_unif.light_power_pointer = glGetUniformLocation(shaders_IDs[WAVE], "light.power");
    light_uniforms[ShadingType::WAVE] = light_unif;
    time_now = glGetUniformLocation(shaders_IDs[WAVE], "time");
    //Rendiamo attivo lo shader
    glUseProgram(shaders_IDs[WAVE]);
    //Shader uniforms initialization
    glUniform3f(light_uniforms[WAVE].light_position_pointer, light.position.x, light.position.y, light.position.z);
    glUniform3f(light_uniforms[WAVE].light_color_pointer, light.color.r, light.color.g, light.color.b);
    glUniform1f(light_uniforms[WAVE].light_power_pointer, light.power);

}

