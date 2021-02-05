#include "Line.h"

Line::Line(int _pStart[2], int _pFinal[2], int _width, int _height)
{
    m_width = _width;
    m_height = _height;
    m_pStart[0] = _pStart[0];
    m_pStart[1] = _pStart[1];
    m_pFinal[0] = _pFinal[0];
    m_pFinal[1] = _pFinal[1];
}

Line::~Line()
{
}

void Line::convertToNDC(int _x, int _y, float* r_x, float* r_y)
{
    // convert into opengl Normalized Device Coordinates (NDC) range (-1 to 1)
    *r_x = 2.0f * _x / m_width - 1.0f;
    *r_y = 2.0f * _y / m_height - 1.0f;
}


// This code is very redundant, there's a lot of code duplication, it's useful to get a 
// solid understanding of how the algorithm works, but be sure to fix this spaghetti code
// before shipping it
std::vector<float> Line::createPoints(const unsigned int _pattern, const int _lineWidth)
{
    unsigned int curr_pattern;
    std::vector<float> points = {};

    int counter = 0;
    curr_pattern = _pattern;

    if (m_pStart[0] == m_pFinal[0] && m_pStart[1] == m_pFinal[1]) // a single point
    {
        float ndc_x, ndc_y;
        this->convertToNDC(m_pStart[0], m_pStart[1], &ndc_x, &ndc_y);
        points = { ndc_x, ndc_y, 0.0f };
        glVertex3f(ndc_x, ndc_y, 0.0f);
        //std::cout << "case 0 (point)" << std::endl;
    }
    else if (m_pStart[1] == m_pFinal[1]) // horizontal line
    {
        int yvalue = m_pStart[1];
        int len = m_pStart[0] - m_pFinal[0];
        int counter = 0;
        curr_pattern = _pattern;
        if (len < 0) // "rightward"
        {
            //std::cout << "case 1 (horizontal right)" << std::endl;
            int x = m_pStart[0];
            for (int i = 0; x < m_pFinal[0]; i++)
            {
                float ndc_x, ndc_y, ndc_z;
                ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                for (int j = 0; j < _lineWidth; j++)
                {
                    this->convertToNDC(x, yvalue + j, &ndc_x, &ndc_y);
                    std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                    points.insert(points.end(), point.begin(), point.end());
                    glVertex2f(ndc_x, ndc_y);
                }
                x++;

                if (counter == 23)
                {
                    counter = 0;
                    curr_pattern = _pattern;
                }
                else
                {
                    counter++;
                    // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                    // that was not a very technical description for a right bit shift of 1.
                    curr_pattern = curr_pattern >> 1;
                }
            }
        }
        else if (len > 0) // "leftward"
        {
            std::cout << "case 3 (horizontal left)" << std::endl;
            int x = m_pFinal[0];
            for (int i = 0; x < m_pStart[0]; i++)
            {
                float ndc_x, ndc_y, ndc_z;
                ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                for (int j = 0; j < _lineWidth; j++)
                {
                    this->convertToNDC(x, yvalue + j, &ndc_x, &ndc_y);
                    std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                    points.insert(points.end(), point.begin(), point.end());
                }
                x++;

                if (counter == 23)
                {
                    counter = 0;
                    curr_pattern = _pattern;
                }
                else
                {
                    counter++;
                    // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                    // that was not a very technical description for a right bit shift of 1.
                    curr_pattern = curr_pattern >> 1;
                }
            }
        }
    }
    else if (m_pStart[0] == m_pFinal[0]) // vertical line
    {
        int xvalue = m_pStart[0];
        int len = m_pStart[1] - m_pFinal[1];
        int counter = 0;
        curr_pattern = _pattern;
        if (len < 0) // "upward"
        {
            std::cout << "case 2 (vertical up)" << std::endl;
            int y = m_pStart[1];
            for (int i = 0; y < m_pFinal[1]; i++)
            {
                float ndc_x, ndc_y, ndc_z;
                ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                for (int j = 0; j < _lineWidth; j++)
                {
                    this->convertToNDC(xvalue + j, y, &ndc_x, &ndc_y);
                    std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                    points.insert(points.end(), point.begin(), point.end());
                }
                y++;

                if (counter == 23)
                {
                    counter = 0;
                    curr_pattern = _pattern;
                }
                else
                {
                    counter++;
                    // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                    // that was not a very technical description for a right bit shift of 1.
                    curr_pattern = curr_pattern >> 1;
                }
            }
        }
        else if (len > 0) // "downward"
        {
            std::cout << "case 4 (vertical down)" << std::endl;
            int y = m_pFinal[1];
            for (int i = 0; y < m_pStart[1]; i++)
            {
                float ndc_x, ndc_y, ndc_z;
                ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                for (int j = 0; j < _lineWidth; j++)
                {
                    this->convertToNDC(xvalue + j, y, &ndc_x, &ndc_y);
                    std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                    points.insert(points.end(), point.begin(), point.end());
                }
                y++;

                if (counter == 23)
                {
                    counter = 0;
                    curr_pattern = _pattern;
                }
                else
                {
                    counter++;
                    // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                    // that was not a very technical description for a right bit shift of 1.
                    curr_pattern = curr_pattern >> 1;
                }
            }
        }
    }
    else if (std::abs(m_pFinal[0] - m_pStart[0]) == std::abs(m_pFinal[1] - m_pStart[1])) // 45 degree cases
    {
        int len = m_pStart[0] - m_pFinal[0];

        int x = m_pStart[0];
        int y = m_pStart[1];
        if (len < 0) // rightward
        {
            if (m_pFinal[1] > m_pStart[1]) // upward rightward
            {
                std::cout << "case 5 (45 degree 1st quadrant)" << std::endl;
                for (int i = 0; x < m_pFinal[0]; i++)
                {
                    float ndc_x, ndc_y, ndc_z;
                    ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                    for (int j = 0; j < _lineWidth; j++)
                    {
                        this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                        std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                        points.insert(points.end(), point.begin(), point.end());
                    }
                    x++;
                    y++;

                    if (counter == 23)
                    {
                        counter = 0;
                        curr_pattern = _pattern;
                    }
                    else
                    {
                        counter++;
                        // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                        // that was not a very technical description for a right bit shift of 1.
                        curr_pattern = curr_pattern >> 1;
                    }
                }
            }
            else // rightward downward
            {
                std::cout << "case 8 (45 degree 4th quadrant)" << std::endl;
                for (int i = 0; x < m_pFinal[0]; i++)
                {
                    float ndc_x, ndc_y, ndc_z;
                    ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                    for (int j = 0; j < _lineWidth; j++)
                    {
                        this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                        std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                        points.insert(points.end(), point.begin(), point.end());
                    }
                    x++;
                    y--;

                    if (counter == 23)
                    {
                        counter = 0;
                        curr_pattern = _pattern;
                    }
                    else
                    {
                        counter++;
                        // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                        // that was not a very technical description for a right bit shift of 1.
                        curr_pattern = curr_pattern >> 1;
                    }
                }
            }
        }
        else if (len > 0) // leftward
        {
            if (m_pFinal[1] > m_pStart[1]) // upward leftward
            {
                std::cout << "case 6 (45 degree 2nd quadrant)" << std::endl;
                for (int i = 0; x > m_pFinal[0]; i++)
                {
                    float ndc_x, ndc_y, ndc_z;
                    ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                    for (int j = 0; j < _lineWidth; j++)
                    {
                        this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                        std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                        points.insert(points.end(), point.begin(), point.end());
                    }
                    x--;
                    y++;

                    if (counter == 23)
                    {
                        counter = 0;
                        curr_pattern = _pattern;
                    }
                    else
                    {
                        counter++;
                        // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                        // that was not a very technical description for a right bit shift of 1.
                        curr_pattern = curr_pattern >> 1;
                    }
                }
            }
            else // leftward downward
            {
                std::cout << "case 7 (45 degree 3rd quadrant)" << std::endl;
                for (int i = 0; x > m_pFinal[0]; i++)
                {
                    float ndc_x, ndc_y, ndc_z;
                    ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                    for (int j = 0; j < _lineWidth; j++)
                    {
                        this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                        std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                        points.insert(points.end(), point.begin(), point.end());
                    }
                    x--;
                    y--;

                    if (counter == 23)
                    {
                        counter = 0;
                        curr_pattern = _pattern;
                    }
                    else
                    {
                        counter++;
                        // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                        // that was not a very technical description for a right bit shift of 1.
                        curr_pattern = curr_pattern >> 1;
                    }
                }
            }
        }
    }
    else // case 9 - 16 with bresenham
    {
        int dx = abs(m_pFinal[0] - m_pStart[0]);
        int dy = abs(m_pFinal[1] - m_pStart[1]);

        if (dx > dy)
        {
            if (m_pFinal[0] > m_pStart[0])
            {
                if (m_pFinal[1] > m_pStart[1])
                {
                    std::cout << "case 9 (theta < 45 degree 1st quadrant)" << std::endl;

                    int dR = 2 * dy;
                    int dUR = 2 * (dy - dx);
                    int d = 2 * dy - dx;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; x < m_pFinal[0]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        x++;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            y++;
                        }

                        if (counter == 23)
                        {
                            counter = 0;
                            curr_pattern = _pattern;
                        }
                        else
                        {
                            counter++;
                            // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                            // that was not a very technical description for a right bit shift of 1.
                            curr_pattern = curr_pattern >> 1;
                        }
                    }
                }
                else
                {
                    std::cout << "case 16 (theta < 45 degree 4th quadrant)" << std::endl;

                    int dR = 2 * dy;
                    int dUR = 2 * (dy - dx);
                    int d = 2 * dy - dx;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; x < m_pFinal[0]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        x++;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            y--;
                        }

                        if (counter == 23)
                        {
                            counter = 0;
                            curr_pattern = _pattern;
                        }
                        else
                        {
                            counter++;
                            // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                            // that was not a very technical description for a right bit shift of 1.
                            curr_pattern = curr_pattern >> 1;
                        }
                    }
                }
            }
            else
            {
                if (m_pFinal[1] > m_pStart[1])
                {
                    std::cout << "case 12 (theta < 45 degree 2nd quadrant)" << std::endl;

                    int dR = 2 * dy;
                    int dUR = 2 * (dy - dx);
                    int d = 2 * dy - dx;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; x > m_pFinal[0]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        x--;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            y++;
                        }

                        if (counter == 23)
                        {
                            counter = 0;
                            curr_pattern = _pattern;
                        }
                        else
                        {
                            counter++;
                            // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                            // that was not a very technical description for a right bit shift of 1.
                            curr_pattern = curr_pattern >> 1;
                        }
                    }
                }
                else
                {
                    std::cout << "case 13 (theta < 45 degree 3rd quadrant)" << std::endl;

                    int dR = 2 * dy;
                    int dUR = 2 * (dy - dx);
                    int d = 2 * dy - dx;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; x > m_pFinal[0]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        x--;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            y--;
                        }

                        if (counter == 23)
                        {
                            counter = 0;
                            curr_pattern = _pattern;
                        }
                        else
                        {
                            counter++;
                            // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                            // that was not a very technical description for a right bit shift of 1.
                            curr_pattern = curr_pattern >> 1;
                        }
                    }
                }

            }

        } // |dy| > |dx|
        else
        {
            if (m_pFinal[0] > m_pStart[0])
            {
                if (m_pFinal[1] > m_pStart[1])
                {
                    std::cout << "case 10 (theta > 45 degree 1st quadrant)" << std::endl;
                    int dR = 2 * dx;
                    int dUR = 2 * (dx - dy);
                    int d = 2 * dx - dy;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; y < m_pFinal[1]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x + j, y, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        y++;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            x++;
                        }

                        if (counter == 23)
                        {
                            counter = 0;
                            curr_pattern = _pattern;
                        }
                        else
                        {
                            counter++;
                            // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                            // that was not a very technical description for a right bit shift of 1.
                            curr_pattern = curr_pattern >> 1;
                        }
                    }
                }
                else
                {
                    std::cout << "case 15 (theta > 45 degree 4th quadrant)" << std::endl;
                    int dR = 2 * dx;
                    int dUR = 2 * (dx - dy);
                    int d = 2 * dx - dy;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; y > m_pFinal[1]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x + j, y, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        y--;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            x++;
                        }

                        if (counter == 23)
                        {
                            counter = 0;
                            curr_pattern = _pattern;
                        }
                        else
                        {
                            counter++;
                            // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                            // that was not a very technical description for a right bit shift of 1.
                            curr_pattern = curr_pattern >> 1;
                        }
                    }
                }

            }
            else
            {
                if (m_pFinal[1] > m_pStart[1])
                {
                    std::cout << "case 11 (theta > 45 degree 2nd quadrant)" << std::endl;
                    int dR = 2 * dx;
                    int dUR = 2 * (dx - dy);
                    int d = 2 * dx - dy;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; y < m_pFinal[1]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x + j, y, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        y++;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            x--;
                        }

                        if (counter == 23)
                        {
                            counter = 0;
                            curr_pattern = _pattern;
                        }
                        else
                        {
                            counter++;
                            // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                            // that was not a very technical description for a right bit shift of 1.
                            curr_pattern = curr_pattern >> 1;
                        }
                    }
                }
                else
                {
                    std::cout << "case 14 (theta > 45 degree 3rd quadrant)" << std::endl;
                    int dR = 2 * dx;
                    int dUR = 2 * (dx - dy);
                    int d = 2 * dx - dy;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; y > m_pFinal[1]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x + j, y, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        y--;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            x--;
                        }

                        if (counter == 23)
                        {
                            counter = 0;
                            curr_pattern = _pattern;
                        }
                        else
                        {
                            counter++;
                            // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                            // that was not a very technical description for a right bit shift of 1.
                            curr_pattern = curr_pattern >> 1;
                        }
                    }
                }
            }
        }
    }
    return points;
}