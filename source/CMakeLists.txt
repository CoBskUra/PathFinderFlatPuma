
# Add source to this project's executable.
add_executable (AngleInterpolation "Main.cpp" "Main.h"  "Constants.h" "resource.h" 
      

 "ShaderManadement/EBO.h" "ShaderManadement/Shader.h" "ShaderManadement/VAO.h" "ShaderManadement/VBO.h" 
 "ShaderManadement/shaderClass.cpp"  "ShaderManadement/Texture.h" "ShaderManadement/Texture.cpp"

 "ShaderRefrence/ShaderCreator.h" "ShaderRefrence/StaticShaders.cpp" "ShaderRefrence/StaticShaders.h" 
 
 

 "Helper/MathOperations.h"  "Helper/OpenGLHelper.h" "Helper/ImGuiExtenstions/imfilebrowser.h"
  "Transformations/Quaternion.cpp" "Transformations/Transpose.cpp" "Transformations/Transpose.h" "Transformations/TransposeImGui.h" "Transformations/Quaternion.h" "Transformations/QuaternionRotationImGui.cpp" "Transformations/QuaternionRotationImGui.h"
  "Transformations/Rotation.cpp" "Transformations/Rotation.h" "Transformations/RotationImGui.h" "Transformations/Scale.cpp" "Transformations/Scale.h" "Transformations/ScaleImGui.h"
   "Helper/FileManager.h"
 "PathFinder/Puma2D.h" "PathFinder/Obsticles.h" "vector2D.h" "PathFinder/Puma2D.cpp" "PathFinder/Rantagle.h" "PathFinder/Rantagle.cpp" "PathFinder/CorrdsTexture.h" "PathFinder/CorrdsTexture.cpp" "PathFinder/Obsticles.cpp")

  if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
            -Wall
            -Wextra
            -Wshadow
            -Wconversion
            -Wfloat-equal
            -Wunreachable-code
            -Wnull-dereference
            -Wno-unused-parameter
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /permissive)
endif ()

  target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR} ".")


 file(COPY ${CMAKE_SOURCE_DIR}/resource DESTINATION ${CMAKE_BINARY_DIR})



target_link_libraries(AngleInterpolation PRIVATE glm::glm)
target_link_libraries(AngleInterpolation PRIVATE glad::glad)
target_link_libraries(AngleInterpolation PRIVATE glfw)
target_link_libraries(AngleInterpolation PRIVATE imgui::imgui)




find_package(OpenMP REQUIRED)

# Dodanie flag kompilatora OpenMP
if(OpenMP_CXX_FOUND)
    target_link_libraries(AngleInterpolation PRIVATE OpenMP::OpenMP_CXX)
endif()

# Ustawienie katalogu z danymi
set(DATA_FILES_DIR "${CMAKE_SOURCE_DIR}/resource")

# Kopiowanie plików danych po zbudowaniu pliku wykonywalnego
add_custom_command(TARGET AngleInterpolation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${DATA_FILES_DIR} "${CMAKE_BINARY_DIR}/resource")

    # Sprawdzenie, czy kompilator obs³uguje OpenMP

