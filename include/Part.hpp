#ifndef PART_H
#define PART_H

class Part{
    public:
        Part(){}
        virtual ~Part(){}

        virtual double getOpTime() = 0;
    protected:
};

class Part1: public Part{
    public:
        Part1();
        virtual ~Part1(){}

        double getOpTime(){ return 1.; }
};

class Part2: public Part{
    public:
        Part2();
        virtual ~Part2(){}

        double getOpTime(){ return 3.; }
};

class Part3: public Part{
    public:
        Part3();
        virtual ~Part3(){}

        double getOpTime(){ return 4.; }
};

class Part4: public Part{
    public:
        Part4();
        virtual ~Part4(){}

        double getOpTime(){ return 10.; }
};

#endif

