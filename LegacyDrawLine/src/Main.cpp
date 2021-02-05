#include <iostream>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl2.h"
#include <GLFW/glfw3.h>

#include "Line.h"

// initial setting
int SCR_WIDTH = 800;
int SCR_HEIGHT = 800;
int* pStart = new int[2]{ 0, 0 };
int* pFinal = new int[2]{ 0, 0 };

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

//void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(void)
{
    // glfw: initialize and configure
    // ------------------------------
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our initial state
    float clear_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float line_color[] = { 0.0f, 0.0f, 0.0f };
    //float point_size = 1.0f;
    int lineWidth = 1;
    int spacing_current = 0;
    const char* spacing[] = { // 2^24
        "0 pixel",
        "4 pixels",
        "8 pixel",
        "12 pixel",
        "16 pixel",
        "20 pixel",
    };

    const unsigned int pattern[] = {
        0xffffff,
        0x0fffff,
        0x00ffff,
        0x000fff,
        0x0000ff,
        0x00000f,
    };

    std::vector<Line*> onScreen;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        processInput(window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Draw Line"); // Create a window called "Hello, world!" and append into it.
            ImGui::ColorEdit4("Background color", clear_color); // RGBA

            ImGui::Text("These settings are applied on the next draw");
            ImGui::InputInt2("Starting point", pStart);
            ImGui::InputInt2("Final point", pFinal);
            ImGui::ColorEdit3("Line color", line_color); // RGB
            ImGui::SliderInt("Line width", &lineWidth, 1, 5);

            ImGui::Combo("Pixel Spacing", &spacing_current, spacing, IM_ARRAYSIZE(spacing));
            if (ImGui::Button("Draw line"))
            {
                Line* newLine = new Line(pStart, pFinal, SCR_WIDTH, SCR_HEIGHT, pattern[spacing_current], lineWidth, line_color);
                onScreen.push_back(newLine);
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear screen"))
            {
                clear_color[0] = 1.0f;
                clear_color[1] = 1.0f;
                clear_color[2] = 1.0f;
                clear_color[3] = 1.0f;

                for (int i = 0; i < onScreen.size(); i++)
                {
                    delete onScreen[i];
                }

                onScreen.clear();
            }
            /*if (ImGui::Button("Save file"))
            {
                saveFileDialog.Open();
            }
            ImGui::SameLine();
            if (ImGui::Button("Open file"))
            {
                loadFileDialog.Open();
            }*/

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

            //saveFileDialog.Display();
            //loadFileDialog.Display();

            //if (saveFileDialog.HasSelected())
            //{
            //    std::cout << "Creating file: " << saveFileDialog.GetSelected().string() << std::endl;
            //    *newLine = Line(pStart, pFinal, SCR_WIDTH, SCR_HEIGHT);
            //    newLine->writeJSON(saveFileDialog.GetSelected().string(), point_size, spacing_current, clear_color, line_color, lineWidth);

            //    saveFileDialog.ClearSelected();
            //}

            //if (loadFileDialog.HasSelected())
            //{
            //    std::cout << "Selected file: " << loadFileDialog.GetSelected().string() << std::endl;

            //    // this is really ugly
            //    *newLine = Line(pStart, pFinal, SCR_WIDTH, SCR_HEIGHT);
            //    if (newLine->readJSON(loadFileDialog.GetSelected().string(), pStart, pFinal, &point_size, &spacing_current, clear_color, line_color, &lineWidth))
            //    {
            //        recalculateVertices(pattern[spacing_current], lineWidth, &numOfPixels, &VAO);
            //    }
            //    loadFileDialog.ClearSelected();
            //}
        }

        // Rendering
        ImGui::Render();
        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_POINTS);

        for (int i = 0; i < onScreen.size(); i++)
        {
            onScreen[i]->createPoints();
        }

        glEnd();

        //// If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
        //// you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
        ////GLint last_program;
        ////glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        ////glUseProgram(0);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        ////glUseProgram(last_program);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    // Cleanup
    for (int i = 0; i < onScreen.size(); i++)
    {
        delete onScreen[i];
    }

    delete pStart;
    delete pFinal;

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        double x;
        double y;
        glfwGetCursorPos(window, &x, &y);

        // opengl set the coordinate from bottom left while glfw is from top left smh
        pStart[1] = -1 * (floor(y)) + SCR_HEIGHT;
        pStart[0] = floor(x);
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        double x;
        double y;
        glfwGetCursorPos(window, &x, &y);

        // opengl set the coordinate from bottom left while glfw is from top left smh
        pFinal[1] = -1.0 * (floor(y)) + SCR_HEIGHT;
        pFinal[0] = floor(x);
    }
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

//void mouse_callback(GLFWwindow* window, int button, int action, int mods)
//{
//    // if i use mouse button, clicking anything on imgui window will trigger it as well
//    if (button == GLFW_MOUSE_BUTTON_LEFT) {
//        double x;
//        double y;
//        glfwGetCursorPos(window, &x, &y);
//
//        // opengl set the coordinate from bottom left while glfw is from top left smh
//        pStart[1] = -1 * (floor(y)) + SCR_HEIGHT;
//        pStart[0] = floor(x);
//    }
//
//    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
//        double x;
//        double y;
//        glfwGetCursorPos(window, &x, &y);
//
//        // opengl set the coordinate from bottom left while glfw is from top left smh
//        pFinal[1] = -1 * (floor(y)) + SCR_HEIGHT;
//        pFinal[0] = floor(x);
//    }
//}