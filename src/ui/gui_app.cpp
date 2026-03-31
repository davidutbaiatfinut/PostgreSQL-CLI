#include "gui_app.hpp"
#include <iostream>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GL_SILENCE_DEPRECATION // macOS silencer
#include <GLFW/glfw3.h> 

#include "db/pg_query.hpp"
#include "db/db_exceptions.hpp"

namespace pgcli::ui {

static void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << "\n";
}

void runGui(core::AppContext& context) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;

    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "pgcli - Database Manager", nullptr, nullptr);
    if (window == nullptr)
        return;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; 

    // Dark sleek aesthetic requested by architectural guidelines
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.09f, 0.09f, 0.11f, 1.00f);
    char connStrBuf[512] = "";
    char queryBuf[1024] = "SELECT * FROM information_schema.tables LIMIT 10;";
    
    db::PGResultSet lastResult;
    std::string errorMessage;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Dockspace root setup
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // --- Connection Window ---
        ImGui::Begin("Connection Panel");
        ImGui::InputText("Connection URI", connStrBuf, IM_ARRAYSIZE(connStrBuf));
        if (ImGui::Button("Connect")) {
            try {
                errorMessage.clear();
                context.getDatabase().connect(std::string(connStrBuf));
                lastResult = {};
            } catch(const std::exception& e) {
                errorMessage = e.what();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Disconnect")) {
            context.getDatabase().disconnect();
        }
        ImGui::Text("Status: %s", context.getDatabase().isConnected() ? "Connected" : "Disconnected");
        if (!errorMessage.empty()) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: %s", errorMessage.c_str());
        }
        ImGui::End();

        // --- Editor Window ---
        ImGui::Begin("Query Editor");
        ImGui::InputTextMultiline("##SQL", queryBuf, IM_ARRAYSIZE(queryBuf), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10));
        if (ImGui::Button("Execute") && context.getDatabase().isConnected()) {
            try {
                errorMessage.clear();
                db::PGQuery q(context.getDatabase());
                lastResult = q.execute(std::string(queryBuf));
            } catch(const std::exception& e) {
                errorMessage = e.what();
                lastResult = {};
            }
        }
        ImGui::End();

        // --- Results Window ---
        ImGui::Begin("Results Grid");
        if (!context.getDatabase().isConnected()) {
            ImGui::Text("Not connected.");
        } else if (!lastResult.headers.empty()) {
            if (ImGui::BeginTable("result_table", lastResult.headers.size(), ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX)) {
                
                // Mount Columns
                for (const auto& h : lastResult.headers) {
                    ImGui::TableSetupColumn(h.c_str());
                }
                ImGui::TableHeadersRow();

                // Mount Data Values
                for (const auto& row : lastResult.rows) {
                    ImGui::TableNextRow();
                    for (size_t i = 0; i < row.columns.size(); i++) {
                        ImGui::TableSetColumnIndex(i);
                        if (row.columns[i].has_value()) {
                            ImGui::TextUnformatted(row.columns[i]->c_str());
                        } else {
                            ImGui::TextDisabled("NULL");
                        }
                    }
                }
                ImGui::EndTable();
            }
        } else if (!lastResult.commandStatus.empty()) {
            ImGui::Text("%s", lastResult.commandStatus.c_str());
        }
        ImGui::End();

        // Render pass
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup resources
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

} // namespace pgcli::ui
