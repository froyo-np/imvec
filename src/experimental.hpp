#include "swizzles.h"
// lets try to combine the idea that vec<T,2> extends vec<T,1>
// with static storage in the concrete class.
// I can think of no way to put static storage in any base class with this inheritance method
// without v<t,2> hiding the storage of its base: v<t,1>, hence storage in the concrete class
// this way does actually work! its a bit confusing, and requires a bunch of friending of
// base classes so that the crtp base can access protected members of its concrete derived class.
// although it lets us share x() y() z() accessors (vec<t,2> just inheirits x() and adds in a y())
// its actually a fair bit less consise and more confusing due to the distinction between the concrete size (cN) and the
// 'at least' lexical size...
// we can now not share constructors - because only the most derived type has any storage
// it compiles and optimizes well.
// we could also share swizzles via inheiritance (put them in _vec<..> not vec<T,{1,2,3,4}>)... but it would actually be pretty confusing...
// I think techhnically the macros could expand fewer lines in this world, and the compile might actually
// go just a bit faster - but probably not worth it!
namespace fancy {
    template <class V, typename T, unsigned short N>
    class __vec {
        protected:
        public:
        const T& operator[](unsigned int i) const {
            return static_cast<const V*>(this)->data[i];
        }
        V operator+(const V& rhs){
            const V& me = *static_cast<const V*>(this);
            T tmp[N];
            for(auto i = 0;i < N;++i){
                tmp[i]=me[i]+rhs[i];
            }
            return V(tmp);
        }
    };
    template <class V, typename T, unsigned short N, unsigned short cN>
    class _vec : public __vec<V,T,N> {
        protected:
        friend class __vec<V,T,N>;
    };
    template <class V, typename T, unsigned short cN>
    class _vec<V,T,1, cN> : public __vec<V,T,cN> {
        protected:
        friend class __vec<V,T,1>;
        public:
        // it wont be possible to cascade constructors...
        T x() const {return static_cast<const V*>(this)->data[0];}
    };
    // note: I extend "single underscore vec<T,1>"
    template <class V, typename T, unsigned short cN>
    class _vec<V,T,2,cN> : public _vec<V,T,1,cN> {
        protected:
        friend class _vec<V,T,1,cN>;
        public:
        T y() const {return static_cast<const V*>(this)->data[1];}
    };

    template <class V, typename T, unsigned short cN>
    class _vec<V,T,3,cN> : public _vec<V,T,2,cN> {
        protected:
        friend class _vec<V,T,2,cN>;
        public:
        
        T z() const {return static_cast<const V*>(this)->data[2];}
    };
    template <class V, typename T, unsigned short cN>
    class _vec<V,T,4,cN> : public _vec<V,T,3,cN> {
        protected:
        friend class _vec<V,T,3,cN>;
        public:
        
        T w() const {return static_cast<const V*>(this)->data[3];}
    };
    // now - the real ones you can actually touch:
    // ok the first vector with storage:
    template <typename T, unsigned short N>
    class vec : public _vec<vec<T,N>,T,N,N> {
        protected:
        T data[N];
        // now my base class can access my data...
        // what about the base-class of my base class?
        friend class _vec<vec<T,N>,T,N,N>;
        public:
        vec(T v[N]){
            for(auto i = 0;i < N;++i){
                this->data[i]=v[i];
            }
        }
    };
    template <typename T>
    class vec<T,1> : public _vec<vec<T,1>,T,1,1> {
        protected:
        T data[1];
        // now my base class can access my data...
        // what about the base-class of my base class?
        friend class _vec<vec<T,1>,T,1,1>;
        public:
        vec(T v[1]){
            for(auto i = 0;i < 1;++i){
                this->data[i]=v[i];
            }
        }
    };
    template <typename T>
    class vec<T,2> : public _vec<vec<T,2>,T,2,2> {
        protected:
        T data[2];
        // now my base class can access my data...
        // what about the base-class of my base class?
        friend class _vec<vec<T,2>,T,2,2>;
        public:
        vec(T v[2]){
            for(auto i = 0;i < 2;++i){
                this->data[i]=v[i];
            }
        }
        // swizzles
        _ALL_SWIZZLES_2
    };
    template <typename T>
    class vec<T,3> : public _vec<vec<T,3>,T,3,3> {
        protected:
        T data[3];
        // now my base class can access my data...
        // what about the base-class of my base class?
        friend class __vec<vec<T,3>,T,3>;
        friend class _vec<vec<T,3>,T,2,3>;
        friend class _vec<vec<T,3>,T,1,3>;
        public:
        
        vec(T v[3]){
            for(auto i = 0;i < 3;++i){
                this->data[i]=v[i];
            }
        }
        // swizzles
        _ALL_SWIZZLES_3
    };
    template <typename T>
    class vec<T,4> : public _vec<vec<T,4>,T,4,4> {
        protected:
        T data[4];
        // now my base class can access my data...
        // what about the base-class of my base class?
        friend class __vec<vec<T,4>,T,4>;
        friend class _vec<vec<T,4>,T,3,4>;
        friend class _vec<vec<T,4>,T,2,4>;
        friend class _vec<vec<T,4>,T,1,4>;
        public:
        
        vec(T v[4]){
            for(auto i = 0;i < 4;++i){
                this->data[i]=v[i];
            }
        }
        // swizzles
        _ALL_SWIZZLES_4
    };
};