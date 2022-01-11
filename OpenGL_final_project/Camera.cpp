#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::vec3(0.0f, 0.0f, -1.0f);
        this->cameraRightDirection = glm::vec3(1.0f, 0.0f, 0.0f);
        this->pitch = 0;
        this->yaw = 0;
        //TODO - Update the rest of camera parameters

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        switch (direction)
        {
        case MOVE_FORWARD:
            cameraPosition += cameraFrontDirection * speed;
            cameraTarget += cameraFrontDirection * speed;
            break;

        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            cameraTarget -= cameraFrontDirection * speed;
            break;

        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            cameraTarget -= cameraRightDirection * speed;
            break;

        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            cameraTarget += cameraRightDirection * speed;
            break;

        default:
            break;
        }

    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        this->yaw += yaw;
        this->pitch += pitch;

        if (this->pitch > 89.0f)
        {
            this->pitch = 89.0f;
        }

        if (this->pitch < -89.0f)
        {
            this->pitch = -89.0f;
        }

        cameraTarget.x = cameraPosition.x + sin(glm::radians(this->yaw));
        cameraTarget.y = cameraPosition.y - sin(glm::radians(this->pitch));
        cameraTarget.z = cameraPosition.z - cos(glm::radians(this->yaw));

        this->cameraFrontDirection = glm::normalize(this->cameraTarget - this->cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
    }
    void Camera::changeCameraPosition(glm::vec3 cameraPosition) {
        this->cameraPosition = cameraPosition;
    }
    void Camera::changeCameraTarget(glm::vec3 cameraTarget) {
        this->cameraTarget = cameraTarget;
    }
    glm::vec3 Camera::getCameraTarget() {
        return this->cameraTarget;
    }
}