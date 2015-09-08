#ifndef CLIPPING_HPP
#define CLIPPING_HPP

#include "Objects.hpp"

class ClipWindow : public Polygon {
public:
    ClipWindow(double minX_, double maxX_, double minY_, double maxY_);

    void addCoordinate(double x, double y) {_coords.emplace_back(x,y);
                                            _nCoords.emplace_back(x,y);}
    double minX, maxX, minY, maxY;
};

enum class LineClipAlgs { CS, LB };

class Clipping
{
    public:
        Clipping(ClipWindow* window):
            _w(window), _current(LineClipAlgs::CS) {}
        virtual ~Clipping() {}

        void setLineClipAlg(LineClipAlgs alg){ _current = alg; }

        bool clipPoint(Point* p);
        bool clipLine(Line* l);
        bool clipPolygon(Polygon* p){return SutherlandHodgmanPolygonClip(p);}
    protected:
    private:
        ClipWindow* _w;
        LineClipAlgs _current;

        enum RC {INSIDE=0, LEFT=1, RIGHT=2, BOTTOM=4, TOP=8};

        int getCoordRC(const Coordinate& c);
        bool CohenSutherlandLineClip(Line* l);
        bool LiangBaskyLineClip(Line* l);
        bool SutherlandHodgmanPolygonClip(Polygon* p);

        void clipLeft(Coordinates& input, Coordinates& output);
        void clipRight(Coordinates& input, Coordinates& output);
        void clipTop(Coordinates& input, Coordinates& output);
        void clipBottom(Coordinates& input, Coordinates& output);
};

ClipWindow::ClipWindow(double minX_, double maxX_, double minY_, double maxY_):
        Polygon("_border_", GdkRGBA({0,0.9,0})) {

    minX = minX_; maxX = maxX_;
    minY = minY_; maxY = maxY_;

    addCoordinate(minX,minY);
    addCoordinate(maxX,minY);
    addCoordinate(maxX,maxY);
    addCoordinate(minX,maxY);
}

bool Clipping::clipPoint(Point* p){
    if(p->getNCoordsSize() == 0) return false;

    auto c = p->getNCoord(0);
    return c.x >= _w->minX && c.x <= _w->maxX &&
                c.y >= _w->minY && c.y <= _w->maxY;
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

    if(c.x < _w->minX)
        rc |= Clipping::RC::LEFT;
    else if(c.x > _w->maxX)
        rc |= Clipping::RC::RIGHT;

    if(c.y < _w->minY)
        rc |= Clipping::RC::BOTTOM;
    else if(c.y > _w->maxY)
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
        int rc = rc1 ? rc1 : rc2;// (rc1 == 0) => Dentro
        double m = (c2.y-c1.y)/(c2.x-c1.x);

        if(rc & Clipping::RC::TOP){
            x = c1.x + 1/m * (_w->maxY-c1.y);
            y = _w->maxY;
        }else if(rc & Clipping::RC::BOTTOM){
            x = c1.x + 1/m * (_w->minY-c1.y);
            y = _w->minY;
        }else if(rc & Clipping::RC::RIGHT){
            y = m * (_w->maxX-c1.x) + c1.y;
            x = _w->maxX;
        }else if(rc & Clipping::RC::LEFT){
            y = m * (_w->minX-c1.x) + c1.y;
            x = _w->minX;
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
        if     (pos == 0){ p = -delta.x; q = c1.x - _w->minX; }
        else if(pos == 1){ p = delta.x; q = _w->maxX - c1.x; }
        else if(pos == 2){ p = -delta.y; q = c1.y - _w->minY; }
        else if(pos == 3){ p = delta.y; q = _w->maxY - c1.y; }

        if(p == 0 && q < 0)
            return false;

        r = q/p;
        if(p < 0){
            if(r > u1)
                u1 = r;
        }else if(p > 0){
            if(r < u2)
                u2 = r;
        }
    }

    if(u1 > u2) return false;

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

bool Clipping::SutherlandHodgmanPolygonClip(Polygon* p){

    auto input = p->getNCoords();
    Coordinates tmp;
    Coordinates output;

    clipLeft(input, tmp);
    clipRight(tmp, output);
    clipTop(output, tmp);
    clipBottom(tmp, output);

    if(output.size() == 0)
        return false;

    p->setNCoord(output);
    return true;
}

void Clipping::clipLeft(Coordinates& input, Coordinates& output){
    if(output.size() > 0)
        output.clear();
    if(input.size() == 0)
        return;

    double clipX = _w->minX;
    input.push_back(input[0]);
    for(unsigned int i = 0; i < input.size()-1; i++){
        Coordinate c0 = input[i];
        Coordinate c1 = input[i+1];

        //Caso 1: out -> out
        if(c0.x < clipX && c1.x < clipX){}

        //Caso 2: in -> in
        if(c0.x >= clipX && c1.x >= clipX)
            output.push_back(c1);

        //Caso 3: in -> out
        if(c0.x >= clipX && c1.x < clipX){
            double x = clipX;
            double m = (c1.y-c0.y)/(c1.x-c0.x);
            double y = m * (x-c0.x) + c0.y;
            output.emplace_back(x,y);
        }

        //Caso 4: out -> in
        if(c0.x < clipX && c1.x >= clipX){
            double x = clipX;
            double m = (c1.y-c0.y)/(c1.x-c0.x);
            double y = m * (x-c0.x) + c0.y;
            output.emplace_back(x,y);
            output.push_back(c1);
        }
    }
}

void Clipping::clipRight(Coordinates& input, Coordinates& output){
    if(output.size() > 0)
        output.clear();
    if(input.size() == 0)
        return;

    double clipX = _w->maxX;
    input.push_back(input[0]);
    for(unsigned int i = 0; i < input.size()-1; i++){
        Coordinate c0 = input[i];
        Coordinate c1 = input[i+1];

        //Caso 1: out -> out
        if(c0.x >= clipX && c1.x >= clipX){}

        //Caso 2: in -> in
        if(c0.x < clipX && c1.x < clipX)
            output.push_back(c1);

        //Caso 3: in -> out
        if(c0.x < clipX && c1.x >= clipX){
            double x = clipX;
            double m = (c1.y-c0.y)/(c1.x-c0.x);
            double y = m * (x-c0.x) + c0.y;
            output.emplace_back(x,y);
        }

        //Caso 4: out -> in
        if(c0.x >= clipX && c1.x < clipX){
            double x = clipX;
            double m = (c1.y-c0.y)/(c1.x-c0.x);
            double y = m * (x-c0.x) + c0.y;
            output.emplace_back(x,y);
            output.push_back(c1);
        }
    }
}

void Clipping::clipTop(Coordinates& input, Coordinates& output){
    if(output.size() > 0)
        output.clear();
    if(input.size() == 0)
        return;

    double clipY = _w->maxY;
    input.push_back(input[0]);
    for(unsigned int i = 0; i < input.size()-1; i++){
        Coordinate c0 = input[i];
        Coordinate c1 = input[i+1];

        //Caso 1: out -> out
        if(c0.y > clipY && c1.y > clipY){}

        //Caso 2: in -> in
        if(c0.y <= clipY && c1.y <= clipY)
            output.push_back(c1);

        //Caso 3: in -> out
        if(c0.y <= clipY && c1.y > clipY){
            double y = clipY;
            double m = (c1.x-c0.x)/(c1.y-c0.y);
            double x = m * (y-c0.y) + c0.x;
            output.emplace_back(x,y);
        }

        //Caso 4: out -> in
        if(c0.y > clipY && c1.y <= clipY){
            double y = clipY;
            double m = (c1.x-c0.x)/(c1.y-c0.y);
            double x = m * (y-c0.y) + c0.x;
            output.emplace_back(x,y);
            output.push_back(c1);
        }
    }
}

void Clipping::clipBottom(Coordinates& input, Coordinates& output){
    if(output.size() > 0)
        output.clear();
    if(input.size() == 0)
        return;

    double clipY = _w->minY;
    input.push_back(input[0]);
    for(unsigned int i = 0; i < input.size()-1; i++){
        Coordinate c0 = input[i];
        Coordinate c1 = input[i+1];

        //Caso 1: out -> out
        if(c0.y < clipY && c1.y < clipY){}

        //Caso 2: in -> in
        if(c0.y >= clipY && c1.y >= clipY)
            output.push_back(c1);

        //Caso 3: in -> out
        if(c0.y >= clipY && c1.y < clipY){
            double y = clipY;
            double m = (c1.x-c0.x)/(c1.y-c0.y);
            double x = m * (y-c0.y) + c0.x;
            output.emplace_back(x,y);
        }

        //Caso 4: out -> in
        if(c0.y < clipY && c1.y >= clipY){
            double y = clipY;
            double m = (c1.x-c0.x)/(c1.y-c0.y);
            double x = m * (y-c0.y) + c0.x;
            output.emplace_back(x,y);
            output.push_back(c1);
        }
    }
}

#endif // CLIPPING_HPP
