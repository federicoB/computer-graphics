//
// Created by fede on 28/09/20.
//
// Contains code for showing specific objects on shene
//

void init_sphere_FLAT() {

    Mesh sphereF = {};
    loadObjFile(MeshDir + "sphere_n_t_flat.obj", &sphereF);
    generate_and_load_buffers(true, &sphereF);
    // Object Setup with NO texture, will use the light shader and a material for color and light behavior
    Object obj3 = {};
    obj3.mesh = sphereF;
    obj3.material = MaterialType::EMERALD;
    obj3.shading = ShadingType::GOURAUD;
    obj3.name = "Sphere FLAT";
    obj3.model_matrix = glm::translate(glm::mat4(1), glm::vec3(3., 0., -6.));
    objects.push_back(obj3);
}

void init_sphere_SMOOTH() {
    Mesh sphereS = {};
    loadObjFile(MeshDir + "sphere_n_t_smooth.obj", &sphereS);
    generate_and_load_buffers(true, &sphereS);
    // Object Setup with NO texture, will use the light shader and a material for color and light behavior
    Object obj4 = {};
    obj4.mesh = sphereS;
    obj4.material = MaterialType::RED_PLASTIC;
    obj4.shading = ShadingType::BLINN;
    obj4.name = "Sphere SMOOTH";
    obj4.model_matrix = glm::translate(glm::mat4(1), glm::vec3(6., 0., -3.));
    objects.push_back(obj4);
}

//Textured plane (2 triangles) with a procedural texture but no material, use a texture-only shader
void init_textured_plane() {

    Mesh surface = {};
    surface.vertices = { {-2,0,-2}, {-2,0,2}, {2,0,2}, {2,0,-2}, {-2,0,-2}, {2,0,2} };
    surface.normals = { {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0} };
    //Tex coords are out of bound to show the GL_REPEAT effect
    //surface.texCoords = { {-4,-4}, {-4,+4}, {4,4}, {4,-4}, {-4,-4}, {4,4} }; // out of bound UVs
    surface.texCoords = { {0,0}, {0,1}, {1,1}, {1,0}, {0,0}, {1,1} }; // standard UVs
    generate_and_load_buffers(true, &surface);
    Object obj0 = {};
    obj0.mesh = surface;
    obj0.material = MaterialType::NO_MATERIAL;
    obj0.shading = ShadingType::TEXTURE_ONLY;

    /////////////////////////////////////////////////////////////////////////
    //  Compute checkboard procedural_texture image of dimension width x width x 3 (RGB)
    /////////////////////////////////////////////////////////////////////////
    GLubyte image[64][64][3];
    int i, j, c;
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
            image[i][j][0] = (GLubyte)c;
            image[i][j][1] = (GLubyte)c;
            image[i][j][2] = (GLubyte)c;
        }
    }
    /////////////////////////////////////////
    glGenTextures(1, &obj0.textureID);
    glBindTexture(GL_TEXTURE_2D, obj0.textureID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,  //the target
                 0, // the mip map level we want to generate
                 GL_RGB, // the format of the texture
                 64, //texture_size, width
                 64, //texture_size, heigth
                 0,  // border, leave 0
                 GL_RGB, // we assume is a RGB color image with 24 bit depth per pixel
                 GL_UNSIGNED_BYTE, // the data type
                 image);
    obj0.name = "Plane";
    obj0.model_matrix = glm::translate(glm::mat4(1), glm::vec3(-5., 0., -5.));
    objects.push_back(obj0);
}

void init_cube() {

    Mesh cube = {};
    loadObjFile(MeshDir + "cube_n_t_flat.obj", &cube);
    generate_and_load_buffers(true, &cube);
    Object obj1 = {};
    obj1.mesh = cube;
    obj1.material = MaterialType::NO_MATERIAL;
    obj1.shading = ShadingType::TEXTURE_ONLY;
    obj1.textureID = loadTexture(TextureDir + "cube_tex.jpg");
    obj1.name = "Textured Cube";
    obj1.model_matrix = glm::translate(glm::mat4(1), glm::vec3(-5., 0., 5.));
    objects.push_back(obj1);
}

void init_light_object() {
    Mesh sphereS = {};
    loadObjFile(MeshDir + "sphere_n_t_smooth.obj", &sphereS);
    generate_and_load_buffers(true, &sphereS);
    Object obj = {};
    obj.mesh = sphereS;
    obj.material = MaterialType::NO_MATERIAL;
    obj.shading = ShadingType::PASS_THROUGH;
    obj.name = "light";
    obj.model_matrix = glm::scale(glm::translate(glm::mat4(1), light.position), glm::vec3(0.2, 0.2, 0.2));
    objects.push_back(obj);
}

void init_axis() {
    Mesh _grid = {};
    loadObjFile(MeshDir + "axis.obj", &_grid);
    generate_and_load_buffers(true, &_grid);
    Object obj1 = {};
    obj1.mesh = _grid;
    obj1.material = MaterialType::NO_MATERIAL;
    obj1.shading = ShadingType::TEXTURE_ONLY;
    obj1.textureID = loadTexture(TextureDir + "AXIS_TEX.png");
    obj1.name = "axis";
    obj1.model_matrix = glm::scale(glm::mat4(1),glm::vec3(2.f,2.f,2.f));
    Axis = obj1;
}

void init_grid() {
    Mesh _grid = {};
    loadObjFile(MeshDir + "reference_grid.obj", &_grid);
    generate_and_load_buffers(true, &_grid);
    Object obj1 = {};
    obj1.mesh = _grid;
    obj1.material = MaterialType::NO_MATERIAL;
    obj1.shading = ShadingType::PASS_THROUGH;
    obj1.name = "grid";
    obj1.model_matrix = glm::scale(glm::mat4(1), glm::vec3(2.f, 2.f, 2.f));
    Grid = obj1;
}

/*
 * Computes the Vertex attributes data for segment number j of wrap number i.
 */
void computeTorusVertex(int i, int j, Mesh* mesh) {

    float theta = 2.f * glm::pi<float>() *(float)i / (float)TorusSetup.NumWraps;
    float phi = 2.f * glm::pi<float>()*(float)j / (float)TorusSetup.NumPerWrap;
    float sinphi = sin(phi);
    float cosphi = cos(phi);
    float sintheta = sin(theta);
    float costheta = cos(theta);

    float tmp = TorusSetup.MajorRadius + TorusSetup.MinorRadius * cosphi;
    float x = sintheta * tmp;
    float y = TorusSetup.MinorRadius * sinphi;
    float z = costheta * tmp;

    mesh->vertices.push_back(glm::vec3(x, y, z));
    mesh->normals.push_back(glm::vec3(sintheta*cosphi, sinphi, costheta*cosphi));
    mesh->texCoords.push_back(glm::vec2(0));
}

void compute_Torus(Mesh* mesh)
{
    mesh->vertices.clear();
    mesh->normals.clear();
    mesh->texCoords.clear();
    // draw the torus as NumWraps strips one next to the other
    for (int i = 0; i < TorusSetup.NumWraps; i++) {
        for (int j = 0; j <= TorusSetup.NumPerWrap; j++) {
            // first face   3
            //				| \
			//				1--2
            computeTorusVertex(i, j, mesh);
            computeTorusVertex(i + 1, j,mesh);
            computeTorusVertex(i , j + 1, mesh);
            // second face  1--3
            //				 \ |
            //				   2
            computeTorusVertex(i, j + 1, mesh);
            computeTorusVertex(i + 1, j, mesh);
            computeTorusVertex(i + 1, j + 1, mesh);
        }
    }
}

void init_torus() {

    Mesh torus = {};
    compute_Torus(&torus);
    generate_and_load_buffers(true, &torus);
    // Object Setup with NO texture, will use the light shader and a material for color
    Object obj2 = {};
    obj2.mesh = torus;
    obj2.material = MaterialType::BRASS;
    obj2.shading = ShadingType::BLINN; // GOURAUD;
    obj2.name = "Torus";
    obj2.model_matrix = glm::translate(glm::mat4(1), glm::vec3(5., 0., 5.));
    objects.push_back(obj2);
    TorusSetup.torus_index = objects.size() - 1;
}