#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <cstdio>

#include "Graph.hpp"
#include <GLFW/glfw3.h>


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Autobusy", nullptr, nullptr);

    if (window == nullptr)
        return EXIT_FAILURE;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddText(u8"Zażółć gęślą jaźń");
    ImVector<ImWchar> ranges;
    builder.BuildRanges(&ranges);

    io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 14, nullptr, ranges.Data);
    io.Fonts->Build();

    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

    std::vector<Point2D> stations = {{0, 0}};

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        /// USER CODE

        ImGui::Begin("Okno");
        ImGui::BeginTabBar("Tabs");

            if (ImGui::BeginTabItem("Algorytm"))
            {
                if (ImGui::CollapsingHeader("Stacje"))
                {
                    static int vec[2] = {0, 0};

                    ImGui::InputInt2("Position", vec);
                    if (ImGui::Button(u8"Dodaj nową stację"))
                    {
                        stations.emplace_back(vec[0], vec[1]);
                    }

                    /// Tabela z zapamiętanymi stacjami

                    static ImGuiTableFlags flags =
                            ImGuiTableFlags_ScrollY |
                            ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_BordersOuter |
                            ImGuiTableFlags_BordersV |
                            ImGuiTableFlags_Resizable |
                            ImGuiTableFlags_Reorderable |
                            ImGuiTableFlags_Hideable;

                    ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);
                    if (ImGui::BeginTable("Station_table", 4, flags, outer_size))
                    {
                        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
                        ImGui::TableSetupColumn("Nr.", ImGuiTableColumnFlags_None);
                        ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_None);
                        ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_None);

                        ImGui::TableHeadersRow();


                        ImGuiListClipper clipper;
                        clipper.Begin((int)stations.size());

                        while (clipper.Step())
                        {
                            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                            {
                                ImGui::PushID(row);

                                ImGui::TableNextRow();

                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("%d", row);

                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%d", stations[row].x);

                                ImGui::TableSetColumnIndex(2);
                                ImGui::Text("%d", stations[row].y);

                                ImGui::TableSetColumnIndex(3);
                                if (ImGui::SmallButton("Usuń"))
                                    stations.erase(stations.begin() + row);

                                ImGui::PopID();
                            }
                        }
                        ImGui::EndTable();
                    }
                }
                if (ImGui::CollapsingHeader("Połączenia stacji"))
                {

                }

                ImGui::EndTabItem();
            }


            if (ImGui::BeginTabItem("Styl"))
            {
                static int color_idx = 0;
                if (ImGui::Combo("Kolor##Selector", &color_idx, "Ciemny\0Jasny\0"))
                {
                    switch (color_idx)
                    {
                        default:
                        case 0: ImGui::StyleColorsDark(); break;
                        case 1: ImGui::StyleColorsLight(); break;
                    }
                }
                ImGui::EndTabItem();
            }

        ImGui::EndTabBar();
        ImGui::End();


        /// Most of the sample code is in ImGui::ShowDemoWindow()!
        /// You can browse its code to learn more about Dear ImGui!
        ImGui::ShowDemoWindow();
        ImPlot::ShowDemoWindow();

        /// END USER CODE

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}