#include "KartGame.hpp"
#include "Model.hpp"

static float aspect;

KartGame::KartGame(int width, int height) : BaseApp(width, height, "Lab 2: Kart Game") {
    aspect = float(width) / height;
}

void KartGame::setup() {
    // Set paramaters
    lightDir = glm::vec3(0, 1, -1);
    // Basic GL configuration
    glEnable(GL_DEPTH_TEST); // depth test
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // cull back face
    glfwSetFramebufferSizeCallback(window, [] (GLFWwindow*, int width, int height) {
        aspect = float(width) / height;
        glViewport(0, 0, width, height);
    }); // set resize callback

    // Initialize components
    control = std::make_unique<Controller>(window, glm::vec3(6.5, 1, 0), 0);
    car = std::make_unique<Model>("../asset/car/car-n.obj");
    cone = std::make_unique<Model>("../asset/cone/cone2_obj.obj");
    track = std::make_unique<Model>(std::vector<Mesh>{
        CreateTrack(4.f, 9.f, 15.f, 25, 5)
    });
    terrain = std::make_unique<Model>(std::vector<Mesh>{
        CreateTerrain(100, 8)
    });
    diffuse = std::make_unique<Program>(
        "../src/shader/model.v.glsl", 
        "../src/shader/diffuse.f.glsl"
    );
    specular = std::make_unique<Program>(
        "../src/shader/model.v.glsl", 
        "../src/shader/specular.f.glsl"
    );
    trackShader = std::make_unique<Program>(
        "../src/shader/model.v.glsl", 
        "../src/shader/track.f.glsl"
    );
    shadow = std::make_unique<ShadowMap>(lightDir);
    sky = std::make_unique<Skybox>("../asset/sky/sky_", "png");

    // Compute cone model matrices
    constexpr auto coneUp = .25f;
    static const glm::vec3 conePos[] = {
        {4, coneUp, 8}, {-4, coneUp, -8},
    };
    constexpr auto coneScale = 1.5e-2f;
    for (auto &pos : conePos) {
        auto model = glm::translate(glm::identity<glm::mat4>(), pos);
        model = glm::scale(model, glm::vec3(coneScale));
        coneModelMats.push_back(model);
    }
}

void KartGame::draw() {
    // Update controller
    control->Update();

    // Create shadow map
    std::vector<std::pair<Model*, glm::mat4>> shadowTargets = {
        {car.get(), control->GetModelMatrix()},
    };
    for (auto &mat : coneModelMats) 
        shadowTargets.emplace_back(cone.get(), mat);
    shadow->Render(shadowTargets);

    // Compute common parameters for normal rendering
    glm::mat4 view;
    glm::vec3 camPos;
    std::tie(view, camPos) = control->GetView();
    auto proj = glm::perspective(glm::radians(45.f), aspect, .1f, 100.f);

    auto setUniform = [&] (Program &prog) {
        prog.Use();
        prog.SetVec3("camPos", camPos);
        prog.SetMat4("view", view);
        prog.SetMat4("proj", proj);
        prog.SetVec3("lightDir", lightDir);
        shadow->SetMap(prog); // uniforms provided by shadow map
    };

    // Render all visible targets
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setUniform(*specular);
    sky->SetCubemap(*specular);
    car->Draw(*specular, control->GetModelMatrix()); // car
    setUniform(*diffuse);
    for (auto &mat : coneModelMats)  // cones
        cone->Draw(*diffuse, mat);
    terrain->Draw(*diffuse, glm::translate(glm::identity<glm::mat4>(), {0, -1e-2f, 0})); // terrain
    setUniform(*trackShader);
    track->Draw(*trackShader, glm::identity<glm::mat4>()); // track
    sky->Render(view, proj); // skybox
}