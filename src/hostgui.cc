#define STB_IMAGE_IMPLEMENTATION
// dont include this file anywhere else.
// dont include it in a header file;
#include <algorithm>
#include <stb/stb_image.h>
#include "hostgui.h"

HostGui::HostGui(){
    thread_ = std::thread{&HostGui::ThreadMain, this};
}
HostGui::~HostGui(){
    if(thread_.joinable()){
        thread_.join();
    }
}

void HostGui::TurnOff(){
	is_on_ = false;
}

void HostGui::TurnOn(){
	is_on_ = true;
}
unsigned int HostGui::win_width_ = 1280;
unsigned int HostGui::win_height_ = 720;

void HostGui::ThreadMain(){
    glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()){
		std::cout << "Failed to init glfw\n";
        return;
    }

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(win_width_, win_height_, "Host", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		std::cout << "Failed to create glfw window\n";
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, HostGui::framebuffer_size_callback);
	glfwSwapInterval(1); // Enable vsync

	bool err = gl3wInit() != 0;
	if (err){
		std::cout << "Failed to initialize OpenGL loader!\n";
		return;
	}

	glEnable(GL_DEPTH_TEST);

	Shader shader_planes{"./src/planes.vs", "./src/planes.fs"};

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// TODO: class Texture
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // NOTE: I failed to load .png that I made using photoshop 2019, .jpg works though
    unsigned char *data = stbi_load("./resources/yb256.jpg", &width, &height, &nrChannels, 0);
    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    shader_planes.use();
    shader_planes.setInt("texture1", 0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	io.Fonts->AddFontFromFileTTF("./resources/DroidSans.ttf", 20.0f);

	bool show_demo_window = false;

	while (!glfwWindowShouldClose(window) && is_on_){
		processInput(window);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		/* Render ImGui here */
		MainPanel();

		ImGui::Render();
		// glfwMakeContextCurrent(window);
		glClearColor(0.9f, 0.9f, 0.9f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

		// TODO: package this section about transformation
        glm::mat4 model         = glm::mat4(1.0f);
        glm::mat4 view;//          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        model = glm::rotate(model, /*(float)glfwGetTime()*/30.f, glm::vec3(0.f, 1.0f, 0.0f));
		float cam_x = sin(glfwGetTime()) * map_range_;
		float cam_z = cos(glfwGetTime()) * map_range_;
		view = glm::lookAt(glm::vec3(cam_x, 2.f, cam_z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(60.0f), (float)win_width_ / (float)win_height_, 0.1f, 100.0f);

		/* Render OpenGL primitives here */
		if(show_planes_){
			glViewport(0, 0, win_width_/2, win_height_/2);
			RenderPlanes(shader_planes, model, view, projection);
			glViewport(0, win_height_/2, win_width_/2, win_height_/2);
			RenderPlanes(shader_planes, model, view, projection);
		}
		if(show_point_cloud_){
			RenderPointCloud();
		}
		if(show_path_togo_){
			RenderPathToGo();
		}
		if(show_path_been_){
			RenderPathBeen();
		}
		if(show_image_window_){
			RenderImageWindow();
		}

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

    is_on_ = false;
}

void HostGui::MainPanel(){
	if(!ImGui::Begin("Panel")){
        ImGui::End();
        return;
    }

	/* add primitives */
	static float x1 = 0, y1 = 0, z1 = 0, x2 = 1, y2 = 1, z2 = 1;
	ImGui::PushItemWidth(100);
	ImGui::InputFloat("x1", &x1, 0.5, 2, "%.1f"); ImGui::SameLine();
	ImGui::InputFloat("y1", &y1, 0.5, 2, "%.1f"); ImGui::SameLine();
	ImGui::InputFloat("z1", &z1, 0.5, 2, "%.1f");
	ImGui::InputFloat("x2", &x2, 0.5, 2, "%.1f"); ImGui::SameLine();
	ImGui::InputFloat("y2", &y2, 0.5, 2, "%.1f"); ImGui::SameLine();
	ImGui::InputFloat("z2", &z2, 0.5, 2, "%.1f");
	ImGui::PopItemWidth();
	if(ImGui::Button("Add Plane")){
		AddPlane(x1, y1, z1, x2, y2, z2);
	}
    /* parameter control */
    ImGui::Separator();
    ImGui::PushItemWidth(180);
    static float pre_map_range = map_range_;
	static float crt_map_range = map_range_;
    pre_map_range = crt_map_range;
    if((ImGui::InputFloat("Map Range(m)", &crt_map_range, 1.0, 2.0, "%.0f"))){
		std::lock_guard<std::mutex> guard{mtx_};
		if(crt_map_range <= 0.f){
			crt_map_range = pre_map_range;
		}
		map_range_ = crt_map_range;
        auto scale = [coe = pre_map_range/map_range_](auto &ele){return coe*ele;};
		std::transform(planes_.begin(), planes_.end(), planes_.begin(), scale);
        std::transform(path_togo_.begin(), path_togo_.end(), path_togo_.begin(), scale);
		std::transform(path_been_.begin(), path_been_.end(), path_been_.begin(), scale);
        std::transform(point_cloud_.begin(), point_cloud_.end(), point_cloud_.begin(), scale);
    }
    ImGui::PopItemWidth();

    /* render option */
    ImGui::Separator();
    ImGui::Text("Render Option");
    ImGui::Checkbox("Compact Model", &show_planes_);
    // ImGui::Text("%d rectangles", GetCompactModelData().size());
    ImGui::Checkbox("Point Cloud", &show_point_cloud_);
    // ImGui::Text("%d points", point_cloud_gl_.size()/3);
    ImGui::Checkbox("Flight Path To Go", &show_path_togo_);
	ImGui::Checkbox("Flight Path Been", &show_path_been_);

    ImGui::Separator();
    static bool show_demo_window = false;
    ImGui::Checkbox("Imgui Demo Window", &show_demo_window);
    if(show_demo_window){
        ImGui::ShowDemoWindow(&show_demo_window);
    }
    // NOTE: this is NOT the fps of the main application
    ImGui::Text("%.1f ms/frame (%.0f FPS)", 1000.0f/ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("window width: %d, height: %d", win_width_, win_height_);
    ImGui::End();
}

void HostGui::RenderPlanes(Shader &shader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection){
	shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

	auto n_plane = planes_.size();
    /* draw model data */
    for(auto i=0; i<n_plane; i+=6){
		auto x1 = planes_[i+0];
		auto y1 = planes_[i+1];
		auto z1 = planes_[i+2];
		auto x2 = planes_[i+3];
		auto y2 = planes_[i+4];
		auto z2 = planes_[i+5];
        /*
            a rectangle is composed of two triangles
            p3-----*p2*
            | \     |
            |   \   |
            |     \ |
           *p1*-----p4
        */
        float texture_width = sqrt(pow(x1-x2, 2) + pow(y1-y2, 2)) * map_range_ / 2.f;
        float texture_height = abs(z1-z2) * map_range_ / 2.f;
        float rectangle_gl[] = {
            x1, z1, -y1, 0.f, 			0.f,               // p1
            x1, z2, -y1, 0.f, 			texture_height,    // p3
        	x2, z1, -y2, texture_width, 0.f,               // p4,
        	x2, z2, -y2, texture_width, texture_height,    // p2
            x1, z2, -y1, 0.f, 			texture_height,    // p3
           	x2, z1, -y2, texture_width, 0.f                // p4
        };

        unsigned int VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_gl), rectangle_gl, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(rectangle_gl)/sizeof(float));
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

}
// TODO:
void HostGui::RenderPointCloud(){

}
void HostGui::RenderPathToGo(){

}
void HostGui::RenderPathBeen(){

}
void HostGui::RenderImageWindow(){

}

void HostGui::AddPlane(float x1, float y1, float z1, float x2, float y2, float z2){
	std::lock_guard<std::mutex> guard{mtx_};
	planes_.push_back(x1/map_range_);
	planes_.push_back(y1/map_range_);
	planes_.push_back(z1/map_range_);
	planes_.push_back(x2/map_range_);
	planes_.push_back(y2/map_range_);
	planes_.push_back(z2/map_range_);
}
void HostGui::FlushPlanes(){
	std::lock_guard<std::mutex> guard{mtx_};
	planes_.clear();
}
// TODO:
void HostGui::AddPointToGo(float x, float y, float z){

}
void HostGui::FlushPointToGo(){

}
void HostGui::AddPointBeen(float x, float y, float z){

}
void HostGui::FlushPointBeen(){

}
void HostGui::AddPlotVal(int no, float value){

}

void HostGui::framebuffer_size_callback(GLFWwindow* window, int width, int height){
	// glViewport(0, 0, width, height);
	win_width_ = width;
	win_height_ = height;
}
void HostGui::processInput(GLFWwindow *window){
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
void HostGui::glfw_error_callback(int error, const char* description){
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}