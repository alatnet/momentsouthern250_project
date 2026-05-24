May 8  
MomentSouthern250 - 3:44 AM  
hey, i've just read your post on r/gamedevclassified, i am solo deving (very unprofessionally) and could use some (paid) help, Are you open for some smaller tasks? It would be in c++ with mostly c features. Right now i would need to port some 3D animation structs to use on gpu etc. regards, MomentSouthern250

alatnet - 7:07 AM  
I'm open to doing that. What kind of things are needed?  
I should note that my current timezone is JST.

May 9  
alatnet - 11:32 AM  
yea, sure. im open to doing something like that. what kind of tasks are we looking at?  
i should note that my timezone is currently JST.

May 10  
MomentSouthern250 - 5:48 AM  
hey great, i don't care much about the timezone, right now i am basically using this https://github.com/ufbx/ufbx/blob/master/examples/viewer/viewer.c to render my model, i need an "animation stack" where i tell the model which animation it should play (no animation blending, you can take a look at update_animation function, also probably at the ufxb library for how the model is set up), and the other thing is i need to extract the vertices, normals and trias into a flat array( and the bones and boneweights) so i am able to run the animations on that data (the trias in the fbx are split into materials, i think). I am using the extracted for collision detection. This is the short overview if you get what i'm trying to do and if you are still interested, i think you can take a look at the viewer.c and think about how long/how much you think that would be. Thanks.
![Images/2ti9bj9ib60h1.png]()

alatnet - 12:06 PM  
huh, seems similar to what i was doing in unity with the descent 1 and 2 polymodels.  
animation seems similar to what i was doing with cubism 3 for lumberyard.  
i cant give a concrete timeline, but i'd say about 1 week minimum.  

alatnet - 12:13 PM  
for the flat array, what kind of format are you looking for?

MomentSouthern250 - 4:50 PM  
just an array of floats, i have a basic struct that would need to be extended with the bone weights and matrices

    typedef struct Sim_Mesh{  
        float* vertices;  
        size_t num_vertices;  
      
        uint16_t* indeces;  
        size_t num_indeces;  
      
    }Sim_Mesh;

and i would need a function that fills the sim_mesh with the given fbx data 

May 14  
alatnet - 8:25 PM
just to give you an update, i've gotten to the point where im able to get vertex information (position, normals, and uv's) into semi flat arrays (mainly structs for readability) along with indices, bone index's and weights, and separate node matrix information. currently working on getting my testing app to render the data via raylib to make sure that the data loaded is correct.  

alatnet - 8:36 PM  
I am making this as a c++ class and will be using a few standard libraries for it.

MomentSouthern250 - 9:39 PM  
that is great, but we havent talked price, if i can't afford you, you'd have done that for nothing.  

alatnet - 9:44 PM  
i'd say, at the moment, i've been putting in about 2 to 4 hours a day  

MomentSouthern250 - 9:46 PM  
and what is your hourly rate?  
and how do i pay you? paypal?  

alatnet - 9:47 PM  
we can negotiate, but for hourly rate, about $20.  
and paypal is fine  
so, current total time range of what I have done would be 10 to 20 hours total  
so current expected price at $20 would be around $200 to $400.  
for the 5 days at 2 to 4 hours each day  

MomentSouthern250 - 9:52 PM  
ok  

MomentSouthern250 - 10:27 PM  
for testing i've used this model  
https://thatnewdevgirl.itch.io/free-3d-anime-girl-theresa  
i just removed all cloth and hair

alatnet - 10:36 PM  
Ah, for me. I've been going simple and using two cubes with two bones on it, each cube on its own bone.  
I do have a few other models that I can use for testing on my end.  

MomentSouthern250 - 10:37 PM  
probably smarter, but was mainly as information that i will be testing it on this one.

alatnet - 10:37 PM  
K. I'll run that one through the testing too.  

May 22  
alatnet - 6:01 PM  
ok, update, i've been working with trying to validate the data in the flat arrays and was, more or less, banging my head against the wall with opengl. i have been able to get the flat array of positions, normals, and uv's (though i havent tested that) working. been working on validating the bone data but that's taking some time...  
![Images/whvza4azln2h1.png]()  
![Images/izla1yh6mn2h1.png]()  
first image is without bones, second is with bones  
should be two cubes with one smaller and floating above the other  
mostly just figuring out how to work with opengl in using the data. which would most likely be useful for you to understand how to use it with what program you are going to integrate it with  

MomentSouthern250 - 7:25 PM  
so do you use a standard shader for the bones? The layouting for the shader input is always a pain in the ass. The drawing of bone data is already working for me, i just need to redo it in opencl and for that i need/want the flat vertices and bone data. If you say you have it in a workable state i can try to use it

alatnet - 7:47 PM
this is the vertex shader that im using:  

    #version 460 core

    //#define MAX_BLEND_SHAPES 64
    #define MAX_BONES 64

    layout(location = 0) in vec3 vPos;
    layout(location = 1) in vec3 vNorm;
    layout(location = 2) in vec2 vUV;

    layout(location = 3) in ivec4 vBoneIndex;
    layout(location = 4) in vec4 vBoneWeight;

    layout (std430, binding = 0) readonly buffer bones_ssbo {
        mat4 bones[];
    };

    uniform vec3 vCol;
    uniform mat4 mvp;
    uniform mat4 geom2world;
    uniform mat4 norm2world;

    uniform bool isSkinned;

    out vec3 color;
    out vec3 fNorm;
    out vec2 fUV;

    void main(){
    color = vCol;

    vec3 local_pos = vPos;
        //local_pos += evaluate_blend_shape(int(a_vertex_index));
        vec3 world_pos = vec3(0.0f);
        vec3 world_normal = vec3(0.0f);

        if (isSkinned){
            //do bone calcs here
            mat4 bind_to_world
                = bones[int(vBoneIndex.x)] * vBoneWeight.x
                + bones[int(vBoneIndex.y)] * vBoneWeight.y
                + bones[int(vBoneIndex.z)] * vBoneWeight.z
                + bones[int(vBoneIndex.w)] * vBoneWeight.w;
            world_pos = (bind_to_world * vec4(local_pos, 1.0f)).xyz;
            world_normal = (bind_to_world * vec4(vNorm, 0.0f)).xyz;
        } else {
            world_pos = (geom2world * vec4(vPos, 1.0f)).xyz;
            world_normal = (norm2world * vec4(vNorm, 0.0f)).xyz;
        }

        gl_Position = mvp * vec4(world_pos, 1.0f);
        fNorm = normalize(world_normal);
        fUV = vUV;
    }  
![Images/d9okdoe4av2h1.png]()  
dont know why it's limiting the message...  
anyway  
this is how im updating the bones into the flat array:  
![Images/kbf1q6maav2h1.png]()  
and this is how im updating the ssbo in the shader with the bone meshes in the flat array:  
![Images/8lr31lteav2h1.png]()  
for the drawing that im doing, im enabling the shader, updating the model view perspective matrix (mvp uniform), then for each mesh, updating the bone buffer, color (which can probable be material in the future), and if the model is skinned, then for each node instance im updating the geom2world matrix and norm2world matrix, then finally, for each part, im binding the VAO buffer (this has the vertexes, normals, uvs, and indices in it) and drawing it with the glDrawElements function to draw each part with their indicies.  
with the a test model on hand, it gives me this:  
![Images/6d2i0mj9bv2h1.png]()  
disabling bones gives me this:  
![Images/xxxalrqebv2h1.png]()  
which is what it should look like  
ah... that probably wasnt a good pic of the vertex shader...  
![Images/httr7zenbv2h1.png]()  
wait... what?  
![Images/bg9ky6c5cv2h1.png]()  
ok.... so... it's rendering the test cubes correctly with the bones disabled...  
hmmm, looking at the data via renderdoc with the bone matricies. the data looks to be correct by all accounts.  

MomentSouthern250 - 8:32 PM  
i remember i had a big problem with the indecis of the bones, that they didnt get them as int but as floats or something, the shader i used also has this

    #if SOKOL_GLSL
        layout(location=4) in vec4 a_bone_indices;
    #else
        layout(location=4) in ivec4 a_bone_indices;
    #endif

so i think the opengl gets the indices as floats and are converted to ints again. Could be why the head looks ok, but the rest is "undefined"  

alatnet - 8:38 PM  
So, I came across that too.  
I was able to, more or less, fix that by defining vBoneIndex as GL_UNSIGNED_INT and vBoneWeight as GL_FLOAT.  
And instead of using uint8_t for bone indexes and weights, I just used unsigned int and float for them. Don't need to bother with the weird quantization thing.  
Data in the buffers are correct in renderdoc though.  
For vBoneIndex, I'm using ivec4 instead of vec4.  
Which should make it an integer  
Thinking that I should move all matrix stuff into the shader since it looks to be per node.  
So, node to parent n such.  

MomentSouthern250 - 9:02 PM  
i... wouldn't XD, evey vertex on the gpu would need to get the all bone matrices and recalculate them. Problem wouldnt even be the calculations but the size of the memory each vertex needs. maybe it's a 64wide integer on cpu and 32wide or crap like that.  

alatnet - 9:04 PM  
Hmmm, true. While it would be mostly multiplying two matrices then multiplying the vector, it might bog down the GPU...  

Today  
MomentSouthern250 - 12:04 AM  
that's what it's already doing (multiplying 4 matrices), as i understand it, it is mostly the problem of random access (each thread on the gpu needs the full bone structure), but anyway, how far are you with making it a flat structure out of fbx data? than i can take a look and try to incorporate that into my setup (and pay you for that part)  

alatnet - 9:33 AM  
which is wierd, i do have the bone indexes and weights stored in the vao along with the bone matricies stored in an ssbo. did find that apparently the index for the smaller cube is not getting the right index for some reason. probably some wierd bug where it's not showing the data as an integer but as a float (since i did see that it was 1.4e in renderdoc before converting it to an integer).  
i do have everything as a flat structure. vertex positions, normals, uvs, bone indexes, bone weights, and bone matricies.  
i do have an animation setup  
though the animation blend channels are structured atm  
dont have it as a flat array  
i want to say that for this, about $400 seems appropriate.  
i have included a rather unoptimized math library in it but it can be easily swapped out for whatever math library you are using. i have glm working with it.  

MomentSouthern250 - 12:28 PM  
ok, half now, you se  
nd it, and half later?  

alatnet - 12:29 PM  
sounds good  
paypal account is "[redacted]"  
do you mind if i put it on my github and add in this conversation for record keeping?  

MomentSouthern250 - 12:33 PM
yeah, sure. I send the money  
sent  

alatnet - 12:36 PM  
here's a packaged zip version of it, i'll get it uploaded to github in a moment: [redacted]  
didnt know of any good file sharing sites atm...  
do you have any objections of using your username for information about the project and/or title for the github repo?  

MomentSouthern250 - 12:48 PM  
no go nuts XD. I would have said email, but i got the file.  
i sent the other half  

alatnet - 12:48 PM  
thx. it's mainly so that I can add it to my coding porfolio.  
if you have any trouble, feel free to contact me  
hopefully the GL_FBX files will help out in how to use it  

MomentSouthern250 - 12:49 PM  
yeah, i probably will, but i think i get what you are doing so far. Thanks  

alatnet - 12:50 PM  
np. pleasure to do business with you.  