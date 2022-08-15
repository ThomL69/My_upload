#pragma once

#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <vector>
#include <math.h>
#include "CharAnimViewer.h"

using namespace std;
using namespace chara;

class Array2D
{
public:
    Array2D() { m_dimX = m_dimY = 0; }
    void init(const int DIMX, const int DIMY) { m_dimX = DIMX; m_dimY = DIMY; m_data.resize(m_dimX * m_dimY); }
    void setAll(const float v) { m_data.assign(m_dimX * m_dimY, v); }
    const int dimX() const { return m_dimX; }
    const int dimY() const { return m_dimY; }

    float& operator()(const int x, const int y)
    {
        assert(x >= 0 && x < m_dimX && y >= 0 && y < m_dimY);
        return m_data[y * m_dimX + x];
    }

    float operator()(const int x, const int y) const
    {
        assert(x >= 0 && x < m_dimX&& y >= 0 && y < m_dimY);
        return m_data[y * m_dimX + x];
    }

    float interpolate(const float x, const float y)
    {
        if ((x < 0) || (x > m_dimX) || (y < 0) || (y > m_dimY)) return 0.f;
        int X = int(x);
        int Y = int(y);
        if ((X + 1 >= m_dimX) || (Y + 1 >= m_dimY)) return operator()(X, Y);
        float IX_b = (x - X) * operator()(X + 1, Y) + (X + 1 - x) * operator()(X, Y);
        float IX_h = (x - X) * operator()(X + 1, Y + 1) + (X + 1 - x) * operator()(X, Y + 1);
        return (y - Y) * IX_h + (Y + 1 - y) * IX_b;
    }

    int size() const { return m_data.size(); }

protected:
    vector<float> m_data; // OU float* m_data
    int m_dimX, m_dimY;
};


class ShallowWater
{
public:
    ShallowWater() {};
    //void init(const int DIMX, const int DIMY);
    //void draw() const;
    //void computeOneStep();
    int size() const { return m_n.size(); }

protected:
    Array2D m_g;     // height of the ground (0 if flat)
    Array2D m_h;     // height of the water : the thing to compute and to draw
    Array2D m_n;     // m_n = m_h - m_g : amount of water above the ground
    Array2D m_vX;    // velocity along X   
    Array2D m_vY;    // velocity along Y
    float dt;
    float dx = 2;
    float dy = 2;
    Mesh m_quad;
    DrawParam gl;

public:

    void init(const int DIMX, const int DIMY)
    {
        m_quad = Mesh(GL_TRIANGLES);
        m_quad.color(Color(1, 0, 1));
        
        m_g.init(DIMX, DIMY);
        m_h.init(DIMX, DIMY);
        m_n.init(DIMX, DIMY);
        m_vX.init(DIMX, DIMY);
        m_vY.init(DIMX, DIMY);

        m_n.setAll(1.0f);
        
        int i, j;
        for (i = 0; i < DIMX; i++) {
            for (j = 0; j < DIMY; j++) {
                
                m_quad.vertex(i, 0, j);  
            }
        }

        int x, y;
        for (x = 0; x < DIMX - 1; x++) {
            for (y = 0; y < DIMY - 1; y++) {
                int top = x + (y + 1) * DIMX;
                int current = x + y * DIMX;
                m_quad.triangle(current, top, top + 1);
                m_quad.triangle(current, top + 1, current + 1);
            }
        }
    }

    void render(DrawParam& gl, Orbiter& m_cam) {

        Point pmin, pmax;
        m_quad.bounds(pmin, pmax);
        //m_cam.lookat(pmin, pmax);

        gl.camera(m_cam);
        Transform t = Translation(Vector(0, 0, 0));
        draw(m_quad, t, m_cam);
    }

    void update(float dt)
    {
        int id = 0;
        int i, j;
        for (i = 0; i < m_h.dimX(); i++) {
            for (j = 0; j < m_h.dimY(); j++) {

                m_quad.vertex(id, i, m_h(i, j), j);
                id++;
            }
        }

        computeOneStep(dt);

        if (key_state('l')) {
            addPerturbation(0.5f, 0.5f, 5.0f);
        }
    }

    void computeOneStep(float dt)
    {
        Array2D vx_old = m_vX;
        Array2D vy_old = m_vY;

        advect(m_n, vx_old, vy_old, dt);
        advect(m_vX, vx_old, vy_old, dt);
        advect(m_vY, vy_old, vx_old, dt);

        update_height(dt);

        for (int x = 0; x < m_vX.dimX(); x++) {
            for (int y = 0; y < m_vY.dimY(); y++) {
                m_h(x, y) = m_n(x, y) + m_g(x, y);
            }
        }

        update_velocity(dt);

    }

    static void advect(Array2D& s, const Array2D& vx, Array2D& vy, float dt)
    {
        assert(&vx != &vy);
        assert(vx.size() == vy.size());

        Array2D s_old = s;
        float x, y;
        int i, j;

        for (i = 0; i < s.dimX(); i++) {
            for (j = 0; j < s.dimY(); j++) {
                
                x = i - dt * vx(i, j);
                y = j - dt * vy(i, j);

                s(i, j) = s_old.interpolate(x, y);
            }
        }
    }

    void update_height(float dt)
    {
        int i, j;
        float div_x, div_y, div_xy;
        for (i = 1; i < m_vX.dimX() - 1; i++) {
            for (j = 1; j < m_vY.dimY() - 1; j++) {
                
                div_x = (m_vX(i + 1, j) - m_vX(i, j)) / (2.f * dx);
                div_y = (m_vY(i, j + 1) - m_vY(i, j)) / (2.f * dy);

                div_xy = div_x + div_y;

                m_n(i, j) -= m_n(i, j) * dt * div_xy;
            }
        }
    }

    void update_velocity(float dt)
    {
        const float a = 9.81f;
        int i, j;
        float grad_x, grad_y;

        for (i = 1; i < m_vX.dimX() - 1; i++) {
            for (j = 1; j < m_vY.dimY() - 1; j++) {

                grad_x = (m_h(i - 1, j) - m_h(i, j)) / (2.f * dx);
                grad_y = (m_h(i, j - 1) - m_h(i, j)) / (2.f * dy);

                m_vX(i, j) = m_vX(i, j) + a * dt * grad_x;
                m_vY(i, j) = m_vY(i, j) + a * dt * grad_y;
            }
        }
    }

    void ShallowWater::addPerturbation(float x, float y, float power) {
        int p_x = (int)(x * m_vX.dimX());
        int p_y = (int)(y * m_vY.dimY());
        m_n(p_x, p_y) += power;
    }
};
