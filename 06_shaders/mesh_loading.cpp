//
// Created by fede on 04/09/20.
//

//texture loading utility
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const string MeshDir = "Mesh/";

// legge un file obj ed inizializza i vector della mesh in input
void loadObjFile(string file_path, Mesh *mesh) {
    FILE * file = fopen(file_path.c_str(), "r");
    if (file == NULL) {
        std::cerr << "\nFailed to open obj file! --> " << file_path << std::endl;
        return;
    }
    // tmp data structures
    vector<GLuint> vertexIndices, normalIndices, uvIndices;
    vector<glm::vec3> tmp_vertices,  tmp_normals;
    vector<glm::vec2> tmp_uvs;

    char lineHeader[128];
    while (fscanf(file, "%s", lineHeader) != EOF) {
        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, " %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            tmp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, " %f %f %f\n", &normal.x, &normal.y, &normal.z);
            tmp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, " %f %f\n", &uv.x, &uv.y);
            uv.y = 1 - uv.y;
            tmp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            GLuint v_a, v_b, v_c; // index in position array
            GLuint n_a, n_b, n_c; // index in normal array
            GLuint t_a, t_b, t_c; // index in UV array

            fscanf(file, "%s", lineHeader);
            if (strstr(lineHeader, "//")) { // case: v//n v//n v//n (vertex and normal)
                sscanf(lineHeader, "%d//%d",&v_a, &n_a);
                fscanf(file,"%d//%d %d//%d\n", &v_b, &n_b, &v_c, &n_c);
                n_a--, n_b--, n_c--;
                normalIndices.push_back(n_a); normalIndices.push_back(n_b); normalIndices.push_back(n_c);
            }
            else if (strstr(lineHeader, "/")) {// case: v/t/n v/t/n v/t/n (vertex, normal and texture coordinate)
                sscanf(lineHeader, "%d/%d/%d", &v_a, &t_a, &n_a);
                fscanf(file, "%d/%d/%d %d/%d/%d\n", &v_b, &t_b, &n_b, &v_c, &t_c, &n_c);
                n_a--, n_b--, n_c--;
                t_a--, t_b--, t_c--;
                normalIndices.push_back(n_a); normalIndices.push_back(n_b); normalIndices.push_back(n_c);
                uvIndices.push_back(t_a); uvIndices.push_back(t_b); uvIndices.push_back(t_c);
            }
            else {// case: v v v (only vertex)
                sscanf(lineHeader, "%d", &v_a);
                fscanf(file,"%d %d\n", &v_b, &v_c);
            }
            v_a--; v_b--; v_c--;
            vertexIndices.push_back(v_a); vertexIndices.push_back(v_b); vertexIndices.push_back(v_c);
        }
    }
    fclose(file);

    // If normals and uvs are not loaded, we calculate them for a default smooth shading effect
    if (tmp_normals.size() == 0) {
        tmp_normals.resize(vertexIndices.size() / 3, glm::vec3(0.0, 0.0, 0.0));
        // normal of each face saved 1 time PER FACE!
        for (int i = 0; i < vertexIndices.size(); i += 3)
        {
            GLushort ia = vertexIndices[i];
            GLushort ib = vertexIndices[i + 1];
            GLushort ic = vertexIndices[i + 2];
            glm::vec3 normal = glm::normalize(glm::cross(
                    glm::vec3(tmp_vertices[ib]) - glm::vec3(tmp_vertices[ia]),
                    glm::vec3(tmp_vertices[ic]) - glm::vec3(tmp_vertices[ia])));
            tmp_normals[ i / 3 ] = normal;
            //Put an index to the normal for all 3 vertex of the face
            normalIndices.push_back(i/3);
            normalIndices.push_back(i/3);
            normalIndices.push_back(i/3);
        }
    }
    //if texture coordinates were not included we fake them
    if (tmp_uvs.size() == 0) {
        tmp_uvs.push_back(glm::vec2(0)); //dummy uv
        for (int i = 0; i < vertexIndices.size(); i += 3)
        {
            // The UV is dummy
            uvIndices.push_back(0);
            uvIndices.push_back(0);
            uvIndices.push_back(0);
        }
    }
    // The data for loaded for the glDrawArrays call, this is a simple but non optimal way of storing mesh data.
    // However, you could optimize the mesh data using a index array for both vertex positions,
    // normals and textures and later use glDrawElements
    int i = 0;
    // Following the index arrays, the final arrays that will contain the data for glDrawArray are built.
    for (int i = 0; i < vertexIndices.size();  i++) {

        mesh->vertices.push_back(tmp_vertices[vertexIndices[i]]);
        mesh->normals.push_back(tmp_normals[normalIndices[i]]);
        mesh->texCoords.push_back(tmp_uvs[uvIndices[i]]);
    }

    glEnable(GL_NORMALIZE);
}


//TODO extract buffer generation and call it only where necessary
// Genera i buffer per la mesh in input e ne salva i puntatori di openGL
/**
 *
 * @param generate bool: if it is necessary to pre-generate
 * @param mesh
 */
void generate_and_load_buffers(bool generate, Mesh *mesh) {

    if (generate) {
        // Genero 1 Vertex Array Object
        glGenVertexArrays(1, &mesh->vertexArrayObjID);
        // Genero 1 Vertex Buffer Object per i vertici
        glGenBuffers(1, &mesh->vertexBufferObjID);
        // Genero 1 Buffer Object per le normali
        glGenBuffers(1, &mesh->normalBufferObjID);
        // Genero 1 Buffer Object per le coordinate texture
        glGenBuffers(1, &mesh->uvBufferObjID);
    }


    glBindVertexArray(mesh->vertexArrayObjID);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferObjID);
    // GL_STATIC_DRAW -> data stored will not be modified
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(glm::vec3), &mesh->vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            0,					// attribute index in the shader
            3,                  // size
            GL_FLOAT,           // type
            false,              // normalized
            0,					// stride
            (void*)0            // array buffer offset
    );

    //normali
    glBindBuffer(GL_ARRAY_BUFFER, mesh->normalBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(glm::vec3), &mesh->normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1,					// attribute index in the shader
            3,                  // size
            GL_FLOAT,           // type
            false,              // normalized
            0,					// stride
            (void*)0            // array buffer offset
    );

    //texture mapping
    glBindBuffer(GL_ARRAY_BUFFER, mesh->uvBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, mesh->texCoords.size() * sizeof(glm::vec2), mesh->texCoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
            2,					// attribute index in the shader
            2,                  // size
            GL_FLOAT,           // type
            false,              // normalized
            0,					// stride
            (void*)0            // array buffer offset
    );

    // Disable Vertex array to avoid invalid pointers errors
    // https://stackoverflow.com/questions/12427880/is-it-important-to-call-gldisablevertexattribarray
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

GLuint loadTexture(string path)
{
    int width, height, texChannels;
    GLuint textureID;
    stbi_uc* pixels = stbi_load(path.data(), &width, &height, &texChannels, STBI_rgb_alpha);
    if (!pixels) {
        std::cerr << "\nFailed to load texture image! --> " << path << std::endl;
        std::getchar();
        exit(EXIT_FAILURE);
    }
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    //Texture displacement logic
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //Texture sampling logic
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // data loading in memory
    glTexImage2D(GL_TEXTURE_2D,  //the target
                 0, // the mip map level we want to generate
                 GL_RGBA,
                 width,
                 height,
                 0, // border, leave 0
                 GL_RGBA, // we assume is a RGB color image with 24 bit depth per pixel
                 GL_UNSIGNED_BYTE, // the data type
                 pixels);
    glGenerateMipmap(GL_TEXTURE_2D);// automatic mip maps generation

    stbi_image_free(pixels);
    return textureID;
}