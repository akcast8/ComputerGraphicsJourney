#include "Common.h"
#include "OpenGLCommon.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLBgEffect.h"
#include "OpenGLMesh.h"
#include "OpenGLViewer.h"
#include "OpenGLWindow.h"
#include "TinyObjLoader.h"
#include "OpenGLSkybox.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>
#include <string>

#ifndef __Main_cpp__
#define __Main_cpp__

#ifdef __APPLE__
#define CLOCKS_PER_SEC 100000
#endif


class MyDriver : public OpenGLViewer
{
    std::vector<OpenGLTriangleMesh *> mesh_object_array;
    OpenGLBgEffect *bgEffect = nullptr;
    OpenGLSkybox *skybox = nullptr;
    clock_t startTime;
    int frame;

public:
    virtual void Initialize()
    {
        draw_axes = false;
        startTime = clock();
        frame = 1;
        OpenGLViewer::Initialize();
    }

    virtual void Initialize_Data()
    {
        //// Load all the shaders you need for the scene 
        //// In the function call of Add_Shader_From_File(), we specify three names: 
        //// (1) vertex shader file name
        //// (2) fragment shader file name
        //// (3) shader name used in the shader library
        //// When we bind a shader to an object, we implement it as follows:
        //// object->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("shader_name"));
        //// Here "shader_name" needs to be one of the shader names you created previously with Add_Shader_From_File()

        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/basic.frag", "basic");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/environment.frag", "environment");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/stars.vert", "shaders/stars.frag", "stars");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/alphablend.frag", "blend");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/billboard.vert", "shaders/alphablend.frag", "billboard");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/terrain.vert", "shaders/terrain.frag", "terrain");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/skybox.vert", "shaders/skybox.frag", "skybox");

        //// Load all the textures you need for the scene
        //// In the function call of Add_Shader_From_File(), we specify two names:
        //// (1) the texture's file name
        //// (2) the texture used in the texture library
        //// When we bind a texture to an object, we implement it as follows:
        //// object->Add_Texture("tex_sampler", OpenGLTextureLibrary::Get_Texture("tex_name"));
        //// Here "tex_sampler" is the name of the texture sampler2D you used in your shader, and
        //// "tex_name" needs to be one of the texture names you created previously with Add_Texture_From_File()

        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/marble_color.jpg", "marble_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/marble_normal.jpg", "marble_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/earth_color.png", "earth_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/earth_normal.png", "earth_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/Earth_diffuse_8k.png", "earth_diffuse");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/rock_color.jpg", "rock_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/rock_normal.jpg", "rock_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/rock_occlusion.jpg", "rock_occlusion");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/rock_height.png", "rock_height");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/rock_roughness.jpg", "rock_roughness");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/plastic_color.jpg", "plastic_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/plastic_normal.jpg", "plastic_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/neptune_map.jpg", "neptune_map");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/lava_color.png", "lava_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/lava_normal.png", "lava_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/lava_spec.png", "lava_spec");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/lava_occlusion.png", "lava_occlusion");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/lava_disp.png", "lava_disp");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/canyonrock_color.jpg", "canyonrock_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/canyonrock_normal.jpg", "canyonrock_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/canyonrock_occlusion.jpg", "canyonrock_occlusion");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/canyonrock_height.png", "canyonrock_height");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/canyonrock_roughness.jpg", "canyonrock_roughness");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/ufo_color.png", "ufo_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/ufo_normal.png", "ufo_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/hull_color.jpg", "hull_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/hull_normal.jpg", "hull_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/window.png", "galaxy_color");
        //// Add all the lights you need for the scene (no more than 4 lights)
        //// The four parameters are position, ambient, diffuse, and specular.
        //// The lights you declared here will be synchronized to all shaders in uniform lights.
        //// You may access these lights using lt[0].pos, lt[1].amb, lt[1].dif, etc.
        //// You can also create your own lights by directly declaring them in a shader without using Add_Light().
        //// Here we declared three default lights for you. Feel free to add/delete/change them at your will.

        opengl_window->Add_Light(Vector3f(3, 3, 3), Vector3f(0.1, 0.1, 0.1), Vector3f(0.9, 0.9, 0.9), Vector3f(0.5, 0.5, 0.5));

        //// Add the background / environment
        //// Here we provide you with four default options to create the background of your scene:
        //// (1) Gradient color (like A1 and A2; if you want a simple background, use this one)
        //// (2) Programmable Canvas (like A7 and A8; if you consider implementing noise or particles for the background, use this one)
        //// (3) Sky box (cubemap; if you want to load six background images for a skybox, use this one)
        //// (4) Sky sphere (if you want to implement a sky sphere, enlarge the size of the sphere to make it colver the entire scene and update its shaders for texture colors)
        //// By default, Option (2) (Buzz stars) is turned on, and all the other three are commented out.
        
        //// Background Option (1): Gradient color
        /*
        {
            auto bg = Add_Interactive_Object<OpenGLBackground>();
            bg->Set_Color(OpenGLColor(0.1f, 0.1f, 0.1f, 1.f), OpenGLColor(0.3f, 0.1f, .1f, 1.f));
            bg->Initialize();
        }
        */

        //// Background Option (2): Programmable Canvas
        //// By default, we load a GT buzz + a number of stars
        /*
        {
            
            bgEffect = Add_Interactive_Object<OpenGLBgEffect>();
            bgEffect->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("stars"));
            bgEffect->Add_Texture("tex_buzz", OpenGLTextureLibrary::Get_Texture("buzz_color")); // bgEffect can also Add_Texture
            bgEffect->Initialize();
            
        }
        */
        //// Background Option (3): Sky box
        //// Here we provide a default implementation of a sky box; customize it for your own sky box
        
        {
            // from https://www.humus.name/index.php?page=Textures
            const std::vector<std::string> cubemap_files{
                "cubemap/bkg1_right.png",     //// + X
                "cubemap/bkg1_left.png",     //// - X
                "cubemap/bkg1_top.png",     //// + Y
                "cubemap/bkg1_bot.png",     //// - Y
                "cubemap/bkg1_front.png",     //// + Z
                "cubemap/bkg1_back.png",     //// - Z 
            };
            OpenGLTextureLibrary::Instance()->Add_CubeMap_From_Files(cubemap_files, "cube_map");

            skybox = Add_Interactive_Object<OpenGLSkybox>();
            skybox->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("skybox"));
            ////skybox->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("stars"));
            skybox->Initialize();
            ////skybox->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("stars"));
        }
        

        //// Background Option (4): Sky sphere
        //// Here we provide a default implementation of a textured sphere; customize it for your own sky sphere
        {
            /*
            //// create object by reading an obj mesh
            auto sphere = Add_Obj_Mesh_Object("obj/sphere.obj");

            //// set object's transform
            Matrix4f t;
            t << 10, 0, 0, 0,
                0, 10, 0, 0,
                0, 0, 10, 0,
                0, 0, 0, 1;
            sphere->Set_Model_Matrix(t);

            //// set object's material
            sphere->Set_Ka(Vector3f(0.1, 0.1, 0.1));
            sphere->Set_Kd(Vector3f(0.7, 0.7, 0.7));
            sphere->Set_Ks(Vector3f(2, 2, 2));
            sphere->Set_Shininess(128);

            //// bind texture to object
            sphere->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("marble_color"));
            sphere->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("marble_normal"));

            //// bind shader to object
            sphere->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("environment"));
            */
        }

    //// Earth object
        {
            auto earth = Add_Obj_Mesh_Object("obj/Earth.obj");

            Matrix4f t;
            t << .01, 0, 0, 0,
                0, .01, 0, -.5,
                0, 0, .01, 0,
                0, 0, 0, 1;
            earth->Set_Model_Matrix(t);
            
            earth->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("earth_diffuse"));

            earth->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("billboard"));
        }
    //// Planet1 object
        {
            auto planet1 = Add_Obj_Mesh_Object("obj/sphere.obj");

            Matrix4f t;
            t << .3, 0, 0, -1.,
                0, .3, 0, 1.5,
                0, 0, .3, 3.,
                0, 0, 0, 1;
            planet1->Set_Model_Matrix(t);

            planet1->Set_Ka(Vector3f(0.5, 0.5, 0.5));
            planet1->Set_Kd(Vector3f(0.9, 0.9, 0.9));
            planet1->Set_Ks(Vector3f(4, 4, 4));
            planet1->Set_Shininess(256);
            
            planet1->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("rock_color"));
            planet1->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("rock_normal"));
            planet1->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic"));
        }
    //// Planet2 object
        {
            auto planet2 = Add_Obj_Mesh_Object("obj/sphere.obj");

            Matrix4f t;
            t << .5, 0, 0, -2.2,
                0, .5, 0, -2.7,
                0, 0, .5, -3.1,
                0, 0, 0, 1;
            planet2->Set_Model_Matrix(t);

            planet2->Set_Ka(Vector3f(0.4, 0.4, 0.4));
            planet2->Set_Kd(Vector3f(0.3, 0.3, 0.3));
            planet2->Set_Ks(Vector3f(2, 2, 2));
            planet2->Set_Shininess(128);
            
            planet2->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("canyonrock_color"));
            planet2->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("canyonrock_normal"));
            planet2->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic"));
        }
    //// Planet3 object
        {
            auto planet3 = Add_Obj_Mesh_Object("obj/Neptune.obj");

            Matrix4f t;
            t << .05, 0, 0, 3.,
                0, .05, 0, -2.,
                0, 0, .05, 4.8,
                0, 0, 0, 1;
            planet3->Set_Model_Matrix(t);

            planet3->Set_Ka(Vector3f(0.4, 0.4, 0.4));
            planet3->Set_Kd(Vector3f(0.4, 0.4, 0.4));
            planet3->Set_Ks(Vector3f(3, 3, 3));
            planet3->Set_Shininess(200);
            
            planet3->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("neptune_map"));
            planet3->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("plastic_normal"));
            planet3->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic"));
        }
    //// Hull object
        {
            auto hull = Add_Obj_Mesh_Object("obj/hull.obj");

            Matrix4f t;
            t << .05, 0, 0, 2,
                0, .05, 0, 1.8,
                0, 0, .05, 2,
                0, 0, 0, 1;
            hull->Set_Model_Matrix(t);

            hull->Set_Ka(Vector3f(0.5, 0.5, 0.5));
            hull->Set_Kd(Vector3f(0.4, 0.4, 0.4));
            hull->Set_Ks(Vector3f(3, 3, 3));
            hull->Set_Shininess(200);
            
            hull->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("hull_color"));
            hull->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("hull_normal"));
            hull->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic"));
        }
    //// UFO object
        {
            auto ufo = Add_Obj_Mesh_Object("obj/ufo.obj");

            Matrix4f t;
            t << .05, 0, 0, -5,
                0, .05, 0, 2,
                0, 0, .05, -8,
                0, 0, 0, 1;
            ufo->Set_Model_Matrix(t);
            
            ufo->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("ufo_color"));
            ufo->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("ufo_normal"));
            ufo->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("environment"));
        }
        //// Here we show an example of adding a mesh with noise-terrain (A6)
        {
            /*
            //// create object by reading an obj mesh
            auto terrain = Add_Obj_Mesh_Object("obj/plane.obj");

            //// set object's transform
            Matrix4f r, s, t;
            r << 1, 0, 0, 0,
                0, 0.5, 0.67, 0,
                0, -0.67, 0.5, 0,
                0, 0, 0, 1;
            s << 0.5, 0, 0, 0,
                0, 0.5, 0, 0,
                0, 0, 0.5, 0,
                0, 0, 0, 1;
            t << 1, 0, 0, -2,
                 0, 1, 0, 0.5,
                 0, 0, 1, 0,
                 0, 0, 0, 1,
            terrain->Set_Model_Matrix(t * s * r);

            //// set object's material
            terrain->Set_Ka(Vector3f(0.1f, 0.1f, 0.1f));
            terrain->Set_Kd(Vector3f(0.7f, 0.7f, 0.7f));
            terrain->Set_Ks(Vector3f(1, 1, 1));
            terrain->Set_Shininess(128.f);

            //// bind shader to object (we do not bind texture for this object because we create noise for texture)
            terrain->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("terrain"));
            */
        }
    

        //// This for-loop updates the rendering model for each object on the list
        for (auto &mesh_obj : mesh_object_array){
            Set_Polygon_Mode(mesh_obj, PolygonMode::Fill);
            Set_Shading_Mode(mesh_obj, ShadingMode::TexAlpha);
            mesh_obj->Set_Data_Refreshed();
            mesh_obj->Initialize();
        }
        Toggle_Play();
    }

    OpenGLTriangleMesh *Add_Obj_Mesh_Object(std::string obj_file_name)
    {
        auto mesh_obj = Add_Interactive_Object<OpenGLTriangleMesh>();
        Array<std::shared_ptr<TriangleMesh<3>>> meshes;
        // Obj::Read_From_Obj_File(obj_file_name, meshes);
        Obj::Read_From_Obj_File_Discrete_Triangles(obj_file_name, meshes);

        mesh_obj->mesh = *meshes[0];
        std::cout << "load tri_mesh from obj file, #vtx: " << mesh_obj->mesh.Vertices().size() << ", #ele: " << mesh_obj->mesh.Elements().size() << std::endl;

        mesh_object_array.push_back(mesh_obj);
        return mesh_obj;
    }

    //// Go to next frame
    virtual void Toggle_Next_Frame()
    {
        ////skybox->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);
        ////skybox->setFrame(frame++);
        for (auto &mesh_obj : mesh_object_array)
            mesh_obj->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);

        if (bgEffect){
            bgEffect->setResolution((float)Win_Width(), (float)Win_Height());
            bgEffect->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);
            bgEffect->setFrame(frame++);
        }
        OpenGLViewer::Toggle_Next_Frame();
    }

    virtual void Run()
    {
        OpenGLViewer::Run();
    }
};

int main(int argc, char *argv[])
{
    MyDriver driver;
    driver.Initialize();
    driver.Run();
}

#endif