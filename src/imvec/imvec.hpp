#pragma once
#include "swizzles.h"
#include <math.h>
#include <vector>
// some goals:
// vector math (NOT as in 'std::vector' - more like GLSL's vec3)
// immutable! (explicitly disallow someVector.x = whatever, or someVector[1] = whatever)
// consise, but with a convenient interface
// no vtables! pack down into the minimum storage! (sizeof(vec<float,3>)==sizeof(float[3]) )
// consider thoughts like 'free your functions' and 'no inheritance', which are very interesting to me at the moment
// I would love to use c++20 modules, and perhaps I will someday!
namespace imvec {
    namespace _deets {
        // inline templated math ops, which
        // when used as a fn-ptr template arg
        // will (hopefully) decay into their nice inline forms
        // if I had a nice c++20 module :private fragment, I could hide all this...
        // but for now - hopefully no one bothers with _deets
        template<typename T>
        inline T add(const T& a, const T& b){
            return a+b;
        }
        template <typename T>
        inline T sub(const T& lhs, const T& rhs) {
            return lhs - rhs;
        }

        template <typename T>
        inline T div(const T& lhs, const T& rhs) {
            return lhs / rhs;
        }
        template <typename T>
        inline T mul(const T& lhs, const T& rhs) {
            return lhs * rhs;
        }
        template <typename T>
        inline T _min(const T& lhs, const T& rhs) {
            return lhs < rhs ? lhs : rhs;
        }
        template <typename T>
        inline T _max(const T& lhs, const T& rhs) {
            return lhs > rhs ? lhs : rhs;
        }
        template <typename T>
        inline bool _equal(const T& lhs, const T& rhs) {
            return lhs == rhs;
        }
        template <typename T>
        inline bool _and(const T& lhs, const T& rhs) {
            return lhs && rhs;
        }
        template <typename T, unsigned int N, T (*op)(const T& lhs, const T& rhs)>
        inline void perComponent(const T lhs[N], const T rhs[N], T dst[N]) {
            for (auto i = 0; i < N; i++) {
                dst[i] = op(lhs[i], rhs[i]);
            }
        }
        template <typename T, unsigned int N, T (*op)(const T& lhs, const T& rhs)>
        inline void perComponent(const T lhs[N], const T rhs, T dst[N]) {
            for (auto i = 0; i < N; i++) {
                dst[i] = op(lhs[i], rhs);
            }
        }
        
        template <typename T, unsigned int N, T (*op)(const T& lhs, const T& rhs)>
        inline T reduceComponent(const T v[N], T initialValue) {
            T tmp = initialValue;
            for (auto i = 0; i < N; i++) {
                tmp = op(tmp, v[i]);
            }
            return tmp;
        }
    }; // end _deets namespace
    namespace _secret_base {
    /// overview:
    // we want an interface like vec<float,3>
    // we want specializations and swizzles, like v.yzx() - which is only available
    // on a vec of length (at least) 3, but we also want common functions like +/*-, dot, sum, min,max etc.
    // recall that when you specialize a class - you must re-implement the entire thing - it is in no way related to the
    // "general" templated class - no members, no fns, nothing.
    // we could try to specialize specific functions - but it wouldn't be possible to do that with constructors -
    // for example vec<float,2>(x,y) and vec<float,3>(x,y,z) would be impossible (citation needed...)
    // so - we're gonna use a base class for common functionality, and use a little CRTP (class V)
    // do note that we will not be using ANY VIRTUAL FUNCTIONS because all we need is static-polymorphism here
    // the guideline of 'perfer composition over inheritance' suggests that I just cram a _vec inside the templated vec
    // but feels silly and doesn't solve my re-implementation of common interface problem
    // compiling this on godbolt suggests that there is no penalty of any kind for this use of non-virtual inheritance,
    // so Im gonna go with it. also... truly, a vec "is a" _vec...
    template <class V, typename T, unsigned short N>
    class _vec {
        protected:
        T data[N];
        // private default constructor
        // the plan is to call this from a safe place,
        // then pass the uninitialized data array to a helper that will fill it in
        _vec(){}

        // a private static helper to hide 2 lines of boilerplate per function:
        // do note that using these little inline functions as template arg function-pointers
        // is free - they decay directly to their inline versions! 
        template <T (*op)(const T& lhs, const T& rhs)>
        static inline V binOpHelper(const V& lhs, const V& rhs) {
            V tmp;
            _deets::perComponent<T,N,op>(lhs.data,rhs.data,tmp.data);
            return tmp;
        }
        template <T (*op)(const T& lhs, const T& rhs)>
        static inline V binOpHelper(const V& lhs, const T& rhs) {
            V tmp;
            _deets::perComponent<T,N,op>(lhs.data,rhs.data,tmp.data);
            return tmp;
        }
        public:
        _vec(T v[N]){
            for(auto i=0;i<N;++i){
                this->data[i] = v[i];
            }
        }
        // non-mutable indexing operator!
        const T& operator[](unsigned int index) const{
            // todo: throw exception on out-of-bounds?
            return data[index];
        }
        // I considered friend free operator fns here... 
        // overall they just seemed more trouble than their worth...
        V operator+(const V& rhs) const{
            return binOpHelper<_deets::add>(*static_cast<const V*>(this),rhs);
        }
        V operator-(const V& rhs) const{
            return binOpHelper<_deets::sub>(*static_cast<const V*>(this),rhs);
        }
        V operator*(const V& rhs) const{
            return binOpHelper<_deets::mul>(*static_cast<const V*>(this),rhs);
        }
        V operator/(const V& rhs) const{
            return binOpHelper<_deets::div>(*static_cast<const V*>(this),rhs);
        }
        // some scalar operators
        V operator/(const T& rhs) const{
            return binOpHelper<_deets::div>(*static_cast<const V*>(this),rhs);
        }
        V operator*(const T& rhs) const{
            return binOpHelper<_deets::mul>(*static_cast<const V*>(this),rhs);
        }
        // TODO its not common I think to add or subtract scalars
        // from vectors... remove for safety?
        V operator+(const T& rhs) const{
            return binOpHelper<_deets::add>(*static_cast<const V*>(this),rhs);
        }
        V operator-(const T& rhs) const{
            return binOpHelper<_deets::sub>(*static_cast<const V*>(this),rhs);
        }

        T sum(){
            return _deets::reduceComponent<T,N,_deets::add>(this->data,(T)0);
        }
        /// non-operator style math
        T dot(const V& rhs) const {
            return (*this + rhs).sum();
        }

        V max(const V& rhs) const {
            return binOpHelper<_deets::_max>(*static_cast<const V*>(this),rhs);
        }
        V min(const V& rhs) const {
            return binOpHelper<_deets::_min>(*static_cast<const V*>(this),rhs);
        }
        T maxComponent(){
            return _deets::reduceComponent<T,N,_deets::_max>(this->data,this->data[0]);
        }
        T minComponent(){
            return _deets::reduceComponent<T,N,_deets::_min>(this->data,this->data[0]);
        }
        T length(){
            const V& me = *static_cast<const V*>(this);
            T sumOSquares = _deets::reduceComponent<T,N,_deets::add>((me*me).data,(T)0);
            return (T)sqrt(sumOSquares);
        }
        // aka normalize
        V direction(){
            const V& me = *static_cast<const V*>(this);
            return me / me.length();
        }
        V normalize(){
            return this->direction();
        }
    };
    }; // end namespace _secret_base
    template <typename T, unsigned short N>
    class vec : public _secret_base::_vec<vec<T,N>,T,N> {
        protected:
        friend class _secret_base::_vec<vec<T,N>,T,N>;
        vec() : _secret_base::_vec<vec<T,N>,T,N>() {}
        public:
        vec(T v[N]) : _secret_base::_vec<vec<T,N>,T,N>(v) {}
    };
    
    // a single component getter
    #define _GETTER_PLZ(n,i) T n()const {return this->data[i];}
    // boilerplate for connecting up the default constructor
    #define _DEFAULT_CONSTRUCTOR(n) vec() : _secret_base::_vec<vec<T,n>,T,n>() {}
    // boilerplate for connecting up the array-based constructor
    #define _AR_CONSTRUCTOR(n) vec(T v[n]) : _secret_base::_vec<vec<T,n>,T,n>(v) {}
    // attempt to declare our base class as our friend, so that we may
    // call our own protected methods from it via crtp
    #define _FRIEND_DECL(n) friend class _secret_base::_vec<vec<T,n>,T,n>

    // the first specialization: vec2
    template <typename T>
    class vec<T,2> : public _secret_base::_vec<vec<T,2>,T,2> {
        protected:
        _DEFAULT_CONSTRUCTOR(2);
        _FRIEND_DECL(2);
        public:
        // constructors!
        _AR_CONSTRUCTOR(2);
        vec(T x, T y){
            this->data[0]=x;
            this->data[1]=y;
        }
        // single component getters
        _GETTER_PLZ(x,0);
        _GETTER_PLZ(y,1);

        // swizzles
        _ALL_SWIZZLES_2
    };
    template <typename T>
    class vec<T,3> : public _secret_base::_vec<vec<T,3>,T,3> {
        protected:
        _FRIEND_DECL(3);
        _DEFAULT_CONSTRUCTOR(3)
        public:
        // constructors!
        _AR_CONSTRUCTOR(3)
        
        vec(T x, T y, T z){
            this->data[0]=x;
            this->data[1]=y;
            this->data[2]=z;
        }
        vec(T x, vec<T,2> yz) : vec<T,4>(x,yz.x(),yz.y()){}
        vec(vec<T,2> xy,T z) : vec<T,4>(xy.x(),xy.y(),z){}
        // single component getters
        _GETTER_PLZ(x,0);
        _GETTER_PLZ(y,1);
        _GETTER_PLZ(z,2);
        // swizzles
        _ALL_SWIZZLES_3
    };
    template <typename T>
    class vec<T,4> : public _secret_base::_vec<vec<T,4>,T,4> {
        protected:
        _DEFAULT_CONSTRUCTOR(4)
        _FRIEND_DECL(4);
        public:
        // constructors!
        _AR_CONSTRUCTOR(4)
        vec(T x, T y, T z, T w){
            this->data[0]=x;
            this->data[1]=y;
            this->data[2]=z;
            this->data[3]=w;
        }
        vec(T x, vec<T,3> yzw) : vec<T,4>(x,yzw.x(),yzw.y(),yzw.z()){}
        vec(vec<T,3> xyz, T w) : vec<T,4>(xyz.x(),xyz.y(),xyz.z(),w){}
        vec(vec<T,2> xy,T z, T w) : vec<T,4>(xy.x(),xy.y(),z,w){}
        vec(T x, T y,vec<T,2> zw) : vec<T,4>(x,y,zw.x(),zw.y()){}
        // single component getters
        _GETTER_PLZ(x,0);
        _GETTER_PLZ(y,1);
        _GETTER_PLZ(z,2);
        _GETTER_PLZ(w,3);
        // swizzles
        _ALL_SWIZZLES_4
    };
}
