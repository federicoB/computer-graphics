//
// Created by fede on 04/09/20.
//

const string MeshDir = "Mesh/";

// legge un file obj ed inizializza i vector della mesh in input
void loadObjFile(string file_path, Mesh *mesh) {
    FILE *file = fopen(file_path.c_str(), "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return;
    }
    char lineHeader[128];
    while (fscanf(file, "%s", lineHeader) != EOF) {
        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, " %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            mesh->vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "f") == 0) {
            std::string vertex1, vertex2, vertex3;
            GLuint a, b, c;
            fscanf(file, " %d %d %d\n", &a, &b, &c);
            a--;
            b--;
            c--;
            mesh->indices.push_back(a);
            mesh->indices.push_back(b);
            mesh->indices.push_back(c);
        }
    }

    // Norms allocation
    // use resize to guarantee size and default value
    mesh->normals.resize(mesh->vertices.size(), glm::vec3(0.0,0.0,0.0));
    // triangular mesh, Max-Nelson Method
    for (int i = 0; i < mesh->indices.size(); i += 3) {
        GLuint a = mesh->indices[i];
        GLuint b = mesh->indices[i + 1];
        GLuint c = mesh->indices[i + 2];

        glm::vec3 normal = normalize(cross(mesh->vertices[b] - mesh->vertices[a], mesh->vertices[c]- mesh->vertices[a]));

        mesh->normals[a] = normal;
        mesh->normals[b] = normal;
        mesh->normals[c] = normal;
    }

    // normalization of averange of norms for each vertex
    for (int i = 0; i < mesh->normals.size(); i++)
        mesh->normals[i] = normalize(mesh->normals[i]);

    glEnable(GL_NORMALIZE);
}

// Genera i buffer per la mesh in input e ne salva i puntatori di openGL
void generate_and_load_buffers(Mesh *mesh) {
    // generate 1 Vertex Array Object
    glGenVertexArrays(1, &mesh->vertexArrayObjID);
    glBindVertexArray(mesh->vertexArrayObjID);

    // generate 1 Vertex Buffer Object for vertices
    glGenBuffers(1, &mesh->vertexBufferObjID);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferObjID);
    // GL_STATIC_DRAW -> data stored will not be modified
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(glm::vec3), &mesh->vertices[0], GL_STATIC_DRAW);
    glVertexPointer(
            3,                  // size
            GL_FLOAT,           // type
            0,                  // stride
            (void *) 0            // array buffer offset
    );

    //caricamento normali in memoria
    glGenBuffers(1, &mesh->normalBufferObjID);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->normalBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(glm::vec3), &mesh->normals[0], GL_STATIC_DRAW);
    glNormalPointer(
            GL_FLOAT,           // type
            0,                  // stride
            (void*)0            // array buffer offset
    );

    // generate 1 Element Buffer Object for indicies, Nota: GL_ELEMENT_ARRAY_BUFFER
    // this array will contain indicies refering to previous shape
    glGenBuffers(1, &mesh->indexBufferObjID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(GLshort), mesh->indices.data(), GL_STATIC_DRAW);
}