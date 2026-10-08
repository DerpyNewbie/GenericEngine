#pragma once

namespace engine
{
class Component;

/// <summary>
/// 型名の取得や、Vector / Colorからfloat配列への変換などの便利関数をまとめたクラスです。
/// </summary>
class EngineUtil
{
public:
    /// <summary>
    /// typeidの名前からnamespaceなどを取り除いた型名を取得します。
    /// </summary>
    /// <param name="typeid_name">typeid(T).name()の値</param>
    static std::string GetTypeName(const char *typeid_name);
    /// <summary>
    /// Componentの実際の型の型名を取得します。
    /// </summary>
    /// <param name="component">対象のComponent</param>
    static std::string GetTypeName(Component *component);
    /// <summary>
    /// Componentのポインタのtypeidの名前から型名を取得します。
    /// </summary>
    /// <param name="component">対象のComponent</param>
    static std::string GetTypeName(const std::shared_ptr<Component> &component);
    /// <summary>
    /// Vector2の各要素をfloatの配列に書き込みます。
    /// </summary>
    /// <param name="buff">書き込み先の配列</param>
    /// <param name="vec">書き込むVector2</param>
    static void ToFloat2(float buff[2], Vector2 vec);
    /// <summary>
    /// Vector3の各要素をfloatの配列に書き込みます。
    /// </summary>
    /// <param name="buff">書き込み先の配列</param>
    /// <param name="vec">書き込むVector3</param>
    static void ToFloat3(float buff[3], Vector3 vec);
    /// <summary>
    /// Colorの各要素をfloatの配列に書き込みます。
    /// </summary>
    /// <param name="buff">書き込み先の配列</param>
    /// <param name="vec">書き込むColor</param>
    static void ToFloat4(float buff[4], Color vec);
};
}