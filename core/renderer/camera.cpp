#include "camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cmath>

float lerp(const float &a, const float &b, const float &f)
{
    return a + f * (b - a);
}

glm::mat4 alignZAxisWithTarget(glm::vec3 targetDir, glm::vec3 upDir)
{
    // Ensure that the target direction is non-zero.
    if (length2(targetDir) == 0)
        targetDir = glm::vec3(0, 0, 1);

    // Ensure that the up direction is non-zero.
    if (length2(upDir) == 0)
        upDir = glm::vec3(0, 1, 0);

    // Check for degeneracies.  If the upDir and targetDir are parallel
    // or opposite, then compute a new, arbitrary up direction that is
    // not parallel or opposite to the targetDir.
    if (length2(cross(upDir, targetDir)) == 0)
    {
        upDir = cross(targetDir, glm::vec3(1, 0, 0));
        if (length2(upDir) == 0)
            upDir = cross(targetDir, glm::vec3(0, 0, 1));
    }

    // Compute the x-, y-, and z-axis vectors of the new coordinate system.
    const auto targetPerpDir = cross(upDir, targetDir);
    const auto targetUpDir = cross(targetDir, targetPerpDir);

    // Rotate the x-axis into targetPerpDir (row 0),
    // rotate the y-axis into targetUpDir   (row 1),
    // rotate the z-axis into targetDir     (row 2).
    glm::vec3 row[3];
    row[0] = normalize(targetPerpDir);
    row[1] = normalize(targetUpDir);
    row[2] = normalize(targetDir);

    return {row[0].x, row[0].y, row[0].z, 0, row[1].x, row[1].y, row[1].z, 0,
            row[2].x, row[2].y, row[2].z, 0, 0,        0,        0,        1};
}

namespace blackboard::renderer {
void Camera::setEyePoint(const glm::vec3 &eyePoint)
{
    mEyePoint = eyePoint;
    mModelViewCached = false;
}

void Camera::setViewDirection(const glm::vec3 &viewDirection)
{
    mViewDirection = normalize(viewDirection);
    mOrientation = glm::rotation(mViewDirection, glm::vec3(0, 0, -1));
    mModelViewCached = false;
}

void Camera::setOrientation(const glm::quat &orientation)
{
    mOrientation = glm::normalize(orientation);
    mViewDirection = glm::rotate(mOrientation, glm::vec3(0, 0, -1));
    mModelViewCached = false;
}

// Derived from math presented in http://paulbourke.net/miscellaneous/lens/
float Camera::getFocalLength() const
{
    return 1 / (tan(glm::radians(mFov) * 0.5f) * 2);
}

void Camera::setWorldUp(const glm::vec3 &worldUp)
{
    mWorldUp = normalize(worldUp);
    mOrientation = glm::toQuat(alignZAxisWithTarget(-mViewDirection, worldUp));
    mModelViewCached = false;
}

void Camera::lookAt(const glm::vec3 &target)
{
    mViewDirection = normalize(target - mEyePoint);
    mOrientation = glm::toQuat(alignZAxisWithTarget(-mViewDirection, mWorldUp));
    mPivotDistance = distance(target, mEyePoint);
    mModelViewCached = false;
}

void Camera::lookAt(const glm::vec3 &eyePoint, const glm::vec3 &target)
{
    mEyePoint = eyePoint;
    mViewDirection = normalize(target - mEyePoint);
    mOrientation = glm::toQuat(alignZAxisWithTarget(-mViewDirection, mWorldUp));
    mPivotDistance = distance(target, mEyePoint);
    mModelViewCached = false;
}

void Camera::lookAt(const glm::vec3 &eyePoint, const glm::vec3 &target, const glm::vec3 &aWorldUp)
{
    mEyePoint = eyePoint;
    mWorldUp = normalize(aWorldUp);
    mViewDirection = normalize(target - mEyePoint);
    mOrientation = glm::toQuat(alignZAxisWithTarget(-mViewDirection, mWorldUp));
    mPivotDistance = distance(target, mEyePoint);
    mModelViewCached = false;
}

void Camera::getFrustum(float *left, float *top, float *right, float *bottom, float *near,
                        float *far) const
{
    calcMatrices();

    *left = mFrustumLeft;
    *top = mFrustumTop;
    *right = mFrustumRight;
    *bottom = mFrustumBottom;
    *near = mNearClip;
    *far = mFarClip;
}

void Camera::getBillboardVectors(glm::vec3 *right, glm::vec3 *up) const
{
    *right = glm::vec3(glm::row(getViewMatrix(), 0));
    *up = glm::vec3(glm::row(getViewMatrix(), 1));
}

glm::vec2 Camera::worldToScreen(const glm::vec3 &worldCoord, float screenWidth,
                                float screenHeight) const
{
    const auto eyeCoord = getViewMatrix() * glm::vec4(worldCoord, 1);
    auto ndc = getProjectionMatrix() * eyeCoord;
    ndc.x /= ndc.w;
    ndc.y /= ndc.w;
    ndc.z /= ndc.w;

    return {(ndc.x + 1.0f) / 2.0f * screenWidth, (1.0f - (ndc.y + 1.0f) / 2.0f) * screenHeight};
}

glm::vec2 Camera::eyeToScreen(const glm::vec3 &eyeCoord, const glm::vec2 &screenSizePixels) const
{
    auto ndc = getProjectionMatrix() * glm::vec4(eyeCoord, 1);
    ndc.x /= ndc.w;
    ndc.y /= ndc.w;
    ndc.z /= ndc.w;

    return {(ndc.x + 1.0f) / 2.0f * screenSizePixels.x,
            (1.0f - (ndc.y + 1.0f) / 2.0f) * screenSizePixels.y};
}

float Camera::worldToEyeDepth(const glm::vec3 &worldCoord) const
{
    const auto &m = getViewMatrix();
    return m[0][2] * worldCoord.x + m[1][2] * worldCoord.y + m[2][2] * worldCoord.z + m[3][2];
}

glm::vec3 Camera::worldToNdc(const glm::vec3 &worldCoord) const
{
    const auto eye = getViewMatrix() * glm::vec4(worldCoord, 1);
    const auto unproj = getProjectionMatrix() * eye;
    return {unproj.x / unproj.w, unproj.y / unproj.w, unproj.z / unproj.w};
}

//float Camera::calcScreenArea( const Sphere &sphere, const glm::vec2 &screenSizePixels ) const
//{
//	Sphere camSpaceSphere( glm::vec3( getViewMatrix()*vec4(sphere.getCenter(), 1.0f) ), sphere.getRadius() );
//	return camSpaceSphere.calcProjectedArea( getFocalLength(), screenSizePixels );
//}

//void Camera::calcScreenProjection( const Sphere &sphere, const vec2 &screenSizePixels, vec2 *outCenter, vec2 *outAxisA, vec2 *outAxisB ) const
//{
//	auto toScreenPixels = [=] ( vec2 v, const vec2 &windowSize ) {
//		vec2 result = v;
//		result.x *= 1 / ( windowSize.x / windowSize.y );
//		result += vec2( 0.5f );
//		result *= windowSize;
//		return result;
//	};
//
//	Sphere camSpaceSphere( glm::vec3( getViewMatrix()*vec4(sphere.getCenter(), 1.0f) ), sphere.getRadius() );
//	vec2 center, axisA, axisB;
//	camSpaceSphere.calcProjection( getFocalLength(), &center, &axisA, &axisB );
//	if( outCenter )
//		*outCenter = toScreenPixels( center, screenSizePixels );//( center * vec2( invAspectRatio, 1 ) + vec2( 0.5f ) ) * screenSizePixels;
//	if( outAxisA )
//		*outAxisA = toScreenPixels( center + axisA * 0.5f, screenSizePixels ) - toScreenPixels( center - axisA * 0.5f, screenSizePixels );
//	if( outAxisB )
//		*outAxisB = toScreenPixels( center + axisB * 0.5f, screenSizePixels ) - toScreenPixels( center - axisB * 0.5f, screenSizePixels );
//}

void Camera::calcMatrices() const
{
    if (!mModelViewCached)
        calcViewMatrix();
    if (!mProjectionCached)
        calcProjection();
}

void Camera::calcViewMatrix() const
{
    mW = -normalize(mViewDirection);
    mU = glm::rotate(mOrientation, glm::vec3(1, 0, 0));
    mV = glm::rotate(mOrientation, glm::vec3(0, 1, 0));

    const glm::vec3 d{-dot(mEyePoint, mU), -dot(mEyePoint, mV), -dot(mEyePoint, mW)};

    glm::mat4 &m = mViewMatrix;
    m[0][0] = mU.x;
    m[1][0] = mU.y;
    m[2][0] = mU.z;
    m[3][0] = d.x;
    m[0][1] = mV.x;
    m[1][1] = mV.y;
    m[2][1] = mV.z;
    m[3][1] = d.y;
    m[0][2] = mW.x;
    m[1][2] = mW.y;
    m[2][2] = mW.z;
    m[3][2] = d.z;
    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;

    mModelViewCached = true;
    mInverseModelViewCached = false;
}

void Camera::calcInverseView() const
{
    if (!mModelViewCached)
        calcViewMatrix();

    mInverseModelViewMatrix = glm::inverse(mViewMatrix);
    mInverseModelViewCached = true;
}

void Camera::getClipCoordinates(float clipDist, float ratio, glm::vec3 *topLeft, glm::vec3 *topRight,
                                glm::vec3 *bottomLeft, glm::vec3 *bottomRight) const
{
    calcMatrices();

    const auto viewDirection = normalize(mViewDirection);

    *topLeft =
      mEyePoint + clipDist * viewDirection + ratio * (mFrustumTop * mV) + ratio * (mFrustumLeft * mU);
    *topRight =
      mEyePoint + clipDist * viewDirection + ratio * (mFrustumTop * mV) + ratio * (mFrustumRight * mU);
    *bottomLeft = mEyePoint + clipDist * viewDirection + ratio * (mFrustumBottom * mV) +
                  ratio * (mFrustumLeft * mU);
    *bottomRight = mEyePoint + clipDist * viewDirection + ratio * (mFrustumBottom * mV) +
                   ratio * (mFrustumRight * mU);
}

////////////////////////////////////////////////////////////////////////////////////////
// CameraPersp
// Creates a default camera resembling Maya Persp
CameraPersp::CameraPersp()
{
    lookAt(glm::vec3(28, 21, 28), glm::vec3(), glm::vec3(0, 1, 0));
    setPerspective(35, 1.3333f, 0.1f, 1000);
}

CameraPersp::CameraPersp(int pixelWidth, int pixelHeight, float fovDegrees)
{
    const float eyeX = pixelWidth / 2.0f;
    const float eyeY = pixelHeight / 2.0f;
    const float halfFov = 3.14159f * fovDegrees / 360.0f;
    const float theTan = tan(halfFov);
    const float dist = eyeY / theTan;
    const float nearDist = dist / 10.0f;    // near / far clip plane
    const float farDist = dist * 10.0f;
    const float aspect = pixelWidth / static_cast<float>(pixelHeight);

    setPerspective(fovDegrees, aspect, nearDist, farDist);
    lookAt(glm::vec3(eyeX, eyeY, dist), glm::vec3(eyeX, eyeY, 0.0f));
}

CameraPersp::CameraPersp(int pixelWidth, int pixelHeight, float fovDegrees, float nearPlane,
                         float farPlane)
{
    const float eyeX = pixelWidth / 2.0f;
    const float eyeY = pixelHeight / 2.0f;
    const float halfFov = 3.14159f * fovDegrees / 360.0f;
    const float theTan = tan(halfFov);
    float dist = eyeY / theTan;
    const float aspect = pixelWidth / static_cast<float>(pixelHeight);

    setPerspective(fovDegrees, aspect, nearPlane, farPlane);
    lookAt({eyeX, eyeY, dist}, {eyeX, eyeY, 0.0f});
}

void CameraPersp::setPerspective(float verticalFovDegrees, float aspectRatio, float nearPlane,
                                 float farPlane)
{
    mFov = verticalFovDegrees;
    mAspectRatio = aspectRatio;
    mNearClip = nearPlane;
    mFarClip = farPlane;

    mProjectionCached = false;
}

//Ray CameraPersp::calcRay( float uPos, float vPos, float imagePlaneAspectRatio ) const
//{
//	calcMatrices();
//
//	float s = ( uPos - 0.5f + 0.5f * mLensShift.x ) * imagePlaneAspectRatio;
//	float t = ( vPos - 0.5f + 0.5f * mLensShift.y );
//	float viewDistance = imagePlaneAspectRatio / math<float>::abs( mFrustumRight - mFrustumLeft ) * mNearClip;
//	return Ray( mEyePoint, normalize( mU * s + mV * t - ( mW * viewDistance ) ) );
//}

void CameraPersp::calcProjection() const
{
    mFrustumTop = mNearClip * tan(static_cast<float>(M_PI) / 180.0f * mFov * 0.5f);
    mFrustumBottom = -mFrustumTop;
    mFrustumRight = mFrustumTop * mAspectRatio;
    mFrustumLeft = -mFrustumRight;

    // perform lens shift
    if (mLensShift.y != 0.0f)
    {
        mFrustumTop = lerp(0.0f, 2.0f * mFrustumTop, 0.5f + 0.5f * mLensShift.y);
        mFrustumBottom = lerp(2.0f * mFrustumBottom, 0.0f, 0.5f + 0.5f * mLensShift.y);
    }

    if (mLensShift.x != 0.0f)
    {
        mFrustumRight = lerp(2.0f * mFrustumRight, 0.0f, 0.5f - 0.5f * mLensShift.x);
        mFrustumLeft = lerp(0.0f, 2.0f * mFrustumLeft, 0.5f - 0.5f * mLensShift.x);
    }

    auto &p = mProjectionMatrix;
    p[0][0] = 2.0f * mNearClip / (mFrustumRight - mFrustumLeft);
    p[1][0] = 0.0f;
    p[2][0] = (mFrustumRight + mFrustumLeft) / (mFrustumRight - mFrustumLeft);
    p[3][0] = 0.0f;

    p[0][1] = 0.0f;
    p[1][1] = 2.0f * mNearClip / (mFrustumTop - mFrustumBottom);
    p[2][1] = (mFrustumTop + mFrustumBottom) / (mFrustumTop - mFrustumBottom);
    p[3][1] = 0.0f;

    p[0][2] = 0.0f;
    p[1][2] = 0.0f;
    p[2][2] = -(mFarClip + mNearClip) / (mFarClip - mNearClip);
    p[3][2] = -2.0f * mFarClip * mNearClip / (mFarClip - mNearClip);

    p[0][3] = 0.0f;
    p[1][3] = 0.0f;
    p[2][3] = -1.0f;
    p[3][3] = 0.0f;

    glm::mat4 &m = mInverseProjectionMatrix;
    m[0][0] = (mFrustumRight - mFrustumLeft) / (2.0f * mNearClip);
    m[1][0] = 0.0f;
    m[2][0] = 0.0f;
    m[3][0] = (mFrustumRight + mFrustumLeft) / (2.0f * mNearClip);

    m[0][1] = 0.0f;
    m[1][1] = (mFrustumTop - mFrustumBottom) / (2.0f * mNearClip);
    m[2][1] = 0.0f;
    m[3][1] = (mFrustumTop + mFrustumBottom) / (2.0f * mNearClip);

    m[0][2] = 0.0f;
    m[1][2] = 0.0f;
    m[2][2] = 0.0f;
    m[3][2] = -1.0f;

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = -(mFarClip - mNearClip) / (2.0f * mFarClip * mNearClip);
    m[3][3] = (mFarClip + mNearClip) / (2.0f * mFarClip * mNearClip);

    mProjectionCached = true;
}

void CameraPersp::setLensShift(float horizontal, float vertical)
{
    mLensShift.x = horizontal;
    mLensShift.y = vertical;

    mProjectionCached = false;
}
}    // namespace blackboard::renderer
