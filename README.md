# imvec
an immutable vector-math library, mostly for experimenting with new(er) c++ concepts

## Should you use this?
No, probably not. This is a simple vector math library for experimenting with (usually 3D) math, as well as an experimental playground for new (to me) features of c++, like those in C++20. It may have math mistakes! It may even be slow! Its almost certainly slower than anything using SIMD intrinsics.

## Build instructions?
I would love to be building this with the genius [dds](https://github.com/vector-of-bool/dds) but I cant get it to work for me on windows at the moment. For now, main.cpp is just a simple executable for me to make test compilation, which is done via clang-cl, c++17. Due to the heavy use of templates, the meat of the library is in a single imvec.hpp file, which includes another gross file (swizzles.h) full of macros.

## What and Why?
I do a lot of GLSL, and I very much enjoy the ease with which vectors of 2, 3 and 4 components may be manipulated in that language. I wondered how the similar structures of vectors would play with some (new to me) ideas like [free your functions](https://link) and [type erasure](https://link) and [CRTP](https://link). <BR>
A strong difference between GLSL vectors and those presented here is mutability. In GLSL, its easy to make a vec3, and then `someVec3.y = whatever;`. I always have felt that immutable value semantics make more sense for what are essentially numeric types, so this type of in-place mutation is dissallowed in this library.

## Goals
1. Convenient syntax:<BR> again refering to GLSL, its easy to 'swizzle' any vector - for example `someVec.yyx` is a valid expression that produces a new vector as if you called `vec3(someVec.y,someVec.y,someVec.x)`. I also like common numeric operators, (+, -, *, /) to be available. Some people prefer free-function style math like `dot(a,b)` where others prefer `a.dot(b)` this library aims to provide both forms where possible.
2. Consise:<BR>
Obviously, there are a few ways to think about the similarities between classes of vectors. I've gone with a template-based approach: `vec<ComponentType, Size>`. One could also argue that a vec3 is an extension of a vec2, and so on. The goal is to have the least code without and undue burdon on the other goals.
3. Low Overhead: <BR>
We dont want to pay for virtual tables. It would be great if the size of a vec was no more than the storage needed for its N components of type T.
4. Immutable value semantics, no implicit casting.
5. Reasonably easy to extend - this is made a bit tricky by the immutable requirement... I think.