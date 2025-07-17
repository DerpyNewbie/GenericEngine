#pragma once
#include <LinearMath/btIDebugDraw.h>
#include <LinearMath/btVector3.h>

namespace engine
{
class BulletDebugDrawer : public btIDebugDraw
{
    int m_debug_mode_ = 0;

    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;
    void drawContactPoint(const btVector3 &point_on_b, const btVector3 &normal_on_b, btScalar distance, int life_time,
                          const btVector3 &color) override;
    void reportErrorWarning(const char *warning_string) override;
    void draw3dText(const btVector3 &location, const char *text_string) override;
    void setDebugMode(int debug_mode) override;
    int getDebugMode() const override;
};
}