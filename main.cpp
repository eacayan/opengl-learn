#include "config.h"
#include "shader_s.h"

// Shape types enum
enum ShapeType {
    TRIANGLE,
    TWO_TRIANGLES,
    RECTANGLE_WITH_EBO
};

// Global variables for OpenGL objects
GLuint VAO, VBO, EBO;

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void setupTriangle();
void setupTwoTriangles();
void setupRectangleWithEBO();
void renderTriangle();
void renderTwoTriangles();
void renderRectangleWithEBO();
void cleanup(GLuint shaderProgram);

int main()
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Tatsulok", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
   // ------------------------------------
    Shader ourShader("shader_vs.glsl", "shader_fs.glsl");

    // Choose which shape to render
    ShapeType currentShape = TRIANGLE; // Change this to switch shapes

    // Setup the chosen shape
    switch (currentShape) {
    case TRIANGLE:
        setupTriangle();
        break;
    case TWO_TRIANGLES:
        setupTwoTriangles();
        break;
    case RECTANGLE_WITH_EBO:
        setupRectangleWithEBO();
        break;
    }

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Render
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // dark blue window color
        glClear(GL_COLOR_BUFFER_BIT);

        // 5. Draw the object
        // Use shader program to render the object
        ourShader.use();
        float offset = 0.5f;
        ourShader.setFloat("xOffset", offset);

        switch (currentShape) {
        case TRIANGLE:
            renderTriangle();
            break;
        case TWO_TRIANGLES:
            renderTwoTriangles();
            break;
        case RECTANGLE_WITH_EBO:
            renderRectangleWithEBO();
            break;
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    cleanup(ourShader.programID);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// Setup single triangle
void setupTriangle() {
    // Triangle vertices
    GLfloat vertices[] = {
        // positions         // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };

    // Generate vertex array object and vertex buffer object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 1. Bind Vertex Array Object
    glBindVertexArray(VAO);

    // 2. Copy our vertices array in a buffer for OpenGL to use
    // Bind buffer to a buffer type
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copy data into the currently bound buffer
    // GL_STATIC_DRAW: the data is set only once and used many times
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. Set our vertex attributes pointers
    // Linking vertex attributes
    // // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    // // color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Setup two triangles with EBO
void setupTwoTriangles() {
    // Rectangle made up of two triangles
    // side by side triangles share a vertex, optimized to use EBO (indices) so it won't render the shared vertex
    GLfloat vertices[] = {
        // first triangle
        -0.9f, -0.5f, 0.0f,  // left 0
        -0.0f, -0.5f, 0.0f,  // right 1
        -0.45f, 0.5f, 0.0f,  // top 2
        // second triangle
        // 0.0f, -0.5f, 0.0f,  // left 
         0.9f, -0.5f, 0.0f,  // right 3
         0.45f, 0.5f, 0.0f   // top  4
    };

    // Generate vertex array object and vertex buffer object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 1. Bind Vertex Array Object
    glBindVertexArray(VAO); 

    // 2. Copy our vertices array in a buffer for OpenGL to use
    // Bind buffer to a buffer type
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copy data into the currently bound buffer
    // GL_STATIC_DRAW: the data is set only once and used many times
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. Set our vertex attributes pointers
    // Linking vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    // EBO optimization part
    GLushort indices[] = { 
        0, 1, 2, 
        1, 3, 4 
    };
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Setup rectangle with EBO
void setupRectangleWithEBO() {
    // Rectangle, using EBO
    // EBO is like VBO but stores indices that OpenGL uses to decide what vertices to draw
    // This is called indexed drawing
    GLfloat vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    GLuint indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // Generate buffer with a unique ID
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Generate element buffer object
    glGenBuffers(1, &EBO);

    // 1. Bind Vertex Array Object
    glBindVertexArray(VAO);

    // 2. Copy our vertices array in a buffer for OpenGL to use
    // Bind buffer to a buffer type
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copy data into the currently bound buffer
    // GL_STATIC_DRAW: the data is set only once and used many times
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. Copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 4. Then set our vertex attributes pointers
    // Linking vertex attributes
    glEnableVertexAttribArray(0); // 0 for position, 1 for color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    // Unbind VBO (not EBO!)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind VAO
    glBindVertexArray(0);
}

// Render functions
void renderTriangle() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void renderTwoTriangles() {
    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 6); // if not using EBO
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

void renderRectangleWithEBO() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Cleanup resources
void cleanup(GLuint shaderProgram) {
    // Optional: de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
    }
    glDeleteProgram(shaderProgram);
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}