#include "MS250_Math.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace MS250 {
    namespace Math{

        #ifdef MS250_USE_GLM
            std::string matrixToString(Matrix m_in, std::string padding){
                float * m = glm::value_ptr(m_in);
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
        #else
            Matrix TRS(Vector3 translate, Quat rotation, Vector3 scale){
                Matrix ret = {0.0f};
                ret.m11 = ret.m22 = ret.m33 = ret.m44 = 1.0f;

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

            Vector2 normalizeVec2(float x, float y){
                float m = std::sqrt(x*x + y*y);
                Vector2 ret = {x,y};
                return ret/m;
            }

            Vector2 normalizeVec2(Vector2 a){
                float m = std::sqrt(a.x*a.x + a.y*a.y);
                return a/m;
            }

            Vector3 normalizeVec3(Vector3 a){
                float m = std::sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
                return a/m;
            }
            
            Vector3 normalizeVec3(float x, float y, float z){
                float m = std::sqrt(x*x + y*y + z*z);
                Vector3 ret = {x,y,z};
                return ret/m;
            }

            std::string matrixToString(Matrix m, std::string padding){ return m.toString(padding); };
            Matrix makeMatrix(float f[16]) {
                Matrix ret = {
                    f[0], f[1], f[2], f[3],
                    f[4], f[5], f[6], f[7],
                    f[8], f[9], f[10], f[11],
                    f[12], f[13], f[14], f[15],
                };
                return ret;
            }
        #endif

    }
}