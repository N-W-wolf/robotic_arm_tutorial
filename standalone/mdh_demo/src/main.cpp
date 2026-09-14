#include <array>
#include <cmath>
#include <cstdio>
#include <string>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>

#include "mdh/kinematics.hpp"
#include "mdh/renderer.hpp"

namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr float kRadToDeg = 57.29577951308232f;

std::array<mdh::Parameter, 4> teaching_parameters() {
  return {{
      {0.00, 0.00, 0.12, 0.00},
      {0.00, kPi * 0.5, 0.00, 0.00},
      {0.18, 0.00, 0.00, 0.00},
      {0.18, 0.00, 0.00, 0.00},
  }};
}

void draw_matrix(const char* label, const Eigen::Matrix4d& matrix) {
  if (!ImGui::TreeNode(label)) {
    return;
  }

  for (int row = 0; row < 4; ++row) {
    ImGui::Text(
        "% .4f   % .4f   % .4f   % .4f",
        matrix(row, 0),
        matrix(row, 1),
        matrix(row, 2),
        matrix(row, 3));
  }
  ImGui::TreePop();
}

void draw_controls(std::array<double, 4>& joint_angles,
                   bool& show_grid,
                   bool& show_frames) {
  ImGui::SetNextWindowPos(ImVec2(16.0f, 16.0f), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(295.0f, 0.0f), ImGuiCond_Once);
  ImGui::Begin("MDH controls");

  ImGui::TextUnformatted("Direct kinematics");
  ImGui::Separator();
  ImGui::TextUnformatted("Joint angles");

  for (std::size_t i = 0; i < joint_angles.size(); ++i) {
    const std::string label = "q" + std::to_string(i + 1);
    float angle = static_cast<float>(joint_angles[i]);
    if (ImGui::SliderAngle(label.c_str(), &angle, -180.0f, 180.0f)) {
      joint_angles[i] = angle;
    }
  }

  if (ImGui::Button("Reset angles")) {
    joint_angles.fill(0.0);
  }
  ImGui::SameLine();
  ImGui::Checkbox("Grid", &show_grid);
  ImGui::Checkbox("Frames", &show_frames);

  ImGui::Separator();
  ImGui::TextUnformatted("Positive axis: +Z for every joint");
  ImGui::TextUnformatted("Units: m, rad, N*m");
  ImGui::End();
}

void draw_mdh_table(const std::array<mdh::Parameter, 4>& parameters) {
  ImGui::SetNextWindowPos(ImVec2(16.0f, 410.0f), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(430.0f, 250.0f), ImGuiCond_Once);
  ImGui::Begin("MDH parameters");

  if (ImGui::BeginTable("mdh_table", 5, ImGuiTableFlags_Borders |
                                             ImGuiTableFlags_RowBg)) {
    const char* headers[] = {"Joint", "a", "alpha", "d", "theta"};
    for (const char* header : headers) {
      ImGui::TableSetupColumn(header);
    }
    ImGui::TableHeadersRow();

    for (std::size_t i = 0; i < parameters.size(); ++i) {
      const auto& parameter = parameters[i];
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("q%zu", i + 1);
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%.3f", parameter.a);
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%.3f", parameter.alpha);
      ImGui::TableSetColumnIndex(3);
      ImGui::Text("%.3f", parameter.d);
      ImGui::TableSetColumnIndex(4);
      ImGui::Text("q%zu", i + 1);
    }
    ImGui::EndTable();
  }

  ImGui::TextUnformatted("A_i = Rx(alpha) Tx(a) Rz(theta) Tz(d)");
  ImGui::End();
}

void draw_results(const mdh::Result& result) {
  const float right_panel_width = 410.0f;
  const float right_panel_x =
      std::max(330.0f, ImGui::GetIO().DisplaySize.x - right_panel_width - 16.0f);
  ImGui::SetNextWindowPos(
      ImVec2(right_panel_x, 16.0f), ImGuiCond_Always);
  ImGui::SetNextWindowSize(
      ImVec2(right_panel_width, 0.0f), ImGuiCond_Always);
  ImGui::Begin("FK results", nullptr,
               ImGuiWindowFlags_AlwaysAutoResize);

  const Eigen::Isometry3d& end = result.frames.back();
  const Eigen::Vector3d position = end.translation();
  const Eigen::Vector3d orientation = mdh::rpy(end.rotation());

  ImGui::TextUnformatted("End-effector position");
  ImGui::Text("x = % .5f m", position.x());
  ImGui::Text("y = % .5f m", position.y());
  ImGui::Text("z = % .5f m", position.z());
  ImGui::Separator();
  ImGui::TextUnformatted("XYZ fixed-axis RPY");
  ImGui::Text("r = % .2f deg", orientation.x() * kRadToDeg);
  ImGui::Text("p = % .2f deg", orientation.y() * kRadToDeg);
  ImGui::Text("y = % .2f deg", orientation.z() * kRadToDeg);
  ImGui::Separator();

  for (std::size_t i = 0; i < result.relative.size(); ++i) {
    const std::string label = "A" + std::to_string(i + 1);
    draw_matrix(label.c_str(), result.relative[i].matrix());
  }
  draw_matrix("T04", result.frames.back().matrix());
  ImGui::End();
}

}  // namespace

int main() {
  if (!glfwInit()) {
    std::fprintf(stderr, "Failed to initialize GLFW\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  GLFWwindow* window =
      glfwCreateWindow(1440, 900, "MDH forward kinematics", nullptr, nullptr);
  if (window == nullptr) {
    std::fprintf(stderr, "Failed to create GLFW window\n");
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL2_Init();

  const auto parameters = teaching_parameters();
  std::array<double, 4> joint_angles = {0.0, 0.0, 0.0, 0.0};
  bool show_grid = true;
  bool show_frames = true;
  mdh::Renderer renderer;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    const mdh::Result result = mdh::forward(parameters, joint_angles);
    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
    renderer.draw(
        result.frames,
        framebuffer_width,
        framebuffer_height,
        show_grid,
        show_frames);

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    draw_controls(joint_angles, show_grid, show_frames);
    draw_mdh_table(parameters);
    draw_results(result);
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
