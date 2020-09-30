//
// Created by fede on 29/09/20.
//

#define N_TEXTURE 7
string texture_names[] = {
        "axis",
        "sugar",
        "cube",
        "bombolone",
        "wood",
        "procedural_chess",
        "no_texture"
};


GLuint textures[7];


//texture loading utility
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef struct  {
    stbi_uc* pixels;
    int width;
    int height;
} Texture_data;

Texture_data textures_data[6];

Texture_data load_texture_data(string path) {
    Texture_data texture_data;
    int texChannels;
    texture_data.pixels = stbi_load(path.data(), &texture_data.width, &texture_data.height, &texChannels, STBI_rgb_alpha);
    if (!texture_data.pixels) {
        std::cerr << "\nFailed to load texture image! --> " << path << std::endl;
        std::getchar();
        exit(EXIT_FAILURE);
    }
    return texture_data;
}

GLuint loadTexture(Texture_data texture_data) {
    GLuint textureID;
    glGenTextures(1,&textureID);
    cout << "new texture id " << textureID << endl;
    // bind a named texture to a texturing targett
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
                 texture_data.width,
                 texture_data.height,
                 0, // border, leave 0
                 GL_RGBA, // we assume is a RGB color image with 24 bit depth per pixel
                 GL_UNSIGNED_BYTE, // the data type
                 texture_data.pixels);
    glGenerateMipmap(GL_TEXTURE_2D);// automatic mip maps generation

    return textureID;
}



#include <complex>

int inSet( std::complex<long double> z, std::complex<long double> c, int iterations) {
    long double dist;//, three = 3.0;
    for( int ec = 0; ec < iterations; ec++ ) {
        z = z * z; z = z + c;
        dist = ( z.imag() * z.imag() ) + ( z.real() * z.real() );
        if( dist > 3 ) return( ec );
    }
    return 0;
}

#define RGBA(r,g,b) ( ((unsigned int)(__uint8_t)r)|((unsigned int)((__uint8_t)g)<<8)|((unsigned int)((__uint8_t)b)<<16)|((unsigned int)((__uint8_t)16)<<24 ))

Texture_data gen_fractal_texture() {
    Texture_data texture_data;
    int width = 1024;
    int height = 1024;
    unsigned int* image = new unsigned int[width*height*4];
    srand (time(NULL));
    int iterations = rand()%16+4;
    float FCT = 2.85;
    float hFCT = FCT / 2.0;
    int BMP_SIZE = height;
    std::complex<long double> k(
            ( (FCT / BMP_SIZE) * ((rand()%200)+100) ) - 1.4 ,
            ( (FCT / BMP_SIZE) * ((rand()%400)+100) ) - 2.0);
    std::complex<long double> c;
    std::complex<long double> factor( FCT / BMP_SIZE, FCT / BMP_SIZE ) ;
    int res, pos;
    for( int y = 0; y < BMP_SIZE; y++ ) {
        pos = y * BMP_SIZE;
        c.imag( ( factor.imag() * y ) + -hFCT );
        for( int x = 0; x < BMP_SIZE; x++ ) {
            c.real( factor.real() * x + -hFCT );
            res = inSet( c, k, iterations );
            if( res ) {
                int n_res = res % 255;
                if( res < ( iterations >> 1 ) ) res = RGBA( n_res << 2, n_res << 3, n_res << 4 );
                else res = RGBA( n_res << 4, n_res << 2, n_res << 5 );
            }
            image[pos++] = res;
        }
    }
    int texChannels;
    texture_data.pixels = (stbi_uc*) image;
    texture_data.height = height;
    texture_data.width = width;
    return texture_data;
}

void texture_setup() {
    textures_data[0] = load_texture_data(TextureDir + "AXIS_TEX.png");
    textures_data[1] = load_texture_data(TextureDir + "sugar_color.jpg");
    textures_data[2] = load_texture_data(TextureDir + "cube_tex.jpg");
    textures_data[3] = load_texture_data(TextureDir + "bombolone_2.jpg");
    textures_data[4] = load_texture_data(TextureDir + "WoodGrain.bmp");
    textures_data[5] = gen_fractal_texture();
}