#include "skybox.h"

void Skybox::Load(std::vector<const char *> faces){
    
    // vertices that composes the box, might want to use indices?
    GLfloat vertices[] = {
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f
    };

    // creates and binds all of the buffer objects, we also use a vertex buffer array to contain
    // all of the other buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // tells opengl to bind the vertices to the in vec3 vertices value in the shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0); // unbind the vao

    // load the textures
    glGenTextures(1, &id);

    // bind the texture id to be a cubemap
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    // loop through all of the faces, which are a list of strings that are paths that point to a list textures
    // the faces should be in the correct format, might need to fix this though to be sure its correct.

    //@todo allow this to be run on a seperate thread, should be pretty easy using future
    for (GLuint i = 0; i < faces.size(); i++){
        int w, h;
        unsigned char * image;
        image = SOIL_load_image(faces[i], &w, &h, 0, SOIL_LOAD_RGB);   // load the image into a byte pointer
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image); // sends the texture information into opengl
        SOIL_free_image_data(image); // frees the byte pointer
    }
    // sets the textures parameters, @todo, allow the creator to set the values
    // for example, you might want the texture to be nearest neighbor filtered for pixely games
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // load the skybox shaders
    shader.Load_Shaders("assets/sk_vertex.glsl", "assets/sk_fragment.glsl");
} 

// @todo make this work, its suposed to load the skybox by loading a file and streaming the 
// paths from there, this would clean up the loading process
void Skybox::Load(const char * path){
    std::vector<const char *> faces;
    std::ifstream file (path);

    std::string line;
    while(std::getline(file, line)){
        faces.push_back((const char *)line.c_str());
    }
    file.close();

    Load(faces);
}

void Skybox::Render(glm::mat4 view, glm::mat4 proj){
    shader.Set(); // enable the shader

    // gets the view matrix, then scales it be be a good size for the skybox
    // @refactor, we shouldnt be hardcoding the size :(
    auto _view = glm::mat4(glm::mat3(view));
    _view = glm::scale(_view, glm::vec3(400, 400, 400));

    // set the matrix uniforms
    shader.Set_Uniform("view", _view);
    shader.Set_Uniform("projection", proj);
    
    // enable the vertices
    glEnableVertexAttribArray(0);

    glBindVertexArray(vao); // bind the vao
    glDepthFunc(GL_LEQUAL); // no idea, just makes this works, @learn

    // sets the active texture to the first one, might want to use more then one?
    glActiveTexture(GL_TEXTURE0);

    // sets the sampler cube to use the 0ith one, which is bound in the loading of the skybox
    shader.Set_Uniform("skybox", 0.0f);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id); // bind the texture id
    glDrawArrays(GL_TRIANGLES, 0, 36);      // draw the skybox mesh
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);  // unbind the texture

    // clean up
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    
    glDisableVertexAttribArray(0);

    shader.Unset();
}

GLuint Skybox::Get_ID(){return id;}