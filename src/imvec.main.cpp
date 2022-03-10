#include "imvec/imvec.hpp"

using namespace imvec;
int main(){
    // test all the things?
    float zero[3] = {0,0,0};
    float oneTwoThree[3] = {1,2,3};
    vec<float,3> test(oneTwoThree);
    //vec<float,3> abc(1,2,3);
    auto b = test + test;
    auto c = test.dot(b);
    auto d = test * test;
    auto e = test / test.zxy();
    
    vec<float,4> rgba(3.0f,2.,e.y(),-4);
    vec<float,2> pos = rgba.wz()*rgba.xz();
    vec<float,2> cpy(pos); // default copy constructor?
    return (int)cpy.length();


}