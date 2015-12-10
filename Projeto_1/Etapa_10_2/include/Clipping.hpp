#ifndef CLIPPING_HPP
#define CLIPPING_HPP

#include "Objects.hpp"

/**
 * Retangulo delimitando a window para podermos ver
 *  os algoritmos de clipping funcionando.
 **/
class ClipWindow : public Polygon {
public:
    ClipWindow(double minX_, double maxX_, double minY_, double maxY_);
    void addCoordinate(double x, double y) {m_coords.emplace_back(x,y);
                                            m_nCoords.emplace_back(x,y);}

    double minX, maxX, minY, maxY;
};

/**
 * Enumeração dos dois algoritmos de clipping de linhas.
 *      CS = Cohen Sutherland
 *      LB = Liang Basky
 **/
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
        bool clipPoint(const Coordinate& c);
        bool clipLine(Coordinate& c1, Coordinate& c2);
        bool clipPolygon(Object* p)
            {return SutherlandHodgmanPolygonClip(p);}
        bool clipCurve(Object *obj);

        int getCoordRC(const Coordinate& c);
        bool CohenSutherlandLineClip(Coordinate& c1, Coordinate& c2);
        bool LiangBaskyLineClip(Coordinate& c1, Coordinate& c2);
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
    case ObjType::OBJECT:
        break;
    case ObjType::POINT:
        return clipPoint(obj->getNCoord(0));
    case ObjType::LINE:
        return clipLine(obj->getNCoord(0), obj->getNCoord(1));
    case ObjType::POLYGON:
        return clipPolygon(obj);
    case ObjType::BEZIER_CURVE:
    case ObjType::BSPLINE_CURVE:
        return clipCurve(obj);
    case ObjType::OBJECT3D:{
        Object3D *obj3d = (Object3D*) obj;
        bool draw = false;
        for(auto &face : obj3d->getFaceList()){
            bool tmp = clipPolygon(&face);
            if(!tmp){ face.getNCoords().clear(); }
            draw |= tmp;
        }
        return draw;
    }case ObjType::BEZIER_SURFACE:
    case ObjType::BSPLINE_SURFACE:{
        Surface *surf = (Surface*) obj;
        bool draw = false;
        for(auto &curve : surf->getCurveList()){
            bool tmp = clipCurve(&curve);
            if(!tmp){ curve.getNCoords().clear(); }
            draw |= tmp;
        }
        return draw;
    }}
    return false;
}

bool Clipping::clipPoint(const Coordinate& c){
    return c.x >= m_w->minX && c.x <= m_w->maxX &&
                c.y >= m_w->minY && c.y <= m_w->maxY;
}

bool Clipping::clipLine(Coordinate& c1, Coordinate& c2){
    if(m_current == LineClipAlgs::CS)
        return CohenSutherlandLineClip(c1,c2);
    else
        return LiangBaskyLineClip(c1,c2);
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

bool Clipping::CohenSutherlandLineClip(Coordinate& c1, Coordinate& c2){
    if(c1 == c2) return clipPoint(c1);

    int rc1 = getCoordRC(c1);
    int rc2 = getCoordRC(c2);

    while(true){
        if( (rc1 | rc2) == 0 )// Dentro
            return true;
        else if( (rc1 & rc2) != 0 )// Fora
            return false;

        double x=0,y=0;
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
bool Clipping::LiangBaskyLineClip(Coordinate& c1, Coordinate& c2){
    if(c1 == c2) return clipPoint(c1);

    auto delta = c2 - c1;
    double p,q,r;
    double u1 = 0.0, u2 = 1.0;

    for(int pos = 0; pos<4; pos++){
             if(pos == 0){ p = -delta.x; q = c1.x - m_w->minX; }
        else if(pos == 1){ p = delta.x;  q = m_w->maxX - c1.x; }
        else if(pos == 2){ p = -delta.y; q = c1.y - m_w->minY; }
        else if(pos == 3){ p = delta.y;  q = m_w->maxY - c1.y; }

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
    auto &input = p->getNCoords();
    Coordinates tmp;

    clipLeft(input, tmp);
    clipRight(tmp, input);
    clipBottom(input, tmp);
    clipTop(tmp, input);

    return (input.size() != 0);
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
        if(c0.x < clipX && c1.x < clipX){
            continue;
        }

        //Caso 2: in -> in
        if(c0.x >= clipX && c1.x >= clipX){
            output.push_back(c1);
            continue;
        }

        double x = clipX;
        double m = (c1.y-c0.y)/(c1.x-c0.x);
        double y = m * (x-c0.x) + c0.y;

        //Caso 3: in -> out
        if(c0.x >= clipX && c1.x < clipX){
            output.emplace_back(x,y);
            continue;
        }

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
        if(c0.x > clipX && c1.x > clipX){
            continue;
        }

        //Caso 2: in -> in
        if(c0.x <= clipX && c1.x <= clipX){
            output.push_back(c1);
            continue;
        }

        double x = clipX;
        double m = (c1.y-c0.y)/(c1.x-c0.x);
        double y = m * (x-c0.x) + c0.y;

        //Caso 3: in -> out
        if(c0.x <= clipX && c1.x > clipX){
            output.emplace_back(x,y);
            continue;
        }

        //Caso 4: out -> in
        if(c0.x > clipX && c1.x <= clipX){
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
        if(c0.y > clipY && c1.y > clipY){
            continue;
        }

        //Caso 2: in -> in
        if(c0.y <= clipY && c1.y <= clipY){
            output.push_back(c1);
            continue;
        }

        double y = clipY;
        double m = (c1.x-c0.x)/(c1.y-c0.y);
        double x = m * (y-c0.y) + c0.x;

        //Caso 3: in -> out
        if(c0.y <= clipY && c1.y > clipY){
            output.emplace_back(x,y);
            continue;
        }

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
        if(c0.y < clipY && c1.y < clipY){
            continue;
        }

        //Caso 2: in -> in
        if(c0.y >= clipY && c1.y >= clipY){
            output.push_back(c1);
            continue;
        }

        double y = clipY;
        double m = (c1.x-c0.x)/(c1.y-c0.y);
        double x = m * (y-c0.y) + c0.x;

        //Caso 3: in -> out
        if(c0.y >= clipY && c1.y < clipY){
            output.emplace_back(x,y);
            continue;
        }

        //Caso 4: out -> in
        if(c0.y < clipY && c1.y >= clipY){
            output.emplace_back(x,y);
            output.push_back(c1);
        }
    }
}

bool Clipping::clipCurve(Object *obj){
    auto& coords = obj->getNCoords();
    Coordinates newPath;
    bool prevInside = true;
    Coordinate prev;

    for(unsigned int i = 0; i < coords.size(); i++){
        if(clipPoint(coords[i])){
            if(!prevInside){
                clipLine(prev, coords[i]);
                newPath.push_back(prev);
            }
            newPath.push_back(coords[i]);
            prevInside = true;
        }else{
            if(prevInside && newPath.size() != 0){
                clipLine(prev, coords[i]);
                newPath.push_back(coords[i]);
            }
            prevInside = false;
        }
        prev = coords[i];
    }

    if(newPath.size() == 0)
        return false;

    obj->setNCoord(newPath);
    return true;
}

#endif // CLIPPING_HPP
