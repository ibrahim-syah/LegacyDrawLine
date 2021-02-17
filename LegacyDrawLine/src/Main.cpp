#include <iostream>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl2.h"
#include "vendor/imgui/imfilebrowser.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "vendor/stb/stb_write_image.h"
#include <GLFW/glfw3.h>
#include <json/json.h>

#include "Line.h"



// initial setting
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
int* pStart = new int[2]{ 0, 0 };
int* pFinal = new int[2]{ 0, 0 };
int lineWidth = 1;
float line_color[] = { 0.0f, 0.0f, 0.0f };
int spacing_current = 0;

std::vector<Line*> onScreen; // vector to contain all the lines on currently on the screen

// Temporary Line
bool m_isheld = false;
int* tempStart = new int[2]{ 0, 0 };
int* tempFinal = new int[2]{ 0, 0 };

// Disable mouse
bool draw_state = true;
bool isFileBrowserOpen = false;

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

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

//void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void drawline(int spacing_current, int lineWidth, float line_color[]);
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void writeJSON(std::vector<Line*> allLines, float _clear_color[4], std::string filename);
bool readJSON(std::vector<Line*>* allLines, float _clear_color[4], std::string filename);

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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
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
    glfwSetMouseButtonCallback(window, mouse_callback);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(ImColor(15, 15, 15)));
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    // Our initial state
    float clear_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // Reserve a spot for temp line
    Line* tempLine = new Line(tempStart, tempFinal, SCR_WIDTH, SCR_HEIGHT, pattern[spacing_current], lineWidth, line_color);
    onScreen.push_back(tempLine);

    GLubyte* pixels = new GLubyte[600 * 600 * 3];
    

    // create a file browser instance
    ImGui::FileBrowser saveFileDialog(ImGuiFileBrowserFlags_EnterNewFilename);
    ImGui::FileBrowser saveJPGFileDialog(ImGuiFileBrowserFlags_EnterNewFilename);
    ImGui::FileBrowser loadFileDialog;

    // (optional) set browser properties
    saveFileDialog.SetTypeFilters({ ".json" });
    saveFileDialog.SetTitle("Save file");

    saveJPGFileDialog.SetTypeFilters({ ".jpg" });
    saveJPGFileDialog.SetTitle("Save file");

    loadFileDialog.SetTypeFilters({ ".json" });
    loadFileDialog.SetTitle("Load file");

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

        // Setting imgui flags
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoCollapse;

        {
            ImGui::Begin("Draw Line", (bool*)0, window_flags); // Create a window called "Hello, world!" and append into it.
            ImGui::SetWindowPos(ImVec2(600, 0));
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            ImGui::SetWindowSize(ImVec2(200, height));
            ImGui::Text("Background color");
            ImGui::ColorEdit3("##Background color", clear_color); // RGBA
            if (ImGui::IsItemActive() || isFileBrowserOpen) {
                draw_state = false;
            }
            else if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) && !isFileBrowserOpen) {
                draw_state = true;
            }
            
            if (ImGui::Button("Clear screen"))
            {

                for (int i = 1; i < onScreen.size(); i++)
                {
                    delete onScreen[i];
                }

                onScreen.clear();
                // Reserve a spot for temp line (again)
                Line* tempLine = new Line(tempStart, tempFinal, SCR_WIDTH, SCR_HEIGHT, pattern[spacing_current], lineWidth, line_color);
                onScreen.push_back(tempLine);

            }   
            ImGui::Text("\n");
            ImGui::Separator();
            ImGui::Text("\nStarting point");
            ImGui::InputInt2("##Starting point", pStart);
            ImGui::Text("Final point");
            ImGui::InputInt2("##Final point", pFinal);
            ImGui::Text("\n");
            ImGui::Separator();
            ImGui::Text("\nLine color");
            ImGui::ColorEdit3("##Line color", line_color); // RGB
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                onScreen[0]->m_line_color[0] = line_color[0];
                onScreen[0]->m_line_color[1] = line_color[1];
                onScreen[0]->m_line_color[2] = line_color[2];
            }
            if (ImGui::IsItemActive() || isFileBrowserOpen) {
                draw_state = false;
            }
            else if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) && !isFileBrowserOpen) {
                draw_state = true;
            }
            ImGui::Text("Line width");
            ImGui::SliderInt("##Line width", &lineWidth, 1, 5);
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                onScreen[0]->m_line_width = lineWidth;
            }
            ImGui::Text("Pixel Spacing");
            ImGui::Combo("##Pixel Spacing", &spacing_current, spacing, IM_ARRAYSIZE(spacing));
            if (ImGui::IsItemEdited()) {
                onScreen[0]->m_pattern = pattern[spacing_current];
            }
            if (ImGui::Button("Draw line"))
            {
                drawline(spacing_current, lineWidth, line_color);
            }
            ImGui::Text("\n");
            ImGui::Separator();
            ImGui::Text("\n");
            if (ImGui::Button("Save file"))
            {
                saveFileDialog.Open();
            }
            ImGui::SameLine();
            if (ImGui::Button("Open file"))
            {
                loadFileDialog.Open();
            }

            if (ImGui::Button("Save to JPG"))
            {
                glReadPixels(0, 0, 600, 600, GL_RGB, GL_UNSIGNED_BYTE, pixels);
                saveJPGFileDialog.Open();
            }


            ImGui::End();

            saveFileDialog.Display();
            saveJPGFileDialog.Display();
            loadFileDialog.Display();

            if (saveFileDialog.HasSelected())
            {
                std::cout << "Creating file: " << saveFileDialog.GetSelected().string() << std::endl;
                writeJSON(onScreen, clear_color, saveFileDialog.GetSelected().string());

                saveFileDialog.ClearSelected();
            }
            if (saveJPGFileDialog.HasSelected())
            {
                std::string filename = saveJPGFileDialog.GetSelected().string();
                if (filename.substr(filename.length() - 4, 4) != ".jpg") {
                    filename += ".jpg";
                }
                std::cout << "Creating file: " << filename << std::endl;
                stbi_flip_vertically_on_write(1);
                stbi_write_jpg(filename.c_str(), 600, 600, 3, pixels, 100);
                saveJPGFileDialog.ClearSelected();
            }

            if (loadFileDialog.HasSelected())
            {
                std::cout << "Selected file: " << loadFileDialog.GetSelected().string() << std::endl;

                // if something unexpected, just clear the screen
                if (!readJSON(&onScreen, clear_color, loadFileDialog.GetSelected().string()))
                {
                    std::cout << "failed reading file!" << std::endl;

                    clear_color[0] = 1.0f;
                    clear_color[1] = 1.0f;
                    clear_color[2] = 1.0f;
                    clear_color[3] = 1.0f;
                }
                loadFileDialog.ClearSelected();
            }

            if (saveFileDialog.IsOpened() || saveJPGFileDialog.IsOpened() || loadFileDialog.IsOpened()) {
                isFileBrowserOpen = true;
            }
            else {
                isFileBrowserOpen = false;
            }
        }

        // Rendering
        ImGui::Render();
        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_POINTS);

        /*int sampleStartingPoint[2] = { 0, 0 };
        int sampleFinalPoint[2] = { 1000, 500 };

        Line SampleLine(sampleStartingPoint, sampleFinalPoint, SCR_WIDTH, SCR_HEIGHT, 0xffffff, 5, line_color);

        SampleLine.SampleDDA();
        SampleLine.createPoints();*/

        if (m_isheld) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            onScreen[0]->m_pFinal[0] = xpos;
            onScreen[0]->m_pFinal[1] = -1 * (floor(ypos)) + SCR_HEIGHT;
            onScreen[0]->createPoints();
        }
        for (int i = 1; i < onScreen.size(); i++)
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

void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && draw_state) {
        double x;
        double y;
        glfwGetCursorPos(window, &x, &y);

        if (x < 600){
            // opengl set the coordinate from bottom left while glfw is from top left smh
            pStart[1] = -1 * (floor(y)) + SCR_HEIGHT;
            pStart[0] = floor(x);
            onScreen[0]->m_pStart[1]= -1 * (floor(y)) + SCR_HEIGHT;
            onScreen[0]->m_pStart[0] = floor(x);
            m_isheld = true;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        double x;
        double y;
        glfwGetCursorPos(window, &x, &y);

        if (m_isheld == true) {
            // opengl set the coordinate from bottom left while glfw is from top left smh
            pFinal[1] = -1 * (floor(y)) + SCR_HEIGHT;
            pFinal[0] = floor(x);
            m_isheld = false;
            drawline(spacing_current, lineWidth, line_color);
        }
    }
}

void writeJSON(std::vector<Line*> allLines, float _clear_color[4], std::string filename)
{
    Json::Value root;

    for (int i = 0; i < allLines.size(); i++)
    {
        std::string name = "line" + std::to_string(i);

        root["lines"][name]["scr_width"] = allLines[i]->m_width;
        root["lines"][name]["scr_height"] = allLines[i]->m_height;

        root["lines"][name]["pStart"]["x"] = allLines[i]->m_pStart[0];
        root["lines"][name]["pStart"]["y"] = allLines[i]->m_pStart[1];
        root["lines"][name]["pFinal"]["x"] = allLines[i]->m_pFinal[0];
        root["lines"][name]["pFinal"]["y"] = allLines[i]->m_pFinal[1];

        root["lines"][name]["line_width"] = allLines[i]->m_line_width;

        root["lines"][name]["pattern"] = allLines[i]->m_pattern;

        root["lines"][name]["line_color"]["R"] = allLines[i]->m_line_color[0];
        root["lines"][name]["line_color"]["G"] = allLines[i]->m_line_color[1];
        root["lines"][name]["line_color"]["B"] = allLines[i]->m_line_color[2];
    }

    root["clear_color"]["R"] = _clear_color[0];
    root["clear_color"]["G"] = _clear_color[1];
    root["clear_color"]["B"] = _clear_color[2];
    root["clear_color"]["A"] = _clear_color[3];

    root["scr_width"] = SCR_WIDTH;
    root["scr_height"] = SCR_HEIGHT;

    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "\t";
    std::string document = Json::writeString(wbuilder, root);

    std::fstream file;
    if (filename.substr(filename.length() - 5, 5) != ".json") {
        filename += ".json";
    }
    file.open((filename).c_str(), std::ios::out);
    if (!file) {
        std::cout << "File not created!" << std::endl;
    }
    else {
        std::cout << "File created successfully!" << std::endl;
        file << document << "\n";
        file.close();
    }
}

bool readJSON(std::vector<Line*>* allLines, float _clear_color[4], std::string filename)
{
    Json::Value root;
    Json::CharReaderBuilder rbuilder;
    rbuilder["collectComments"] = false;
    std::string errs;

    std::ifstream file;
    std::cout << filename.c_str() << std::endl;
    file.open(filename.c_str());
    if (!file) {
        std::cout << "File cannot be read!" << std::endl;
        return false;
    }
    else {
        std::cout << allLines->size() << std::endl;
        bool ok = Json::parseFromStream(rbuilder, file, &root, &errs);

        for (int i = 0; i < allLines->size(); i++)
        {
            delete (*allLines)[i];
        }

        allLines->clear();
        std::cout << allLines->size() << std::endl;

        const Json::Value lines = root["lines"];

        for (int i = 0; i < lines.size(); i++)
        {
            std::string name = "line" + std::to_string(i);

            int m_width = lines[name]["scr_width"].asInt();
            int m_height = lines[name]["scr_height"].asInt();

            int m_pStart[2] = { lines[name]["pStart"]["x"].asInt(), lines[name]["pStart"]["y"].asInt() };
            int m_pFinal[2] = { lines[name]["pFinal"]["x"].asInt(), lines[name]["pFinal"]["y"].asInt() };

            int m_line_width = lines[name]["line_width"].asInt();

            unsigned int m_pattern = lines[name]["pattern"].asUInt();

            float m_line_color[3] = { lines[name]["line_color"]["R"].asFloat(), lines[name]["line_color"]["G"].asFloat(), lines[name]["line_color"]["B"].asFloat() };
            allLines->push_back(new Line(m_pStart, m_pFinal, m_width, m_height, m_pattern, m_line_width, m_line_color));
        }

        _clear_color[0] = root["clear_color"]["R"].asFloat();
        _clear_color[1] = root["clear_color"]["G"].asFloat();
        _clear_color[2] = root["clear_color"]["B"].asFloat();
        _clear_color[3] = root["clear_color"]["A"].asFloat();

        SCR_WIDTH = root["scr_width"].asInt();
        SCR_HEIGHT = root["scr_height"].asInt();

        std::cout << "Read file successfully!" << std::endl;
        file.close();
        return true;
    }
}

void drawline(int spacing_current, int lineWidth, float line_color[]) {
    Line* newLine = new Line(pStart, pFinal, SCR_WIDTH, SCR_HEIGHT, pattern[spacing_current], lineWidth, line_color);
    onScreen.push_back(newLine);
}


