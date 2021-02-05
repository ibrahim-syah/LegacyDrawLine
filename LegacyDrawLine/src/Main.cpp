#include <iostream>

#include <GLFW/glfw3.h>

#include "Line.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int SCR_WIDTH = 256;
int SCR_HEIGHT = 256;

//int* pStart = new int[2]{ 0, 0 };
//int* pFinal = new int[2]{ 0, 0 };

int main(void)
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    // required for apple machine
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Draw Line", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    int pStart[2] = { 0,100 };
    int pFinal[2] = { 1000,100 };

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_POINTS);

        Line newLine(pStart, pFinal, SCR_WIDTH, SCR_HEIGHT);
        std::vector<float> points = newLine.createPoints(0xffffff, 5);

        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);

    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}