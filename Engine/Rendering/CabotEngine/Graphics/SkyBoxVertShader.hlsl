cbuffer CameraBuffer : register(b0)
{
    float4x4 viewProj;
}

struct VSInput
{
    float3 pos : POSITION; // ���_���W
    float4 color : COLOR; // ���_�F
    float3 normal : NORMAL; // �@��
    float4 tangent : TANGENT; // �ڋ��
    float2 uv1 : TEXCOOD0; // UV
    float2 uv2 : TEXCOOD1;
    float2 uv3 : TEXCOOD2;
    float2 uv4 : TEXCOOD3;
    float2 uv5 : TEXCOOD4;
    float2 uv6 : TEXCOOD5;
    float2 uv7 : TEXCOOD6;
    float2 uv8 : TEXCOOD7;
    uint bones_per_vertex : BONESPERVERTEX;
    uint4 bone_id : BONEINDEX;
    float4 bone_weight : BONEWEIGHT;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 dir : TEXCOORD;
};

VSOutput vert(VSInput input)
{
    VSOutput output;

    // �J�����ʒu�͖����i��]�̂݁j
    float4 worldPos = float4(input.pos, 1.0f);
    float4 clipPos = mul(viewProj, worldPos);
    clipPos.z = clipPos.w; // z=1.0�ɌŒ�
    output.pos = clipPos;

    output.dir = input.pos; // �J���������x�N�g��
    return output;
}