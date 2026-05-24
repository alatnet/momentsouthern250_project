#pragma once
#ifndef MS250_FBX_H
#define MS250_FBX_H

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <cstddef>

#include "ufbx.h"

#include "MS250_Math.hpp"
#include "MS250_Structs.hpp"

using namespace MS250::Structs;

class MS250_FBX {
    public: //static static variables to handle internal states
        static bool debug;
        static float ANIMATION_FRAMERATE;
        static unsigned long MAX_ANIM_FRAMES;
        static unsigned long MAX_BONES;
        static unsigned long MAX_BLENDSHAPES;

    public: //constructors
        //File path load
        MS250_FBX(const char *filename, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);
        // MS250_FBX(const char *filename, size_t filename_len, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);

        // //Memory load
        // MS250_FBX(const void *data, size_t size, const ufbx_load_opts *opts= nullptr, ufbx_error *error = nullptr);

        // //stdio load
        // MS250_FBX(void *file_void, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);
        // MS250_FBX(void *file_void, const void *prefix, size_t prefix_size, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);

        // //stream load
        // MS250_FBX(const ufbx_stream *stream, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);
        // MS250_FBX(const ufbx_stream *stream, const void *prefix, size_t prefix_size, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);

    public: //deconstructors
        ~MS250_FBX();

    public: //update functions
        void Update(float deltaTime);
        void UpdateAnimation(size_t animID, float time);
        void UpdateHierarchy();
        void UpdateBones();
        
    public: //animation functions
        void SetAnimation(size_t animID, bool updateHierarchy = true);

    public: //get functions
        Node* getNode(size_t id){ return &this->_nodes[id]; }
        size_t numNodes(){ return this->_numNodes; }

        Mesh* getMesh(size_t id){ return &this->_meshes[id]; }
        size_t numMeshes(){ return this->_numMeshes; }

    public: //misc functions
        bool IsLoaded();

        virtual std::string toString(std::string padding = "-");

    private: //loading functions
        void _LoadData();
        void _LoadNodes(Node& node, ufbx_node* fbxNode);
        void _LoadMesh(Mesh& mesh, ufbx_mesh* fbxMesh);
        void _LoadBlendShapes();
        void _LoadAnim(Animation& anim, ufbx_anim_stack * animStack, ufbx_scene * scene);
        void _LoadAnimNode(Animation& anim, AnimNode& animNode, ufbx_anim_stack * animStack, ufbx_node * node);
        void _LoadAnimBlendChannel(Animation& anim, AnimBlendChannel& animBlendChannel, ufbx_anim_stack * animStack, ufbx_blend_channel * blendChannel);

    private: //unloading functions
        void _UnloadData();
        void _UnloadMesh(Mesh& mesh);
        void _UnloadPart(Part& part);
        void _UnloadAnim(Animation& anim);

    protected: //logging functions
        template<typename... Args>
        void Log(Args&&... args);

        template<typename... Args>
        void LogNewLine(Args&&... args);
        
    private: //static misc functions
        static MS250::Math::Matrix convertMatrix4x3to4x4(ufbx_matrix fbxMatrix);
        static MS250::Math::Matrix convertNormalMatrix(ufbx_matrix *fbxMatrix);
        static float lerp(float a, float b, float time) { return a*(1.0f-time) + b*time; }

    private:
        ufbx_scene* _fbx_scene = nullptr;
        bool _isLoaded = false;

    protected:
        Node* _nodes = nullptr; //array of nodes
        Mesh* _meshes = nullptr; //array of meshes (meshes references nodes for instancing by applying a node's matrix to it.)

        size_t _numNodes = 0;
        size_t _numMeshes = 0;

        size_t numBlendChannelWeights = 0;
        float * blendChannelWeights;

        size_t numAnimations = 0;
        Animation * _animations;

        //animation handling
        size_t currAnimID = -1;
        float currAnimTime = 0;

    private: //limits
        float animation_framerate;
        unsigned long max_anim_frames;
        unsigned long max_bones;
        unsigned long max_blendshapes;
};

#endif