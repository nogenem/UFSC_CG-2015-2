#ifndef CLIPPING_HPP
#define CLIPPING_HPP

typedef struct {
  double minX, maxX, minY, maxY;
}ClipWindow;

enum class LineClipAlgs { CS, LB };

class Clipping
{
    public:
        Clipping(ClipWindow window):
            _w(window), _current(LineClipAlgs::CS) {}
        virtual ~Clipping() {}

        void setLineClipAlg(LineClipAlgs alg){ _current = alg; }

        bool clipPoint(Point* p);
        bool clipLine(Line* l);
    protected:
    private:
        ClipWindow _w;
        LineClipAlgs _current;

        enum RC {INSIDE=0, LEFT=1, RIGHT=2, BOTTOM=4, TOP=8};

        int getCoordRC(const Coordinate& c);
        bool CohenSutherlandLineClip(Line* l);
        bool LiangBaskyLineClip(Line* l);
};

bool Clipping::clipPoint(Point* p){
    if(p->getNCoordsSize() == 0) return false;

    auto c = p->getNCoord(0);
    return c.x >= _w.minX && c.x <= _w.maxX &&
                c.y >= _w.minY && c.y <= _w.maxY;
}

bool Clipping::clipLine(Line* l){
    if(_current == LineClipAlgs::CS)
        return CohenSutherlandLineClip(l);
    else
        return LiangBaskyLineClip(l);
}

//https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
int Clipping::getCoordRC(const Coordinate& c){
    int rc = Clipping::RC::INSIDE;

    if(c.x < _w.minX)
        rc |= Clipping::RC::LEFT;
    else if(c.x > _w.maxX)
        rc |= Clipping::RC::RIGHT;

    if(c.y < _w.minY)
        rc |= Clipping::RC::BOTTOM;
    else if(c.y > _w.maxY)
        rc |= Clipping::RC::TOP;

    return rc;
}

bool Clipping::CohenSutherlandLineClip(Line* l){
    auto &c1 = l->getNCoord(0);
    auto &c2 = l->getNCoord(1);

    int rc1 = getCoordRC(c1);
    int rc2 = getCoordRC(c2);

    while(true){
        if( (rc1 | rc2) == 0 )// Dentro
            return true;
        else if( (rc1 & rc2) != 0 )// Fora
            return false;

        double x,y;
        int rc = rc1 ? rc1 : rc2;
        double m = (c2.y-c1.y)/(c2.x-c1.x);

        if(rc & Clipping::RC::TOP){
            x = c1.x + 1/m * (_w.maxY-c1.y);
            y = _w.maxY;
        }else if(rc & Clipping::RC::BOTTOM){
            x = c1.x + 1/m * (_w.minY-c1.y);
            y = _w.minY;
        }else if(rc & Clipping::RC::RIGHT){
            y = m * (_w.maxX-c1.x) + c1.y;
            x = _w.maxX;
        }else if(rc & Clipping::RC::LEFT){
            y = m * (_w.minX-c1.x) + c1.y;
            x = _w.minX;
        }

        if(rc == rc1){
            c1.x = x;
            c1.y = y;
            rc1 = getCoordRC(c1);
        }else{
            c2.x = x;
            c2.y = y;
            rc2 = getCoordRC(c2);
        }
    }
}

//http://www.skytopia.com/project/articles/compsci/clipping.html
bool Clipping::LiangBaskyLineClip(Line* l){
    auto &c1 = l->getNCoord(0);
    auto &c2 = l->getNCoord(1);

    if(c1 == c2) return clipPoint((Point*)l);

    auto delta = c2 - c1;
    double p,q,r;
    double u1 = 0.0, u2 = 1.0;

    for(int pos = 0; pos<4; pos++){
        if(pos == 0){ p = -delta.x; q = c1.x - _w.minX; }
        else if(pos == 1){ p = delta.x; q = _w.maxX - c1.x; }
        else if(pos == 2){ p = -delta.y; q = c1.y - _w.minY; }
        else if(pos == 3){ p = delta.y; q = _w.maxY - c1.y; }

        if(p == 0 && q < 0)
            return false;

        r = q/p;
        if(p < 0){
            if(r > u2)
                return false;
            else if(r > u1)
                u1 = r;
        }else if(p > 0){
            if(r < u1)
                return false;
            else if(r < u2)
                u2 = r;
        }
    }

    if(u2 < 1){
        c2.x = c1.x + u2*delta.x;
        c2.y = c1.y + u2*delta.y;
    }
    if(u1 > 0){
        c1.x = c1.x + u1*delta.x;
        c1.y = c1.y + u1*delta.y;
    }
    return true;
}

#endif // CLIPPING_HPP
