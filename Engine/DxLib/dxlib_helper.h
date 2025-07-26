// SPDX-License-Identifier: MIT-0

#pragma once
#include "DxLib.h"

// ReSharper disable once CppInconsistentNaming
namespace DxLibHelper
{
constexpr float kDefaultBillboardSize = 0.25F;
constexpr float kDefaultAxisLength = 10.0F;

/**
 * 3D空間に文字列を描画します。
 *
 * 文字列は指定された位置にビルボード形式で表示されます。
 *
 * @param pos 文字列を表示する3D座標 
 * @param str 表示する文字列
 * @param color 文字色(0xRRGGBB形式)
 * @param edge_color エッジの色(0xAARRGGBB形式)。デフォルトは0(エッジなし)
 * @param size 描画するビルボードのサイズ。
 */
static void DrawString3D(const VECTOR &pos, const char *str,
                         const unsigned int color, const unsigned int edge_color = 0, const float size = kDefaultBillboardSize) // NOLINT(clang-diagnostic-unused-function)
{
    int size_x, size_y, line_count;
    GetDrawStringSize(&size_x, &size_y, &line_count, str, static_cast<int>(strlen(str)));
    const int draw_screen = MakeScreen(size_x, size_y, TRUE);
    {
        const auto prev_draw_screen_setting = GetUseSetDrawScreenSettingReset();
        SetUseSetDrawScreenSettingReset(FALSE);
        {
            SetDrawScreen(draw_screen);
            DrawString(0, 0, str, color, edge_color);
            SetDrawScreen(DX_SCREEN_BACK);
        }
        SetUseSetDrawScreenSettingReset(prev_draw_screen_setting);
    }
    const auto billboard_size = static_cast<float>(size_x) * size;
    DrawBillboard3D(pos, 0.5F, 0.5f, billboard_size, 0, draw_screen, TRUE);
    DeleteGraph(draw_screen);
}

/**
 * 3D空間に文字列をフォントハンドルを使用して描画します。
 *
 * 文字列は指定された位置にビルボード形式で表示されます。
 * 指定されたフォントハンドルを使用して文字列を描画します。
 *
 * @param pos 文字列を表示する3D座標 
 * @param str 表示する文字列
 * @param color 文字色(0xRRGGBB形式)
 * @param font_handle 使用するフォントのハンドル
 * @param edge_color エッジの色(0xAARRGGBB形式)。デフォルトは0(エッジなし)
 * @param size 描画するビルボードのサイズ。
 */
static void DrawString3DToHandle(const VECTOR &pos, const char *str, // NOLINT(clang-diagnostic-unused-function)
                                 const unsigned int color, const int font_handle,
                                 const unsigned int edge_color = 0, const float size = kDefaultBillboardSize)
{
    int size_x, size_y, line_count;
    GetDrawStringSizeToHandle(&size_x, &size_y, &line_count, str, static_cast<int>(strlen(str)), font_handle);
    const int draw_screen = MakeScreen(size_x, size_y, TRUE);
    {
        const auto prev_draw_screen_setting = GetUseSetDrawScreenSettingReset();
        SetUseSetDrawScreenSettingReset(FALSE);
        {
            SetDrawScreen(draw_screen);
            DrawStringToHandle(0, 0, str, color, font_handle, edge_color);
            SetDrawScreen(DX_SCREEN_BACK);
        }
        SetUseSetDrawScreenSettingReset(prev_draw_screen_setting);
    }

    const auto billboard_size = static_cast<float>(size_x) * size;
    DrawBillboard3D(pos, 0.5F, 0.5f, billboard_size, 0, draw_screen, TRUE);
    DeleteGraph(draw_screen);
}

/**
 * 変換行列からローカル座標軸を取得します。
 *
 * 指定された変換行列から、そのローカル座標系におけるX軸、Y軸、Z軸の
 * 方向ベクトルを計算します。各軸ベクトルは正規化されています。
 *
 * @param matrix オブジェクトの変換行列
 * @param out_left 計算されたローカルX軸方向が格納されます
 * @param out_up 計算されたローカルY軸方向が格納されます
 * @param out_forward 計算されたローカルZ軸方向が格納されます
 */
static void GetLocalAxis(const MATRIX &matrix, VECTOR &out_left, VECTOR &out_up, VECTOR &out_forward) // NOLINT(clang-diagnostic-unused-function)
{
    const MATRIX rot = MGetRotElem(matrix);
    out_left = VNorm(VTransform({1, 0, 0}, rot));
    out_up = VNorm(VTransform({0, 1, 0}, rot));
    out_forward = VNorm(VTransform({0, 0, 1}, rot));
}


/**
 * 3D空間に座標軸を描画します。
 * 
 * X軸は赤、Y軸は緑、Z軸は青で表示されます。
 * 原点から各軸方向に指定された長さの線が描画されます。
 *  
 * @param m 座標軸の変換行列(位置、回転、スケール)
 * @param len 座標軸の長さ
 */
static void DrawAxis3D(const MATRIX &m, const float len = kDefaultAxisLength)
{
    const auto origin = VTransform({0, 0, 0}, m);
    DrawSphere3D(origin, len * 0.1F, 8, 0xFFFFFFFF, 0xFFFFFFFF, false);
    DrawLine3D(origin, VTransform({len, 0, 0}, m), GetColor(255, 0, 0));
    DrawLine3D(origin, VTransform({0, len, 0}, m), GetColor(0, 255, 0));
    DrawLine3D(origin, VTransform({0, 0, len}, m), GetColor(0, 0, 255));
}

/**
 * 水平方向のグリッドを描画します。
 *
 * 指定された間隔とグリッド数に基づいて、3D空間のY平面上にグリッドを描画します。
 * グリッド線の色は指定された法則で描画され、主要な線は異なる色で強調表示されます。
 * また、主要な交点においては垂直線も描画されます。
 *
 * @param spacing グリッドの間隔を指定する2Dベクトル(u方向の間隔, v方向の間隔)。デフォルト値は(100, 100)
 * @param count 描画するグリッドの全体の個数。デフォルト値は50
 */
static void DrawYPlaneGrid(const FLOAT2 spacing = {50, 50}, const int count = 50) // NOLINT(clang-diagnostic-unused-function)
{
    SetUseZBuffer3D(TRUE);

    const MATRIX camera_view_matrix = MInverse(GetCameraViewMatrix());
    const auto [cam_x, cam_y, cam_z] = VTransform({0, 0, 0}, camera_view_matrix);
    const int half_count = count / 2;
    const int center_x_index = static_cast<int>(cam_x / spacing.u);
    const int center_z_index = static_cast<int>(cam_z / spacing.v);
    const int max_x = center_x_index + half_count;
    const int min_x = center_x_index - half_count;
    const int max_z = center_z_index + half_count;
    const int min_z = center_z_index - half_count;

    const float x_end = spacing.u * static_cast<float>(min_x);
    const float z_end = spacing.v * static_cast<float>(min_z);

    for (int z = min_z; z < max_z; ++z)
    {
        for (int x = min_x; x < max_x; ++x)
        {
            const auto dx = spacing.u * static_cast<float>(x);
            const auto dz = spacing.v * static_cast<float>(z);

            const VECTOR z_line_begin = {dx, 0, dz};
            const VECTOR z_line_end = {dx, 0, z_end};

            const VECTOR x_line_begin = {dx, 0, dz};
            const VECTOR x_line_end = {x_end, 0, dz};

            DrawLine3D(z_line_begin, z_line_end, x % 5 == 0 ? 0xffa1a1ff : 0x10A1A1A1);
            DrawLine3D(x_line_begin, x_line_end, z % 5 == 0 ? 0xffffa1a1 : 0x10A1A1A1);

            if (x % 5 == 0 && z % 5 == 0)
            {
                DrawLine3D({dx, 0, dz}, {dx, cam_y, dz}, 0xffa1ffa1);
            }
        }
    }
}
/**
 * オブジェクトの名前と座標軸を3D空間に描画します。
 *
 * オブジェクトの名前は原点付近に表示され、座標軸は与えられた
 * 変換行列に基づいて描画されます。
 *
 * @param object_name 表示するオブジェクトの名前。TCHARで表現される
 *                    NULL終端文字列である必要があります。
 * @param object_matrix オブジェクトのローカル変換(位置、回転、拡大縮小)を
 *                      表す3D空間上の変換行列。
 * @param name_size フレームに表示される名前のサイズ
 * @param axis_len フレームに表示される軸のサイズ
 */
static void DrawObjectInfo(const TCHAR *object_name, const MATRIX &object_matrix, const float name_size = kDefaultBillboardSize, const float axis_len = kDefaultAxisLength) // NOLINT(clang-diagnostic-unused-function)
{
    const auto pos = VTransform({0, 0, 0}, object_matrix);
    DrawString3D(VSub(pos, {0, 1, 0}), object_name, GetColor(255, 255, 255), 0, name_size);
    DrawAxis3D(object_matrix, axis_len);
}

/**
 * 3Dモデルのフレームと階層関係を描画します。
 *
 * このメソッドはモデルと各フレーム(子フレームを含む)の名前と座標軸を表示します。 
 * 親フレームと子フレーム間に線を引いて階層関係を可視化します。
 * 座標情報はモデルハンドルを使用して取得されます。
 *
 * @param model_handle 描画するフレームを持つ3Dモデルのハンドル
 * @param model_name モデルの原点に表示する名前。デフォルトは"Model Origin"
 * @param name_size フレームに表示される名前のサイズ
 * @param axis_len フレームに表示される軸のサイズ
 * 
 */
static void DrawModelFrames(const int model_handle, const TCHAR *model_name = "Model Origin", const float name_size = kDefaultBillboardSize, const float axis_len = kDefaultAxisLength) // NOLINT(clang-diagnostic-unused-function)
{
    DrawObjectInfo(model_name, MV1GetMatrix(model_handle), name_size, axis_len);

    const int frame_num = MV1GetFrameNum(model_handle);
    for (int frame_index = 0; frame_index < frame_num; ++frame_index)
    {
        MATRIX frame_mat = MV1GetFrameLocalWorldMatrix(model_handle, frame_index);
        const TCHAR *frame_name = MV1GetFrameName(model_handle, frame_index);
        DrawObjectInfo(frame_name, frame_mat, name_size, axis_len);

        const int child_frame_num = MV1GetFrameChildNum(model_handle, frame_index);
        for (int j = 0; j < child_frame_num; ++j)
        {
            const int child_frame_index = MV1GetFrameChild(model_handle, frame_index, j);
            MATRIX child_frame_mat = MV1GetFrameLocalWorldMatrix(model_handle, child_frame_index);
            DrawLine3D(MGetTranslateElem(child_frame_mat), MGetTranslateElem(frame_mat), 0xFFFFFFFF);
        }
    }
}
}