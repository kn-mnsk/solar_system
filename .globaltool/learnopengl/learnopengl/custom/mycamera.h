#ifndef MYCAMERA_H
#define MYCAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace MyCamera {

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW{ -90.0f };// -90.0f;
const float PITCH{ 0.0f };
const float ROLL{ 0.0f };
const float SPEED{ 2.5f };
const float SENSITIVITY{ 0.01f };
const float ZOOM{ 45.0f }; // = FOV


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
//namespace MyCamera {
    class CameraStar
    {
    public:
        // Camera Attributes
        glm::vec3 PositionWS{}; // world space
        glm::vec3 FrontWS{};
        glm::vec3 DirectionWS{};
        glm::vec3 UpWS{};
        // Position speher params
        float deltaTheta{}; // degree
        float deltaPhi{}; // degree

        // Euler Angles
        float Yaw;
        float Pitch;
        float Roll;
        // Camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Fov;
        bool Outward;

        glm::vec3 Front; // world space
        glm::vec3 Up;

        CameraStar(glm::vec3 position, glm::vec3 front, glm::vec3 up, float fov) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY) {
            // outward: camera front face is outward; set inward in case of SolarCamera
            PositionWS = position;
            FrontWS = front;
            UpWS = up;
            Fov = fov;
        }

        // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 GetViewMatrix()
        {
            return glm::lookAt(PositionWS, PositionWS + FrontWS, UpWS);
        }

        // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yoffset)
        {
            Fov -= yoffset;
            Fov = glm::clamp(Fov, 1.0f, 160.f);
        }

        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
        {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            deltaPhi =  xoffset;
            deltaTheta = yoffset;

        }


    private:

        // Calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors()
        {  // No use in this version

            glm::vec3 Z = glm::normalize(DirectionWS);
            glm::vec3 X = glm::normalize(glm::cross(UpWS, Z));
            glm::vec3 Y = glm::normalize(glm::cross(Z, X));
          
            Z = glm::normalize(glm::rotate(Z, glm::radians(Yaw), Y));
            X = glm::normalize(glm::rotate(X, glm::radians(Yaw), Y));

            Z = glm::normalize(glm::rotate(Z, glm::radians(Pitch), X));
            Y = glm::normalize(glm::rotate(Y, glm::radians(Pitch), X));

            X = glm::normalize(glm::rotate(X, glm::radians(Roll), Z));
            Y = glm::normalize(glm::rotate(Y, glm::radians(Roll), Z));


            Front = glm::vec3(0.0f, 0.0f, -1.0f);
            Up = glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 XYZ{ 1.0f };
            XYZ[0] = glm::vec4(X, 0.0f);
            XYZ[1] = glm::vec4(Y, 0.0f);
            XYZ[2] = glm::vec4(Z, 0.0f);
            XYZ = glm::inverse(glm::transpose(XYZ));

            // transform back to world space
            Front = glm::normalize(glm::vec3(XYZ * glm::vec4(Front, 1.0f)));
            Up = glm::normalize(glm::vec3(XYZ * glm::vec4(Up, 1.0f)));

        }

    };

    class Camera
    {
    public:
        // Camera Attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        // Euler Angles
        float Yaw;
        float Pitch;
        // Camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Fov;

        // Constructor with vectors
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(ZOOM)
        {
            Position = position;
            WorldUp = up;
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }
        // Constructor with scalar values
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(ZOOM)
        {
            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 GetViewMatrix()
        {
            return glm::lookAt(Position, Position + Front, Up);
        }

        // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(Camera_Movement direction, float deltaTime)
        {
            float velocity = MovementSpeed * deltaTime;
            if (direction == Camera_Movement::FORWARD)
                Position += Front * velocity;
            if (direction == Camera_Movement::BACKWARD)
                Position -= Front * velocity;
            if (direction == Camera_Movement::LEFT)
                Position -= Right * velocity;
            if (direction == Camera_Movement::RIGHT)
                Position += Right * velocity;
        }

        // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
        {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            Yaw += xoffset;
            Pitch += yoffset;

            // Make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch)
            {
                Pitch = glm::clamp(Pitch, -89.0f, 89.0f);

               /* if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;*/
            }

            // Update Front, Right and Up Vectors using the updated Euler angles
            updateCameraVectors();
        }

        // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yoffset)
        {

            Fov -= yoffset;
            Fov = glm::clamp(Fov, 1.0f, 90.0f);

           /* if (Fov >= 1.0f && Fov <= 45.0f)
                Fov -= yoffset;
            if (Fov <= 1.0f)
                Fov = 1.0f;
            if (Fov >= 45.0f)
                Fov = 45.0f;*/
        }

    private:
        // Calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors()
        {
            // Calculate the new Front vector
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
            // Also re-calculate the Right and Up vector
            Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            Up = glm::normalize(glm::cross(Right, Front));
        }
    }; 
    
    
    class Camera2
    {
    public:
        // Camera Attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        // Euler Angles
        float Yaw;
        float Pitch;
        // Camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Fov;

        // Constructor with vectors
        Camera2(glm::vec3 position, glm::vec3 front, glm::vec3 up ):  MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(ZOOM)
        {
            Position = position;
            Front = front;
            Up = up;
            WorldUp = up;
            Yaw = 0.0f;
            Pitch = 0.0f;
            updateCameraVectors();
        }
        // Constructor with scalar values
        Camera2(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(ZOOM)
        {
            Position = glm::vec3(posX, posY, posZ);
            Up = glm::vec3(upX, upY, upZ);
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 GetViewMatrix()
        {
            return glm::lookAt(Position, Position + Front, Up);
        }

        // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(Camera_Movement direction, float deltaTime)
        {
            float velocity = MovementSpeed * deltaTime;
            if (direction == Camera_Movement::FORWARD)
                Position += Front * velocity;
            if (direction == Camera_Movement::BACKWARD)
                Position -= Front * velocity;
            if (direction == Camera_Movement::LEFT)
                Position -= Right * velocity;
            if (direction == Camera_Movement::RIGHT)
                Position += Right * velocity;
        }

        // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
        {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            /*Yaw += xoffset;
            Pitch += yoffset;*/
            Yaw = xoffset;
            Pitch = yoffset;

            // Make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch)
            {
                Pitch = glm::clamp(Pitch, -89.0f, 89.0f);

                /* if (Pitch > 89.0f)
                     Pitch = 89.0f;
                 if (Pitch < -89.0f)
                     Pitch = -89.0f;*/
            }

            // Update Front, Right and Up Vectors using the updated Euler angles
            updateCameraVectors();
        }

        // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yoffset)
        {

            Fov -= yoffset;
            Fov = glm::clamp(Fov, 1.0f, 90.0f);

            /* if (Fov >= 1.0f && Fov <= 45.0f)
                 Fov -= yoffset;
             if (Fov <= 1.0f)
                 Fov = 1.0f;
             if (Fov >= 45.0f)
                 Fov = 45.0f;*/
        }

    private:
        // Calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors()
        {
            // Calculate the new Front vector
          

            //glm::vec3 Z = glm::normalize(-Position);
            glm::vec3 Z = glm::normalize(Front);
            // Also re-calculate the Right and Up vector
            glm::vec3 X = glm::normalize(glm::cross(Z, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            glm::vec3 Y = glm::normalize(glm::cross(X, Z));


            glm::mat4 XYZ{ 1.0f };
            XYZ[0] = glm::vec4(X, 0.0f);
            XYZ[1] = glm::vec4(Y, 0.0f);
            XYZ[2] = glm::vec4(Z, 0.0f);
            XYZ = glm::transpose(XYZ);

            glm::vec3 front = glm::vec3(XYZ * glm::vec4(Front, 0.0f));
            front = glm::rotateY(front, glm::radians(Yaw));
            front = glm::rotateX(front, glm::radians(Pitch));

            Front = glm::vec3(glm::inverse(XYZ) * glm::vec4(front, 0.0f));
            Up = glm::vec3(glm::inverse(XYZ) * glm::vec4(Y, 0.0f));


            //Front = glm::rotateY(Front, glm::radians(Yaw));
            //Front = glm::rotateX(Front, glm::radians(Pitch));




        }
    };
}
#endif