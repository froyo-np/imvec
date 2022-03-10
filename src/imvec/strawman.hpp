#include <vector>
// DONT USE THIS FILE - ITS JUST A THOUGHT!
// USE imvec.hpp instead
// for giggles, I've re-implemented a small portion
// of the vec class based on the idea of sharing code
// by way of vec<T,2> extends vec<T,1>
// doing this immediately runs into a problem: we cannot
// share the storage and have it be a static size - 
// if vec<T,2> declared data as T data[2], that would just hide
// the base class's T data[1].
// instead, we try dynamic storage - std::vector<T>
// this does work, but if you check out on godbolt.org, 
// we pay a hefty price. even our free-function operator+
// cant get optimized out at -O3 by clang13, wheras the imvec.hpp
// style gets almost completely constant-folded out.
// what does it get us?
// 1. more compact - no repetition of common swizzles or accessors
// 2. sorta nice cascading specialized constructors
// can this idea be fixed with crtp style thoughts of the 
// approach in imvec.hpp?

namespace __strawman_vector {
template <typename T, unsigned short N>
class _vec {
    protected:
    _vec(){}
    vector<T> data;
    public:
    const T& operator[](unsigned int index) const{
        return this->data[index];
    }
    _vec(T v[N]){
        for(auto i = 0;i < N;++i){
            this->data.push_back(v[i]);
        }
    }
};
template <typename T, unsigned short N>
class vec : public _vec<T,N>{
    public:
    vec(T v[N]) : _vec<T,N>(v){}
};

template <typename T>
class vec<T,1> : public _vec<T,1> {
    protected:
    vec() : _vec<T,1>(){}
    public:
    vec(T x){this->data.push_back(x);}
    vec(T v[1]) : _vec<T,1>(v){}
    T x()const {return this->data[0];}
};
template <typename T>
class vec<T,2> : public vec<T,1> {
    protected:
    vec() : vec<T,1>() {}
    public:
    vec(T x, T y): vec<T,1>(x){this->data.push_back(y);}
    vec(T v[2]) : vec<T,1>(), _vec<T,2>(v){}
};

template <typename T>
class vec<T,3> : public vec<T,2> {
    public:
    vec(T v[3]) : vec<T,2>() {
        for(unsigned int i=0;i<3;++i){
            this->data.push_back(v[i]);
        }
    }
    vec(T x, T y, T z): vec<T,2>(x,y){this->data.push_back(z);}
};
template<typename T, unsigned short N>
vec<T,N> operator+(const vec<T,N>& lhs, const vec<T,N>& rhs){
    T tmp[N];
    for(unsigned int i = 0;i < N;++i){
        tmp[i] = lhs[i]+rhs[i];
    }
    vec<T,N> ret(tmp);
    return ret;
}
};