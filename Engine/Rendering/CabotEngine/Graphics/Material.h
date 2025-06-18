#pragma once
#include <map>
#include <ranges>
#include <string>
#include <vector>

#include "ShaderObject.h"
#include "Texture2D.h"

class Material
{
public:
    Material();
    
    void SetShader(ShaderObject shaderObject);
    void SetTexture(const std::string& string , const std::shared_ptr<Texture2D>& tex);

    std::vector<float> Values();
    std::vector<std::shared_ptr<Texture2D>> Textures();
private:
    std::map<std::string,float> m_Values;
    std::map<std::string,std::shared_ptr<Texture2D>> m_Textures;
    //FIX:Color追加する
};
