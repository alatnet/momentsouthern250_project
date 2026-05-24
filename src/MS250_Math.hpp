#pragma once
//#include "MS250_FBX.h"
#include "glm/gtc/type_ptr.hpp"
#ifndef MS250_MATH_H
#define MS250_MATH_H

#include <string>

#ifdef MS250_USE_GLM
    #include <glm/gtc/quaternion.hpp>
    #include "glm/ext/matrix_float4x4.hpp"
    #include "glm/ext/vector_float3.hpp"
    #include "glm/fwd.hpp"
#else
    #include <algorithm>
    #include <cfloat>
    #include <cmath>
    #include <stddef.h>
    #include <stdint.h>
#endif

namespace MS250 {
    namespace Math {
        #ifdef MS250_USE_GLM
            typedef glm::quat Quat;
            typedef glm::vec3 Vector3;
            typedef glm::vec2 Vector2;
            typedef glm::mat4x4 Matrix;

            #define MS250_MATH_TOVAL_MATRIX(x) glm::value_ptr(x)
            #define MS250_MATH_TOVAL_VECTOR3(x) glm::value_ptr(x)
            #define MS250_MATH_TOVAL_VECTOR2(x) glm::value_ptr(x)
            #define MS250_MATH_TOVAL_QUAT(x) glm::value_ptr(x)

            #define MS250_MATH_LERP_VECTOR3(a,b,t) glm::mix(a, b, t)
            #define MS250_MATH_LERP_VECTOR2(a,b,t) glm::mix(a, b, t)
            #define MS250_MATH_LERP_QUAT(a,b,t) glm::mix(a, b, t)

            #define MS250_MATH_TRS_MATRIX(pos, rot, scale) \
            glm::translate( \
                glm::scale( \
                    glm::mat4x4(1.0f), scale \
                ) * glm::mat4_cast(rot), \
                pos \
            )

            #define MS250_MATH_INVERSE_MATRIX(m) glm::inverse(m)
            #define MS250_MATH_TRANSPOSE_MATRIX(m) glm::transpose(m)

            #define MS250_MATH_NORMALIZE_VECTOR3(x, y, z) glm::normalize(glm::vec3{x,y,z})
            #define MS250_MATH_NORMALIZE_VECTOR2(x, y) glm::normalize(glm::vec2{x,y})

            #define MS250_MATH_DOT_QUAT(a, b) glm::dot(a, b)

            #define MS250_MATH_TO_MATRIX(f) glm::make_mat4(f);

            std::string matrixToString(Matrix m, std::string padding);

            // std::string matrixToString(Matrix m_in, std::string padding){
            //     float * m = glm::value_ptr(m_in);
            //     std::string ret = "";

            //     /*
            //     🭽0, 1, 2, 3🭾
            //     🭰0, 1, 2, 3🭵
            //     🭰0, 1, 2, 3🭵
            //     🭼0, 1, 2, 3🭿
            //     */

            //     ret += padding + "🭽" + std::to_string(m[0]) + ", " + std::to_string(m[1]) + ", " + std::to_string(m[2]) + ", " + std::to_string(m[3]) + "🭾\n";
            //     ret += padding + "🭰" + std::to_string(m[4]) + ", " + std::to_string(m[5]) + ", " + std::to_string(m[6]) + ", " + std::to_string(m[7]) + "🭵\n";
            //     ret += padding + "🭰" + std::to_string(m[8]) + ", " + std::to_string(m[9]) + ", " + std::to_string(m[10]) + ", " + std::to_string(m[11]) + "🭵\n";
            //     ret += padding + "🭼" + std::to_string(m[12]) + ", " + std::to_string(m[13]) + ", " + std::to_string(m[14]) + ", " + std::to_string(m[15]) + "🭿\n";

            //     return ret;
            // }
        #else
            struct Quat {
                union {
                    struct {
                        float x;
                        float y;
                        float z;
                        float w;
                    };
                    float q[4];
                };

                float dot(Quat a){ return this->x*a.x + this->y*a.y + this->z*a.z + this->w*a.w; }
                Quat multAdd(Quat a, float b){ return { this->x + a.x * b, this->y + a.y * b, this->z + a.z * b, this->w + a.w * b }; }
                Quat inverse(Quat a){ return (-(*this))/(a.x * a.x + a.y*a.y + a.z*a.x + a.w*a.w); }

                Quat inverseNormalized(){ return { -(*this).x, -(*this).y, -(*this).z, (*this).w }; }
                float length(){ return std::sqrt((*this).x*(*this).x + (*this).y*(*this).y + (*this).z*(*this).z + (*this).w*(*this).w); }
                Quat normalize(){
                    float v = this->length();
                    v = v >= FLT_MIN ? 1.0f / v : 0.0f;
                    return { (*this).x * v, (*this).y *v, (*this).z * v, (*this).w * v};
                }

                static Quat inverseNormalized(Quat a){ return { -a.x, -a.y, -a.z, a.w }; }
                static float length(Quat a){ return std::sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w); }
                static Quat normalize(Quat a){
                    float v = length(a);
                    v = v >= FLT_MIN ? 1.0f / v : 0.0f;
                    return { a.x * v, a.y *v, a.z * v, a.w * v};
                }

                bool operator==(Quat a){
                    for(int i=0; i<4; i++) if (a.q[i] != this->q[i]) return false;
                    return true;
                }
                bool operator!=(Quat a){ return !(*this == a); };
                Quat operator+(Quat a){ return {this->x + a.x, this->y + a.y, this->z + a.z, this->w + a.w}; }
                Quat operator-(Quat a){ return {this->x - a.x, this->y - a.y, this->z - a.z, this->w - a.w}; }
                Quat operator-(){ return {-(this->x), -(this->y), -(this->z), -(this->w)}; }
                Quat operator*(float a){ return {this->x * a, this->y * a, this->z * a, this->w * a}; }
                Quat operator/(float a){
                    float v = 1.0f / a;
                    return *this * v;
                }

                Quat lerp(Quat a, float t) {
                    // Ensure shortest path
                    float dot = this->x*a.x + this->y*a.y + this->z*a.z + this->w*a.w;
                    Quat a_copy = a;
                    if (dot < 0.0f) {
                        a_copy.x = -a.x;
                        a_copy.y = -a.y;
                        a_copy.z = -a.z;
                        a_copy.w = -a.w;
                    }

                    // Linear interpolation
                    Quat result;
                    result.x = a.x + t * (a_copy.x - a.x);
                    result.y = a.y + t * (a_copy.y - a.y);
                    result.z = a.z + t * (a_copy.z - a.z);
                    result.w = a.w + t * (a_copy.w - a.w);

                    // Normalize
                    float len = result.length();
                    
                    if (len > 0.0f) {
                        float invLen = 1.0f / len;
                        result.x *= invLen;
                        result.y *= invLen;
                        result.z *= invLen;
                        result.w *= invLen;
                    }
                    
                    return result;
                }
            };

            struct Vector3 {
                union {
                    struct {
                        float x;
                        float y;
                        float z;
                    };
                    float v[3];
                };

                Vector3 operator+(Vector3 a){
                    return {this->x + a.x, this->y + a.y, this->z + a.z};
                }
                Vector3 operator-(Vector3 a){
                    return {this->x - a.x, this->y - a.y, this->z - a.z};
                }
                Vector3 operator*(Vector3 a){
                    return {this->x * a.x, this->y * a.y, this->z * a.z};
                }
                Vector3 operator+(float a){
                    return {this->x + a, this->y + a, this->z + a};
                }
                Vector3 operator-(float a){
                    return {this->x - a, this->y - a, this->z - a};
                }
                Vector3 operator*(float a){
                    return {this->x * a, this->y * a, this->z * a};
                }
                Vector3 operator/(float a){
                    return {this->x / a, this->y / a, this->z / a};
                }

                bool operator==(Vector3 a){
                    for(int i=0;i<3;i++) if (a.v[i] != this->v[i]) return false;
                    return true;
                }
                bool operator!=(Vector3 a){ return !(*this == a); }

                Vector3 min(Vector3 a){
                    return {
                        std::min(this->x, a.x),
                        std::min(this->y, a.y),
                        std::min(this->z, a.z)
                    };
                }

                Vector3 max(Vector3 a){
                    return {
                        std::max(this->x, a.x),
                        std::max(this->y, a.y),
                        std::max(this->z, a.z)
                    };
                }

                Vector3 lerp(Vector3 a, float time){
                    return {
                        this->x*(1.0f-time) + a.x*time,
                        this->y*(1.0f-time) + a.y*time,
                        this->z*(1.0f-time) + a.z*time
                    };
                }

                Vector3 normalize(){
                    float m = std::sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
                    return (*this)/m;
                }

                static Vector3 min(Vector3 a, Vector3 b){
                    return {
                        std::min(a.x, b.x),
                        std::min(a.y, b.y),
                        std::min(a.z, b.z)
                    };
                }

                static Vector3 max(Vector3 a, Vector3 b){
                    return {
                        std::max(a.x, b.x),
                        std::max(a.y, b.y),
                        std::max(a.z, b.z)
                    };
                }
            };

            struct Vector2 {
                union {
                    struct {
                        float x;
                        float y;
                    };
                    float v[2];
                };

                Vector2 operator+(Vector2 a){
                    return {this->x + a.x, this->y + a.y};
                }
                Vector2 operator-(Vector2 a){
                    return {this->x - a.x, this->y - a.y};
                }
                Vector2 operator*(Vector2 a){
                    return {this->x * a.x, this->y * a.y};
                }
                Vector2 operator+(float a){
                    return {this->x + a, this->y + a};
                }
                Vector2 operator-(float a){
                    return {this->x - a, this->y - a};
                }
                Vector2 operator*(float a){
                    return {this->x * a, this->y * a};
                }

                bool operator==(Vector2 a){
                    for(int i=0;i<3;i++) if (a.v[i] != this->v[i]) return false;
                    return true;
                }
                bool operator!=(Vector2 a){ return !(*this == a); }

                Vector2 operator/(float a){
                    return {this->x / a, this->y / a};
                }

                Vector2 min(Vector2 a){
                    return {
                        std::min(this->x, a.x),
                        std::min(this->y, a.y)
                    };
                }

                Vector2 max(Vector2 a){
                    return {
                        std::max(this->x, a.x),
                        std::max(this->y, a.y)
                    };
                }

                Vector2 lerp(Vector2 a, float time){
                    return {
                        this->x*(1.0f-time) + a.x*time,
                        this->y*(1.0f-time) + a.y*time
                    };
                }

                static Vector2 min(Vector2 a, Vector2 b){
                    return {
                        std::min(a.x, b.x),
                        std::min(a.y, b.y)
                    };
                }

                static Vector2 max(Vector2 a, Vector2 b){
                    return {
                        std::max(a.x, b.x),
                        std::max(a.y, b.y)
                    };
                }
                
            };

            struct Matrix {
                union {
                    float m[16];
                    struct {
                        float
                            m11, m21, m31, m41,
                            m12, m22, m32, m42,
                            m13, m23, m33, m43,
                            m14, m24, m34, m44;
                    };
                };

                Matrix Inverse(){
                    Matrix ret = {0.0f};
                    double inv[16], det;
                    int i;

                    inv[0] = m[5]  * m[10] * m[15] - 
                            m[5]  * m[11] * m[14] - 
                            m[9]  * m[6]  * m[15] + 
                            m[9]  * m[7]  * m[14] +
                            m[13] * m[6]  * m[11] - 
                            m[13] * m[7]  * m[10];

                    inv[4] = -m[4]  * m[10] * m[15] + 
                            m[4]  * m[11] * m[14] + 
                            m[8]  * m[6]  * m[15] - 
                            m[8]  * m[7]  * m[14] - 
                            m[12] * m[6]  * m[11] + 
                            m[12] * m[7]  * m[10];

                    inv[8] = m[4]  * m[9] * m[15] - 
                            m[4]  * m[11] * m[13] - 
                            m[8]  * m[5] * m[15] + 
                            m[8]  * m[7] * m[13] + 
                            m[12] * m[5] * m[11] - 
                            m[12] * m[7] * m[9];

                    inv[12] = -m[4]  * m[9] * m[14] + 
                            m[4]  * m[10] * m[13] +
                            m[8]  * m[5] * m[14] - 
                            m[8]  * m[6] * m[13] - 
                            m[12] * m[5] * m[10] + 
                            m[12] * m[6] * m[9];

                    inv[1] = -m[1]  * m[10] * m[15] + 
                            m[1]  * m[11] * m[14] + 
                            m[9]  * m[2] * m[15] - 
                            m[9]  * m[3] * m[14] - 
                            m[13] * m[2] * m[11] + 
                            m[13] * m[3] * m[10];

                    inv[5] = m[0]  * m[10] * m[15] - 
                            m[0]  * m[11] * m[14] - 
                            m[8]  * m[2] * m[15] + 
                            m[8]  * m[3] * m[14] + 
                            m[12] * m[2] * m[11] - 
                            m[12] * m[3] * m[10];

                    inv[9] = -m[0]  * m[9] * m[15] + 
                            m[0]  * m[11] * m[13] + 
                            m[8]  * m[1] * m[15] - 
                            m[8]  * m[3] * m[13] - 
                            m[12] * m[1] * m[11] + 
                            m[12] * m[3] * m[9];

                    inv[13] = m[0]  * m[9] * m[14] - 
                            m[0]  * m[10] * m[13] - 
                            m[8]  * m[1] * m[14] + 
                            m[8]  * m[2] * m[13] + 
                            m[12] * m[1] * m[10] - 
                            m[12] * m[2] * m[9];

                    inv[2] = m[1]  * m[6] * m[15] - 
                            m[1]  * m[7] * m[14] - 
                            m[5]  * m[2] * m[15] + 
                            m[5]  * m[3] * m[14] + 
                            m[13] * m[2] * m[7] - 
                            m[13] * m[3] * m[6];

                    inv[6] = -m[0]  * m[6] * m[15] + 
                            m[0]  * m[7] * m[14] + 
                            m[4]  * m[2] * m[15] - 
                            m[4]  * m[3] * m[14] - 
                            m[12] * m[2] * m[7] + 
                            m[12] * m[3] * m[6];

                    inv[10] = m[0]  * m[5] * m[15] - 
                            m[0]  * m[7] * m[13] - 
                            m[4]  * m[1] * m[15] + 
                            m[4]  * m[3] * m[13] + 
                            m[12] * m[1] * m[7] - 
                            m[12] * m[3] * m[5];

                    inv[14] = -m[0]  * m[5] * m[14] + 
                            m[0]  * m[6] * m[13] + 
                            m[4]  * m[1] * m[14] - 
                            m[4]  * m[2] * m[13] - 
                            m[12] * m[1] * m[6] + 
                            m[12] * m[2] * m[5];

                    inv[3] = -m[1] * m[6] * m[11] + 
                            m[1] * m[7] * m[10] + 
                            m[5] * m[2] * m[11] - 
                            m[5] * m[3] * m[10] - 
                            m[9] * m[2] * m[7] + 
                            m[9] * m[3] * m[6];

                    inv[7] = m[0] * m[6] * m[11] - 
                            m[0] * m[7] * m[10] - 
                            m[4] * m[2] * m[11] + 
                            m[4] * m[3] * m[10] + 
                            m[8] * m[2] * m[7] - 
                            m[8] * m[3] * m[6];

                    inv[11] = -m[0] * m[5] * m[11] + 
                            m[0] * m[7] * m[9] + 
                            m[4] * m[1] * m[11] - 
                            m[4] * m[3] * m[9] - 
                            m[8] * m[1] * m[7] + 
                            m[8] * m[3] * m[5];

                    inv[15] = m[0] * m[5] * m[10] - 
                            m[0] * m[6] * m[9] - 
                            m[4] * m[1] * m[10] + 
                            m[4] * m[2] * m[9] + 
                            m[8] * m[1] * m[6] - 
                            m[8] * m[2] * m[5];

                    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

                    if (det == 0)
                        return ret;

                    det = 1.0 / det;

                    for (i = 0; i < 16; i++)
                        ret.m[i] = inv[i] * det;

                    return ret;
                }

                Matrix Transpose(){
                    /*return {
                        m44, m12, m13, m14,
                        m21, m33, m23, m24,
                        m31, m32, m22, m34,
                        m41, m42, m43, m11,
                    };*/

                    return {
                        m11, m12, m13, m14,
                        m21, m22, m23, m24,
                        m31, m32, m33, m34,
                        m41, m42, m43, m44
                    };
                }

                Matrix TRS(Vector3 translate, Quat rotation, Vector3 scale){
                    Matrix ret = (*this);

                    ret.m11 *= scale.x;
                    ret.m12 *= scale.x;
                    ret.m13 *= scale.x;

                    ret.m21 *= scale.y;
                    ret.m22 *= scale.y;
                    ret.m23 *= scale.y;

                    ret.m31 *= scale.z;
                    ret.m32 *= scale.z;
                    ret.m33 *= scale.z;

                    float x = rotation.x, y = rotation.y, z = rotation.z, w = rotation.w;
                    float x2 = x + x, y2 = y + y, z2 = z + z;
                    float xx = x * x2, xy = x * y2, xz = x * z2;
                    float yy = y * y2, yz = y * z2, zz = z * z2;
                    float wx = w * x2, wy = w * y2, wz = w * z2;

                    ret.m11 *= 1.0f - (yy + zz);
                    ret.m12 *= xy - wz;
                    ret.m13 *= xz + wy;

                    ret.m21 *= xy+wz;
                    ret.m22 *= 1.0f - (xx + zz);
                    ret.m23 *= yz - wx;

                    ret.m31 *= xz - wy;
                    ret.m32 *= yz - wx;
                    ret.m33 *= 1.0f - (xx + yy);

                    ret.m41 *= translate.x;
                    ret.m42 *= translate.y;
                    ret.m43 *= translate.z;

                    return ret;
                }

                bool operator==(Matrix a){
                    for (int i=0;i<16;i++) if(a.m[i] != m[i]) return false;
                    return true;
                }

                Matrix operator=(Matrix a){
                    for(int i=0;i<16;i++) m[i] = a.m[i];
                    return *this;
                }

                Matrix operator*(Matrix a){
                    return {
                        m11 * a.m11 + m21 * a.m12 + m31 * a.m13 + m41 * a.m14,
                        m11 * a.m21 + m21 * a.m22 + m31 * a.m23 + m41 * a.m24,
                        m11 * a.m31 + m21 * a.m32 + m31 * a.m33 + m41 * a.m34,
                        m11 * a.m41 + m21 * a.m42 + m31 * a.m43 + m41 * a.m44,
                        m12 * a.m11 + m22 * a.m12 + m32 * a.m13 + m42 * a.m14,
                        m12 * a.m21 + m22 * a.m22 + m32 * a.m23 + m42 * a.m24,
                        m12 * a.m31 + m22 * a.m32 + m32 * a.m33 + m42 * a.m34,
                        m12 * a.m41 + m22 * a.m42 + m32 * a.m43 + m42 * a.m44,
                        m13 * a.m11 + m23 * a.m12 + m33 * a.m13 + m43 * a.m14,
                        m13 * a.m21 + m23 * a.m22 + m33 * a.m23 + m43 * a.m24,
                        m13 * a.m31 + m23 * a.m32 + m33 * a.m33 + m43 * a.m34,
                        m13 * a.m41 + m23 * a.m42 + m33 * a.m43 + m43 * a.m44,
                        m14 * a.m11 + m24 * a.m12 + m34 * a.m13 + m44 * a.m14,
                        m14 * a.m21 + m24 * a.m22 + m34 * a.m23 + m44 * a.m24,
                        m14 * a.m31 + m24 * a.m32 + m34 * a.m33 + m44 * a.m34,
                        m14 * a.m41 + m24 * a.m42 + m34 * a.m43 + m44 * a.m44
                    };
                }

                std::string toString(std::string padding = "-"){
                    std::string ret = "";

                    /*
                    🭽0, 1, 2, 3🭾
                    🭰0, 1, 2, 3🭵
                    🭰0, 1, 2, 3🭵
                    🭼0, 1, 2, 3🭿
                    */

                    ret += padding + "🭽" + std::to_string(m[0]) + ", " + std::to_string(m[1]) + ", " + std::to_string(m[2]) + ", " + std::to_string(m[3]) + "🭾\n";
                    ret += padding + "🭰" + std::to_string(m[4]) + ", " + std::to_string(m[5]) + ", " + std::to_string(m[6]) + ", " + std::to_string(m[7]) + "🭵\n";
                    ret += padding + "🭰" + std::to_string(m[8]) + ", " + std::to_string(m[9]) + ", " + std::to_string(m[10]) + ", " + std::to_string(m[11]) + "🭵\n";
                    ret += padding + "🭼" + std::to_string(m[12]) + ", " + std::to_string(m[13]) + ", " + std::to_string(m[14]) + ", " + std::to_string(m[15]) + "🭿\n";

                    return ret;
                }
            };

            Matrix TRS(Vector3 translate, Quat rotation, Vector3 scale);
            
            Vector2 normalizeVec2(float x, float y);
            Vector3 normalizeVec3(Vector3 a);
            Vector3 normalizeVec3(float x, float y, float z);

            #define MS250_MATH_TOVAL_MATRIX(x) x.m
            #define MS250_MATH_TOVAL_VECTOR3(x) x.v
            #define MS250_MATH_TOVAL_VECTOR2(x) x.v
            #define MS250_MATH_TOVAL_QUAT(x) x.q

            #define MS250_MATH_LERP_VECTOR3(a,b,t) a.lerp(b, t)
            #define MS250_MATH_LERP_VECTOR2(a,b,t) a.lerp(b, t)
            #define MS250_MATH_LERP_QUAT(a,b,t) a.lerp(b, t)

            #define MS250_MATH_TRS_MATRIX(pos, rot, scale) MS250::Math::TRS(pos, rot, scale)

            #define MS250_MATH_INVERSE_MATRIX(m) m.Inverse()
            #define MS250_MATH_TRANSPOSE_MATRIX(m) m.Transpose()

            #define MS250_MATH_NORMALIZE_VECTOR3(x,y,z) MS250::Math::normalizeVec3(x,y,z)
            #define MS250_MATH_NORMALIZE_VECTOR2(x,y,z) MS250::Math::normalizeVec3(x,y,z)

            #define MS250_MATH_DOT_QUAT(a, b) a.dot(b)

            #define MS250_MATH_TO_MATRIX(f) MS250::Math::makeMatrix(f);

            std::string matrixToString(Matrix m);
            Matrix makeMatrix(float f[16]);
            //std::string matrixToString(Matrix m, std::string padding){ return m.toString(padding); };
        #endif
    }
}

#endif