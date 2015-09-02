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

        bool clipPoint(Point* p, const ClipWindow& window);
    protected:
    private:
};

bool Clipping::clipPoint(Point* p, const ClipWindow& window){
    auto c = p->getNCoord(0);
    bool draw = c.x >= window.minX && c.x <= window.maxX &&
                c.y >= window.minY && c.y <= window.maxY;

    return draw;
}

#endif // CLIPPING_HPP
