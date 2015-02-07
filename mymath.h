#ifndef MYMATH_H
#define MYMATH_H


class MyMath
{
public:
    MyMath();
    ~MyMath();

    static float cclamp(float x, float minVal, float maxVal);
    static float sstep(float edge0, float edge1, float x);
};

#endif // MYMATH_H
