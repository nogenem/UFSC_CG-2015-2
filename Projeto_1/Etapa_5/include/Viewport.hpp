#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <ctime>
#include "Window.hpp"
#include "Objects.hpp"
#include "World.hpp"
#include "Clipping.hpp"

#define PI 3.1415926535897932384626433832795

class Viewport
{
    public:
        Viewport(double width, double height, World *world):
            m_width(width), m_height(height), m_world(world), m_window(width,height),
            m_border(new ClipWindow{-0.95,0.95,-0.95,0.95}), m_clipping(m_border)
            { transformAndClipAllObjs(); }
        virtual ~Viewport(){ delete m_border; }

        void transformAndClipObj(Object* obj);
        void changeLineClipAlg(const LineClipAlgs alg){m_clipping.setLineClipAlg(alg); transformAndClipAllObjs();}

        void gotoObj(const std::string& objName);
        void zoomWindow(double step){m_window.zoom(step); transformAndClipAllObjs();}
        void moveWindow(double x, double y){m_window.move(x,y); transformAndClipAllObjs();}
        void rotateWindow(double graus){m_window.setAngulo(graus); transformAndClipAllObjs();}
        void drawObjs(cairo_t* cr);

    private:
        Coordinate transformCoordinate(const Coordinate& c) const;
        void transformCoordinates(const Coordinates& coords,
                                    Coordinates& output) const;

        void transformAndClipAllObjs();

        void drawObj(Object* obj);
        void drawPoint(Object* obj);
        void drawLine(Object* obj);
        void drawPolygon(Object* obj);
        void drawCurve(Object* obj);

        void prepareContext(const Object* obj);

    private:
        double m_width, m_height;
        World* m_world;
        Window m_window;

        cairo_t* m_cairo;

        ClipWindow *m_border;
        Clipping m_clipping;
};

void Viewport::gotoObj(const std::string& objName){
    Object *obj = m_world->getObj(objName);
    Coordinate c = obj->center();
    m_window.moveTo(c);
    transformAndClipAllObjs();
}

void Viewport::transformAndClipObj(Object* obj){
    auto t = m_window.getT();
    obj->transformNormalized(t);

    if(!m_clipping.clip(obj))
        obj->getNCoords().clear();
}

void Viewport::transformAndClipAllObjs(){
    m_window.updateTransformation();

    auto element = m_world->getFirstObject();
    while(element != nullptr){
        transformAndClipObj(element->getInfo());
        element = element->getProximo();
    }
}

Coordinate Viewport::transformCoordinate(const Coordinate& c) const {
    const Coordinate wmin = m_window.wMin();
    const Coordinate wmax = m_window.wMax();

    double x = ((c.x-wmin.x)/(wmax.x-wmin.x))*m_width;
    double y = (1-((c.y-wmin.y)/(wmax.y-wmin.y)))*m_height;

    return Coordinate(x,y);
}

void Viewport::transformCoordinates(const Coordinates& coords, Coordinates& output) const {
    for(const auto &c : coords)
        output.push_back(transformCoordinate(c));
}

void Viewport::drawObjs(cairo_t* cr){
    m_cairo = cr;

    auto element = m_world->getFirstObject();
    while(element != nullptr){
        drawObj(element->getInfo());
        element = element->getProximo();
    }
    drawObj(m_border);
}

void Viewport::drawObj(Object* obj){
    if(obj->getNCoordsSize() == 0) return;

    switch(obj->getType()){
    case ObjType::OBJECT:
        break;
    case ObjType::POINT:
        drawPoint(obj);
        break;
    case ObjType::LINE:
        drawLine(obj);
        break;
    case ObjType::POLYGON:
        drawPolygon(obj);
        break;
    case ObjType::CURVE:
        drawCurve(obj);
        break;
    }
}

void Viewport::drawPoint(Object* obj){
    Coordinate coord = transformCoordinate(obj->getNCoord(0));
    prepareContext(obj);

    cairo_move_to(m_cairo, coord.x, coord.y);
    cairo_arc(m_cairo, coord.x, coord.y, 1.0, 0.0, (2*PI) );//pnt deveria ir diminuindo, nao?
    cairo_fill(m_cairo);
}

void Viewport::drawLine(Object* obj){
    auto coords = obj->getNCoords();
    Coordinates nCoords;
    if(coords[0] == coords[1]){// Usuario quer um ponto?
        drawPoint(obj);
        return;
    }
    transformCoordinates(coords, nCoords);
    prepareContext(obj);

    cairo_move_to(m_cairo, nCoords[0].x, nCoords[0].y);
    cairo_line_to(m_cairo, nCoords[1].x, nCoords[1].y);
    cairo_stroke(m_cairo);
}

void Viewport::drawPolygon(Object* obj){
    auto coords = obj->getNCoords();
    Coordinates nCoords;
    if(coords.size() == 1){// Usuario quer um ponto?
        drawPoint(obj);
        return;
    }else if(coords.size() == 2){// Usuario quer uma linha?
        drawLine(obj);
        return;
    }

    transformCoordinates(coords, nCoords);
    prepareContext(obj);

    cairo_move_to(m_cairo, nCoords[0].x, nCoords[0].y);
    for(unsigned int i = 0; i<nCoords.size(); i++)
        cairo_line_to(m_cairo, nCoords[i].x, nCoords[i].y);

    cairo_close_path(m_cairo);

    Polygon* p = (Polygon*) obj;
    if(p->filled()){
        cairo_stroke_preserve(m_cairo);
        cairo_fill(m_cairo);
    }else
        cairo_stroke(m_cairo);
}

void Viewport::drawCurve(Object* obj){
    auto coords = obj->getNCoords();
    Coordinates nCoords;

    transformCoordinates(coords, nCoords);
    prepareContext(obj);

    cairo_move_to(m_cairo, nCoords[0].x, nCoords[0].y);
    for(unsigned int i = 0; i<nCoords.size(); i++)
        cairo_line_to(m_cairo, nCoords[i].x, nCoords[i].y);

    cairo_stroke(m_cairo);
}

void Viewport::prepareContext(const Object* obj){
    const GdkRGBA color = obj->getColor();
    cairo_set_source_rgb(m_cairo, color.red, color.green, color.blue);
    cairo_set_line_width(m_cairo, ((obj==m_border) ? 3 : 1) );//Pequena gambiarra...
}

#endif // VIEWPORT_HPP
