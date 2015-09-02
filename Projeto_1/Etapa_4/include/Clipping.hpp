#ifndef CLIPPING_HPP
#define CLIPPING_HPP

typedef struct {
  double minX, maxX, minY, maxY;
}ClipWindow;

class Clipping
{
    public:
        Clipping() {}
        virtual ~Clipping() {}
    protected:
    private:
};

#endif // CLIPPING_HPP
