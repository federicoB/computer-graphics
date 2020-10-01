//
// Created by fede on 28/09/20.
//
// Contains code for showing specific objects on shene
//

struct {
    // Variables controlling the torus mesh resolution
    int NumWraps = 10;
    int NumPerWrap = 8;
    // Variables controlling the size of the torus
    float MajorRadius = 3.0;
    float MinorRadius = 1.0;
    int torus_index;
} TorusSetup;

void init_sphere_FLAT() {
    Mesh sphereF = {};
    loadObjFile(MeshDir + "sphere_n_t_flat.obj", &sphereF);
    generate_and_load_buffers(true, &sphereF);
    // Object Setup with NO texture, will use the light shader and a material for color and light behavior
    Object obj3 = {};
    obj3.mesh = sphereF;
    obj3.material = MaterialType::EMERALD;
    obj3.shading = ShadingType::GOURAUD;
    obj3.textureID = 4;
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
    obj4.textureID = 4;
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
    obj0.material = MaterialType::RED_PLASTIC;
    obj0.shading = ShadingType::TEXTURE_ONLY;
    obj0.textureID = 5;
    obj0.name = "Plane";
    obj0.model_matrix = glm::translate(glm::mat4(1), glm::vec3(0.,-1, 0));
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
    obj1.textureID = 2; //cube
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
    obj.textureID = 4;
    obj.name = "light";
    obj.model_matrix = glm::scale(glm::translate(glm::mat4(1), glm::vec3(-3., 6., -3.)), glm::vec3(0.2, 0.2, 0.2));
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
    obj1.textureID = 0;
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
    obj1.textureID = 6;
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
    // texture coordinates
    mesh->texCoords.push_back(glm::vec2(j / (float)TorusSetup.NumPerWrap, i / (float)TorusSetup.NumWraps));
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
    obj2.shading = ShadingType::TEXTURE_ONLY; // GOURAUD;
    obj2.name = "Torus";
    obj2.textureID = 5;
    obj2.model_matrix = glm::translate(glm::mat4(1), glm::vec3(5., 0., 5.));
    objects.push_back(obj2);
    TorusSetup.torus_index = objects.size() - 1;
}