#include "pch.h"

#include "PrimitiveShape.h"
#include "../../Graphics/DescriptorHeapManager.h"
#include "../../Graphics/PSOManager.h"
#include "application.h"
#include <directxtk12/GeometricPrimitive.h>
using namespace DirectX;

PrimitiveShape::PrimitiveShape(ShapeType type)
{

    CreateMesh(type);

    auto eyePos = XMVectorSet(0.0f, 120.0, 275.0, 0.0f);
    auto targetPos = XMVectorSet(0.0f, 120.0, 0.0, 0.0f);
    auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    auto fov = XMConvertToRadians(60);
    auto aspect = static_cast<float>(Application::WindowWidth()) / static_cast<float>(
                      Application::WindowHeight()); // アスペクト比

    for (size_t i = 0; i < RenderEngine::FRAME_BUFFER_COUNT; i++)
    {
        m_WVPBuffer[i] = std::make_shared<ConstantBuffer>(sizeof(Transform));

        // 変換行列の登録
        auto ptr = m_WVPBuffer[i]->GetPtr<Transform>();
        ptr->World = XMMatrixIdentity() * XMMatrixRotationY(XMConvertToRadians(90)) * XMMatrixTranslation(
                         0.0f, 100.0f, 0.0f);
        ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
        ptr->Proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.0f);
    }

    auto handle = g_DescriptorHeapManager->Get().Register(Texture2D::GetWhite());
    m_MaterialHandle = handle;
}

void PrimitiveShape::Draw()
{}

void PrimitiveShape::CreateMesh(ShapeType type)
{
    GeometricPrimitive::VertexCollection vertices;
    GeometricPrimitive::IndexCollection indices;
    switch (type)
    {
    case ShapeType::Box:
        GeometricPrimitive::CreateBox(vertices, indices, XMFLOAT3(1.0f, 1.0f, 1.0f));
        break;
    case ShapeType::Cone:
        GeometricPrimitive::CreateCone(vertices, indices);
        break;
    case ShapeType::Cube:
        GeometricPrimitive::CreateCube(vertices, indices);
        break;
    case ShapeType::Cylinder:
        GeometricPrimitive::CreateCylinder(vertices, indices);
        break;
    case ShapeType::Dodecahedron:
        GeometricPrimitive::CreateDodecahedron(vertices, indices);
        break;
    case ShapeType::GeoSphere:
        GeometricPrimitive::CreateGeoSphere(vertices, indices);
        break;
    case ShapeType::IcosaHedron:
        GeometricPrimitive::CreateIcosahedron(vertices, indices);
        break;
    case ShapeType::Octahedron:
        GeometricPrimitive::CreateOctahedron(vertices, indices);
        break;
    case ShapeType::Sphere:
        GeometricPrimitive::CreateSphere(vertices, indices);
        break;
    case ShapeType::Teapot:
        GeometricPrimitive::CreateTeapot(vertices, indices);
        break;
    case ShapeType::Tetrahedron:
        GeometricPrimitive::CreateTetrahedron(vertices, indices);
        break;
    case ShapeType::Torus:
        GeometricPrimitive::CreateTorus(vertices, indices);
        break;
    default:
        break;
    }

}