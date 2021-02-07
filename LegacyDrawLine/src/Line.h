#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <chrono>

#include <GLFW/glfw3.h>

class Line
{
private:
    //int m_pStart[2], m_pFinal[2]; // points coordinate
    //int m_width, m_height;

    //unsigned int m_pattern;
    //int m_line_width;
    //float m_line_color[3];
public:
    Line(const int _pStart[2], const int _pFinal[2], const int width, const int height, const unsigned int _pattern, const int _line_width, const float _line_color[3]);
    ~Line();

    void Set(float _x, float _y, float _z); // Primitive

    void convertToNDC(int _x, int _y, float* r_x, float* r_y);

    void createPoints();
    void SampleDDA();

    // only use these for transferring it into json!
    int m_pStart[2], m_pFinal[2]; // points coordinate
    int m_width, m_height;

    unsigned int m_pattern;
    int m_line_width;
    float m_line_color[3];
};