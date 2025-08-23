#include "pch.h"

#include "bullet_debug_drawer.h"
#include "Rendering/gizmos.h"

namespace engine
{
namespace
{
Vector3 ToVector3(const btVector3 vec)
{
    return {vec.x(), vec.y(), vec.z()};
}

Color ToColor(const btVector3 color)
{
    return {color.x(), color.y(), color.z()};
}
}

void BulletDebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    Gizmos::DrawLine(ToVector3(from), ToVector3(to), ToColor(color));
}
void BulletDebugDrawer::drawContactPoint(const btVector3 &point_on_b, const btVector3 &normal_on_b, btScalar distance,
                                         int life_time, const btVector3 &color)
{
    const auto c = ToColor(color);
    const auto p = ToVector3(point_on_b);
    const auto n = ToVector3(normal_on_b);

    Gizmos::DrawSphere(p, 0.05f, c);
    Gizmos::DrawLine(p, p + n, c);
}
void BulletDebugDrawer::reportErrorWarning(const char *warning_string)
{
    Logger::Error(warning_string);
}
void BulletDebugDrawer::draw3dText(const btVector3 &location, const char *text_string)
{
    // NOTE(derpy): unable to implement atm
}
void BulletDebugDrawer::setDebugMode(const int debug_mode)
{
    m_debug_mode_ = debug_mode;
}
int BulletDebugDrawer::getDebugMode() const
{
    return m_debug_mode_;
}
}