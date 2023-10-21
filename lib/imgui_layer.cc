#include "imgui_layer.hh"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

ImGuiLayer::ImGuiLayer(GLFWwindow* window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 450");
}

void ImGuiLayer::begin_frame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (m_opt_fullscreen) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    m_window_flags |= ImGuiWindowFlags_NoTitleBar |
                      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                      ImGuiWindowFlags_NoMove;
    m_window_flags |=
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  }

  if (m_dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
    m_window_flags |= ImGuiWindowFlags_NoBackground;
  }

  if (!m_opt_padding) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  }

  m_open = true;
}

void ImGuiLayer::update(Framebuffer<GL_TEXTURE_2D>& framebuffer) {
  auto& io = ImGui::GetIO();
  ImGui::Begin("DockSpace", &m_open, m_window_flags);

  if (!m_opt_padding) {
    ImGui::PopStyleVar();
  }

  if (m_opt_fullscreen) {
    ImGui::PopStyleVar(2);
  }

  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    auto dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_dockspace_flags);
  }

  ImGui::End();

  ImGui::Begin("Debug");
  ImGui::Text("Delta Time: %.3f ms", 1000.0f / io.Framerate);
  // ImGui::Text("Vertices: %zu", props.meshVertices);
  ImGui::End();

  // ImGui::Begin("Properties");
  // ImGui::Text("Camera:");
  // ImGui::SliderFloat("Fov", &props.cameraFov, 10.0, 120.0);
  // props.reloadShaders = ImGui::Button("Reload Shaders");
  // ImGui::End();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("Viewport");

  m_dim = ImGui::GetContentRegionAvail();
  // fmt::println("{} {}", m_dim.x, m_dim.y);
  ImGui::Image((void*)(intptr_t)framebuffer.get_color_attachments()[0].get_id(),
               m_dim, ImVec2 { 0.0, 0.0 }, ImVec2 { 1.0, -1.0 });

  ImGui::End();
  ImGui::PopStyleVar();
}

void ImGuiLayer::end_frame() {
  auto& io = ImGui::GetIO();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

ImGuiLayer::~ImGuiLayer() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
