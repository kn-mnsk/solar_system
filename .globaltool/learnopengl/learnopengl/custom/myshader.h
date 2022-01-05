#ifndef MYSHADER_H
#define MYSHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace MyShader {
    class Shader {
    public:
        unsigned int ID;
        // constructor generates the shader on the fly
        // ------------------------------------------------------------------------
        Shader(const char* vertPath, const char* fragPath, const char* tescPath = nullptr, const char* tesePath = nullptr, const char* geomPath = nullptr) {
            // 1. retrieve the vertex/fragment source code from filePath
            std::string openHistory{};
            std::string vertCode;
            std::string fragCode;
            std::string tescCode;
            std::string teseCode;
            std::string geomCode;
            std::ifstream vertFile;
            std::ifstream fragFile;
            std::ifstream tescFile;
            std::ifstream teseFile;
            std::ifstream geomFile;
            // ensure ifstream objects can throw exceptions:
            vertFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            tescFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            teseFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            geomFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try {
                // open files
                openHistory.append(getFileName(vertPath) + "; ");
                vertFile.open(vertPath);
                std::stringstream vertSream;
                vertSream << vertFile.rdbuf();// read file's buffer contents into streams
                vertFile.close();// close file handlers
                vertCode = vertSream.str();// convert stream into string

                openHistory.append(getFileName(fragPath) + "; ");
                fragFile.open(fragPath);
                std::stringstream fragStream;
                fragStream << fragFile.rdbuf();
                fragFile.close();
                fragCode = fragStream.str();

                // if tesc shader path is present, also load a geometry shader
                if (tescPath != nullptr) {
                    openHistory.append(getFileName(tescPath) + "; ");
                    tescFile.open(tescPath);
                    std::stringstream tescStream;
                    tescStream << tescFile.rdbuf();
                    tescFile.close();
                    tescCode = tescStream.str();
                }
                // if tese shader path is present, also load a geometry shader
                if (tesePath != nullptr) {
                    openHistory.append(getFileName(tesePath) + "; ");
                    teseFile.open(tesePath);
                    std::stringstream teseStream;
                    teseStream << teseFile.rdbuf();
                    teseFile.close();
                    teseCode = teseStream.str();
                }
                // if geometry shader path is present, also load a geometry shader
                if (geomPath != nullptr) {
                    openHistory.append(getFileName(geomPath));
                    geomFile.open(geomPath);
                    std::stringstream geomSream;
                    geomSream << geomFile.rdbuf();
                    geomFile.close();
                    geomCode = geomSream.str();
                }
            }
            catch (std::ifstream::failure& e) {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << " => last File Not Opened: " << openHistory << std::endl;
            }

            // 2. compile shaders
            const char* vertCode_ = vertCode.c_str();
            unsigned int vert;
            vert = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vert, 1, &vertCode_, NULL);
            glCompileShader(vert);
            checkCompileErrors(vert, "VERTEX: File -> " + getFileName(vertPath));

            const char* fragCode_ = fragCode.c_str();
            unsigned int frag;
            frag = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(frag, 1, &fragCode_, NULL);
            glCompileShader(frag);
            checkCompileErrors(frag, "FRAGMENT: File -> " + getFileName(fragPath));

            unsigned int tesc;
            if (tescPath != nullptr) {
                const char* tescCode_ = tescCode.c_str();
                tesc = glCreateShader(GL_TESS_CONTROL_SHADER);
                glShaderSource(tesc, 1, &tescCode_, NULL);
                glCompileShader(tesc);
                checkCompileErrors(tesc, "TESS CONTROL : File -> " + getFileName(tescPath));
            }

            unsigned int tese;
            if (tesePath != nullptr) {
                const char* teseCode_ = teseCode.c_str();
                tese = glCreateShader(GL_TESS_EVALUATION_SHADER);
                glShaderSource(tese, 1, &teseCode_, NULL);
                glCompileShader(tese);
                checkCompileErrors(tese, "TESS EVALUATION: File -> " + getFileName(tesePath));
            }

            unsigned int geom;
            if (geomPath != nullptr) {
                const char* geomCode_ = geomCode.c_str();
                geom = glCreateShader(GL_GEOMETRY_SHADER);
                glShaderSource(geom, 1, &geomCode_, NULL);
                glCompileShader(geom);
                checkCompileErrors(geom, "GEOMETRY: File -> " + getFileName(geomPath));
            }

            // link Programs
            ID = glCreateProgram();
            glAttachShader(ID, vert);
            glAttachShader(ID, frag);
            if (tescPath != nullptr)
                glAttachShader(ID, tesc);
            if (tesePath != nullptr)
                glAttachShader(ID, tese);
            if (geomPath != nullptr)
                glAttachShader(ID, geom);
            glLinkProgram(ID);
            checkCompileErrors(ID, "PROGRAM");

            // delete the shaders as they're linked into our program now and no longer necessery
            glDeleteShader(vert);
            glDeleteShader(frag);
            if (tescPath != nullptr)
                glDeleteShader(tesc);
            if (tesePath != nullptr)
                glDeleteShader(tese);
            if (geomPath != nullptr)
                glDeleteShader(geom);

        }
        // activate the shader
        // ------------------------------------------------------------------------
        void use()
        {
            glUseProgram(ID);
        }
        // utility uniform functions
        // ------------------------------------------------------------------------
        void setBool(const std::string& name, bool value) const
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        }
        // ------------------------------------------------------------------------
        void setInt(const std::string& name, int value) const
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        }
        // ------------------------------------------------------------------------
        void setFloat(const std::string& name, float value) const
        {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        }
        // ------------------------------------------------------------------------
        void setVec2(const std::string& name, const glm::vec2& value) const
        {
            glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setVec2(const std::string& name, float x, float y) const
        {
            glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
        }
        // ------------------------------------------------------------------------
        void setVec3(const std::string& name, const glm::vec3& value) const
        {
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setVec3(const std::string& name, float x, float y, float z) const
        {
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
        }
        // ------------------------------------------------------------------------
        void setVec4(const std::string& name, const glm::vec4& value) const
        {
            glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setVec4(const std::string& name, float x, float y, float z, float w)
        {
            glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
        }
        // ------------------------------------------------------------------------
        void setMat2(const std::string& name, const glm::mat2& mat) const
        {
            glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        void setMat3(const std::string& name, const glm::mat3& mat) const
        {
            glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        void setMat4(const std::string& name, const glm::mat4& mat) const
        {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

    private:
        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
        void checkCompileErrors(GLuint shader, std::string type)
        {
            GLint success;
            GLchar infoLog[1024];
            if (type != "PROGRAM")
            {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                }
            }
            else
            {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success)
                {
                    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                }
            }
        }

        // find file name
        std::string getFileName(const char* filePath)
        {
            std::string str = std::string(filePath);
            std::size_t found = std::string(str).find_last_of("/\\");
            std::string path = str.substr(0, found);
            std::string filename = str.substr(found + 1);
            return filename;
        }

    };
}
#endif
