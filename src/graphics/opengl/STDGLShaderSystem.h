#pragma once

#include <glad/glad.h>
#include "engine/filesystem/Filesystem.h"
#include <map>
#include "engine/filesystem/ADF.h"

class STDGLShaderSystem {
    struct ComputeShader {
        GLuint ProgramObject;
        std::string SourcePath;
        ComputeShader(GLuint prg, std::string& path) { ProgramObject = prg; SourcePath = path; }
    };
    std::map<std::string, ComputeShader> ComputeShaders;

    std::map<std::string, uint16_t> VertexShaderNameToIndex;
    std::map<std::string, uint16_t> FragmentShaderNameToIndex;

    struct Shader {
        GLuint ShaderObject;
        std::string SourcePath;
        Shader(GLuint shad, std::string& path) { ShaderObject = shad; SourcePath = path; }
    };
    std::vector<Shader> VertexShaders;
    std::vector<Shader> FragmentShaders;
    std::vector<Shader> DepthShaders; // Uses the same indices as fragment shaders

    struct ShaderPipelineKey {
        union {
            struct {
                uint16_t VertexShaderID;
                uint16_t FragmentShaderID;
                bool IsDepth;
            };
            uint64_t KeyAsInt = 0;
        };
        ShaderPipelineKey(uint16_t vert, uint16_t frag, bool isdepth) { VertexShaderID = vert; FragmentShaderID = frag; IsDepth = isdepth; }
    };
    std::map<uint64_t, GLuint> CachedShaderPipelines;


    void InitCompute(const ADFEntry& ShaderDefs);
    void InitGraphic(const ADFEntry& ShaderDefs);
public:
    void Init();

    inline GLuint GetComputeShader(std::string name) {
        return ComputeShaders.at(name).ProgramObject;
    }
    // The first element of the pair is the normal version of the shader pipeline, while the second element is the depth-only version.
    std::pair<GLuint, GLuint> GetShaderPipeline(const std::string& VertexName, const std::string& FragmentName);
};