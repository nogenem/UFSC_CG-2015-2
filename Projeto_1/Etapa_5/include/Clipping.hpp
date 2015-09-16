#ifndef CLIPPING_HPP
#define CLIPPING_HPP

#include "Objects.hpp"

class ClipWindow : public Polygon {
public:
    ClipWindow(double minX_, double maxX_, double minY_, double maxY_);
    void addCoordinate(double x, double y) {m_coords.emplace_back(x,y);
                                            m_nCoords.emplace_back(x,y);}

    double minX, maxX, minY, maxY;
};

enum class LineClipAlgs { CS, LB };

class Clipping
{
    public:
        Clipping(const ClipWindow* window):
            m_w(window) {}
        virtual ~Clipping() {}

        void setLineClipAlg(const LineClipAlgs alg){ m_current = alg; }

        bool clip(Object* obj);

    private:
        bool clipPoint(Object* p);
        bool clipLine(Object* l);
        bool clipPolygon(Object* p){return SutherlandHodgmanPolygonClip(p);}

        int getCoordRC(const Coordinate& c);
        bool CohenSutherlandLineClip(Object* l);
        bool LiangBaskyLineClip(Object* l);
        bool SutherlandHodgmanPolygonClip(Object* p);

        void clipLeft(Coordinates& input, Coordinates& output);
        void clipRight(Coordinates& input, Coordinates& output);
        void clipTop(Coordinates& input, Coordinates& output);
        void clipBottom(Coordinates& input, Coordinates& output);

    private:
        const ClipWindow* m_w;
        LineClipAlgs m_current = LineClipAlgs::CS;

        enum RC {INSIDE=0, LEFT=1, RIGHT=2, BOTTOM=4, TOP=8};
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

bool Clipping::clip(Object* obj){
    switch(obj->getType()){
    case ObjType::POINT:
        return clipPoint(obj);
    case ObjType::LINE:
        return clipLine(obj);
    case ObjType::POLYGON:
        return clipPolygon(obj);
    case ObjType::CURVE:
        return true;//Temporario
    default:
        return false;
    }
}

bool Clipping::clipPoint(Object* p){
    if(p->getNCoordsSize() == 0) return false;

    auto c = p->getNCoord(0);
    return c.x >= m_w->minX && c.x <= m_w->maxX &&
                c.y >= m_w->minY && c.y <= m_w->maxY;
}

bool Clipping::clipLine(Object* l){
    if(m_current == LineClipAlgs::CS)
        return CohenSutherlandLineClip(l);
    else
        return LiangBaskyLineClip(l);
}

//https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
int Clipping::getCoordRC(const Coordinate& c){
    int rc = Clipping::RC::INSIDE;

    if(c.x < m_w->minX)
        rc |= Clipping::RC::LEFT;
    else if(c.x > m_w->maxX)
        rc |= Clipping::RC::RIGHT;

    if(c.y < m_w->minY)
        rc |= Clipping::RC::BOTTOM;
    else if(c.y > m_w->maxY)
        rc |= Clipping::RC::TOP;

    return rc;
}

bool Clipping::CohenSutherlandLineClip(Object* l){
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
            x = c1.x + 1/m * (m_w->maxY-c1.y);
            y = m_w->maxY;
        }else if(rc & Clipping::RC::BOTTOM){
            x = c1.x + 1/m * (m_w->minY-c1.y);
            y = m_w->minY;
        }else if(rc & Clipping::RC::RIGHT){
            y = m * (m_w->maxX-c1.x) + c1.y;
            x = m_w->maxX;
        }else if(rc & Clipping::RC::LEFT){
            y = m * (m_w->minX-c1.x) + c1.y;
            x = m_w->minX;
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
bool Clipping::LiangBaskyLineClip(Object* l){
    auto &c1 = l->getNCoord(0);
    auto &c2 = l->getNCoord(1);

    if(c1 == c2) return clipPoint((Point*)l);

    auto delta = c2 - c1;
    double p,q,r;
    double u1 = 0.0, u2 = 1.0;

    for(int pos = 0; pos<4; pos++){
        if     (pos == 0){ p = -delta.x; q = c1.x - m_w->minX; }
        else if(pos == 1){ p = delta.x; q = m_w->maxX - c1.x; }
        else if(pos == 2){ p = -delta.y; q = c1.y - m_w->minY; }
        else if(pos == 3){ p = delta.y; q = m_w->maxY - c1.y; }

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

bool Clipping::SutherlandHodgmanPolygonClip(Object* p){

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

    double clipX = m_w->minX;
    input.push_back(input[0]);
    for(unsigned int i = 0; i < input.size()-1; i++){
        Coordinate c0 = input[i];
        Coordinate c1 = input[i+1];

        //Caso 1: out -> out
        if(c0.x < clipX && c1.x < clipX){}

        //Caso 2: in -> in
        if(c0.x >= clipX && c1.x >= clipX)
            output.push_back(c1);

        double x = clipX;
        double m = (c1.y-c0.y)/(c1.x-c0.x);
        double y = m * (x-c0.x) + c0.y;

        //Caso 3: in -> out
        if(c0.x >= clipX && c1.x < clipX)
            output.emplace_back(x,y);

        //Caso 4: out -> in
        if(c0.x < clipX && c1.x >= clipX){
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

    double clipX = m_w->maxX;
    input.push_back(input[0]);
    for(unsigned int i = 0; i < input.size()-1; i++){
        Coordinate c0 = input[i];
        Coordinate c1 = input[i+1];

        //Caso 1: out -> out
        if(c0.x >= clipX && c1.x >= clipX){}

        //Caso 2: in -> in
        if(c0.x < clipX && c1.x < clipX)
            output.push_back(c1);

        double x = clipX;
        double m = (c1.y-c0.y)/(c1.x-c0.x);
        double y = m * (x-c0.x) + c0.y;

        //Caso 3: in -> out
        if(c0.x < clipX && c1.x >= clipX)
            output.emplace_back(x,y);

        //Caso 4: out -> in
        if(c0.x >= clipX && c1.x < clipX){
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

    double clipY = m_w->maxY;
    input.push_back(input[0]);
    for(unsigned int i = 0; i < input.size()-1; i++){
        Coordinate c0 = input[i];
        Coordinate c1 = input[i+1];

        //Caso 1: out -> out
        if(c0.y > clipY && c1.y > clipY){}

        //Caso 2: in -> in
        if(c0.y <= clipY && c1.y <= clipY)
            output.push_back(c1);

        double y = clipY;
        double m = (c1.x-c0.x)/(c1.y-c0.y);
        double x = m * (y-c0.y) + c0.x;

        //Caso 3: in -> out
        if(c0.y <= clipY && c1.y > clipY)
            output.emplace_back(x,y);

        //Caso 4: out -> in
        if(c0.y > clipY && c1.y <= clipY){
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

    double clipY = m_w->minY;
    input.push_back(input[0]);
    for(unsigned int i = 0; i < input.size()-1; i++){
        Coordinate c0 = input[i];
        Coordinate c1 = input[i+1];

        //Caso 1: out -> out
        if(c0.y < clipY && c1.y < clipY){}

        //Caso 2: in -> in
        if(c0.y >= clipY && c1.y >= clipY)
            output.push_back(c1);

        double y = clipY;
        double m = (c1.x-c0.x)/(c1.y-c0.y);
        double x = m * (y-c0.y) + c0.x;

        //Caso 3: in -> out
        if(c0.y >= clipY && c1.y < clipY)
            output.emplace_back(x,y);

        //Caso 4: out -> in
        if(c0.y < clipY && c1.y >= clipY){
            output.emplace_back(x,y);
            output.push_back(c1);
        }
    }
}

#endif // CLIPPING_HPP
