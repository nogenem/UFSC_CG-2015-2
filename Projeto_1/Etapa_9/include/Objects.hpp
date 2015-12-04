#ifndef OBJECTS_H
#define OBJECTS_H

#include <gtk/gtk.h>
#include <iostream>
#include <string>
#include <vector>
#include "Transformation.hpp"

class Transformation;

class Coordinate
{
    public:
        Coordinate(){}
        Coordinate(double cx, double cy, double cz = 0) :
            x(cx), y(cy), z(cz), w(1){}
        virtual ~Coordinate(){};

        Coordinate& operator+=(double step);
        Coordinate& operator-=(double step);
        Coordinate& operator+=(const Coordinate& c);
        Coordinate& operator-=(const Coordinate& c);
        Coordinate& operator*=(const Transformation& t);
        Coordinate operator-() const;
        bool operator==(const Coordinate& c)
            { return (this->x==c.x && this->y==c.y &&
                      this->z==c.z); }

        double x = 0, y = 0, z = 0, w = 1;
};
std::ostream& operator<<(std::ostream& os, const Coordinate& c);
Coordinate operator-(const Coordinate& c1, const Coordinate& c2);

typedef std::vector<Coordinate> Coordinates;

enum class ObjType { OBJECT, POINT, LINE, POLYGON, BEZIER_CURVE,
    BSPLINE_CURVE, OBJECT3D, BEZIER_SURFACE, BSPLINE_SURFACE};

class Object
{
    public:
        Object(const std::string& name) :
            m_name(name) {}
        Object(const std::string& name, const GdkRGBA& c) :
            m_name(name), m_color({c.red,c.green,c.blue,c.alpha}) {}
        virtual ~Object() {}

        const std::string& getName() const { return m_name; }

        const GdkRGBA& getColor() const { return m_color; }

        virtual ObjType getType() const { return ObjType::OBJECT; }
		virtual std::string getTypeName() const { return "Object"; }

        Coordinates& getCoords() {return m_coords;}
        Coordinate& getCoord(int index) { return m_coords[index]; }
        int getCoordsSize() const { return m_coords.size(); }

        Coordinates& getNCoords() {return m_nCoords;}
		Coordinate& getNCoord(int index) { return m_nCoords[index]; }
		void setNCoord(const Coordinates& c);
		int getNCoordsSize() const { return m_nCoords.size(); }

        virtual Coordinate center() const;
        virtual Coordinate nCenter() const;
        virtual void transform(const Transformation& t);
        virtual void transformNormalized(const Transformation& t);

        bool operator==(const Object& other)
            { return this->getName() == other.getName(); }
        Object& operator*(){ return *this; }

		virtual void addCoordinate(double x, double y, double z)
            { m_coords.emplace_back(x,y,z); }
		void addCoordinate(const Coordinate& p) { m_coords.push_back(p); }

    protected:
        void addCoordinate(const Coordinates& coords)
            { m_coords.insert(m_coords.end(), coords.begin(), coords.end()); }

    protected:
        std::string m_name;
        GdkRGBA m_color{};// [inicializada como preta]
        Coordinates m_coords;
        Coordinates m_nCoords; // Coordenadas normalizadadas
};

class Point : public Object
{
    public:
        Point(const std::string& name) :
            Object(name) {}
        Point(const std::string& name, const GdkRGBA& color) :
            Object(name,color) {}
		Point(const std::string& name, const GdkRGBA& color, double x, double y, double z) :
            Object(name,color) { addCoordinate(x,y,z); }
        Point(const std::string& name, const GdkRGBA& color, const Coordinate& p) :
            Object(name,color) { addCoordinate(p); }

        virtual ObjType getType() const { return ObjType::POINT; }
		virtual std::string getTypeName() const { return "Point"; }
};

class Line : public Object
{
    public:
        Line(const std::string& name) :
            Object(name) {}
        Line(const std::string& name, const GdkRGBA& color) :
            Object(name,color) {}
		Line(const std::string& name, const GdkRGBA& color, const Coordinates& coords) :
            Object(name,color) { addCoordinate(coords); }

        virtual ObjType getType() const { return ObjType::LINE; }
		virtual std::string getTypeName() const { return "Line"; }
};

class Polygon : public Object
{
    public:
        Polygon(const std::string& name) :
            Object(name) {}
        Polygon(const std::string& name, const GdkRGBA& color) :
            Object(name,color) {}
		Polygon(const std::string& name, const GdkRGBA& color, const Coordinates& coords) :
            Object(name,color) { addCoordinate(coords); }
        Polygon(const std::string& name, const Coordinates& coords) :
            Object(name) { addCoordinate(coords); }
        Polygon(const std::string& name, const GdkRGBA& color, bool filled, const Coordinates& coords) :
            Object(name,color) { m_filled = filled; addCoordinate(coords); }

        virtual ObjType getType() const { return ObjType::POLYGON; }
		virtual std::string getTypeName() const { return "Polygon"; }

        bool filled() const { return m_filled; }
        void setFilled(bool v){ m_filled = v; }

    private:
        bool m_filled = false;
};

class Curve : public Object
{
    public:
        Curve(const std::string& name) :
            Object(name) {}
        Curve(const std::string& name, const GdkRGBA& color) :
            Object(name,color) {}
		Curve(const std::string& name, const GdkRGBA& color, const Coordinates& coords) :
            Object(name,color) { }

        virtual void generateCurve(const Coordinates& cpCoords){};
        Coordinates& getControlPoints(){ return m_controlPoints; }

    protected:
        void setControlPoints(const Coordinates& coords)
                { m_controlPoints.insert(m_controlPoints.end(), coords.begin(), coords.end()); }

    protected:
            //Guarda os pontos de controle da curva
            // para serem usados na hora de salvar a curva no .obj
            Coordinates m_controlPoints;
            float m_step = 0.02; //Passo usado na bleding function
};

class BezierCurve : public Curve
{
    public:
        BezierCurve(const std::string& name) :
            Curve(name) {}
        BezierCurve(const std::string& name, const GdkRGBA& color) :
            Curve(name,color) {}
		BezierCurve(const std::string& name, const GdkRGBA& color, const Coordinates& coords) :
            Curve(name,color) { generateCurve(coords); }

        virtual ObjType getType() const { return ObjType::BEZIER_CURVE; }
		virtual std::string getTypeName() const { return "Bezier Curve"; }

		void generateCurve(const Coordinates& cpCoords);
};

class BSplineCurve : public Curve
{
    public:
        BSplineCurve(const std::string& name) :
            Curve(name) {}
        BSplineCurve(const std::string& name, const GdkRGBA& color) :
            Curve(name,color) {}
		BSplineCurve(const std::string& name, const GdkRGBA& color, const Coordinates& coords) :
            Curve(name,color) { this->generateCurve(coords); }

        virtual ObjType getType() const { return ObjType::BSPLINE_CURVE; }
		virtual std::string getTypeName() const { return "B-Spline Curve"; }

		void generateCurve(const Coordinates& cpCoords);
};

typedef std::vector<Polygon> FaceList;

class Object3D : public Object
{
    public:
        Object3D(const std::string& name) :
            Object(name) {}
        Object3D(const std::string& name, const FaceList& faces) :
            Object(name) { m_faceList = std::move(faces); }

        virtual ObjType getType() const { return ObjType::OBJECT3D; }
		virtual std::string getTypeName() const { return "3D Object"; }

        void transform(const Transformation& t);
        void transformNormalized(const Transformation& t);

        Coordinate center() const;
        Coordinate nCenter() const;

        FaceList& getFaceList()
            { return m_faceList; }

        void insertFaces(const FaceList& faces)
            { m_faceList.insert(m_faceList.end(),
                                faces.begin(), faces.end()); }

    protected:
        FaceList m_faceList;
};

typedef std::vector<Curve> CurveList;

class Surface : public Object
{
    public:
        Surface(const std::string& name) :
            Object(name) {}
        Surface(const std::string& name, const GdkRGBA& color) :
            Object(name,color) {}
        Surface(const std::string& name, const GdkRGBA& color, int maxLines, int maxCols) :
            Object(name,color) { m_maxLines = maxLines; m_maxCols = maxCols; }
		Surface(const std::string& name, const GdkRGBA& color, const Coordinates& coords) :
            Object(name,color) { }

        virtual void generateSurface(const Coordinates& cpCoords){};
        Coordinates& getControlPoints(){ return m_controlPoints; }

        void transform(const Transformation& t);
        void transformNormalized(const Transformation& t);

        Coordinate center() const;
        Coordinate nCenter() const;

        int getMaxLines(){ return m_maxLines; }
        int getMaxCols(){ return m_maxCols; }
        CurveList& getCurveList()
            { return m_curveList; }

    protected:
        void setControlPoints(const Coordinates& coords)
                { m_controlPoints.insert(m_controlPoints.end(), coords.begin(), coords.end()); }

    protected:
            //Guarda os pontos de controle da surface
            // para serem usados na hora de salvar a surface no .obj
            Coordinates m_controlPoints;
            float m_step = 0.02; //Passo usado na bleding function

            int m_maxLines = 4, m_maxCols = 4;
            CurveList m_curveList;
};

//http://www.cad.zju.edu.cn/home/zhx/GM/005/00-bcs2.pdf
class BezierSurface : public Surface
{
    public:
        BezierSurface(const std::string& name) :
            Surface(name) {}
        BezierSurface(const std::string& name, const GdkRGBA& color) :
            Surface(name,color) {}
		BezierSurface(const std::string& name, const GdkRGBA& color,
                int maxLines, int maxCols, const Coordinates& coords) :
            Surface(name,color,maxLines,maxCols) { generateSurface(coords); }
        BezierSurface(const std::string& name, const GdkRGBA& color, const Coordinates& coords) :
            Surface(name,color) { generateSurface(coords); }

        virtual ObjType getType() const { return ObjType::BEZIER_SURFACE; }
		virtual std::string getTypeName() const { return "Bezier Surface"; }

		void generateSurface(const Coordinates& cpCoords);
        Coordinate blendingFunction(float s, double s2, double s3,
                                    float t, double t2, double t3,
                                    int nLine, int nCol,
                                    const Coordinates& coords);
};

class BSplineSurface : public Surface
{
    public:
        BSplineSurface(const std::string& name) :
            Surface(name) {}
        BSplineSurface(const std::string& name, const GdkRGBA& color) :
            Surface(name,color) {}
        BSplineSurface(const std::string& name, const GdkRGBA& color,
                       int maxLines, int maxCols, const Coordinates& coords) :
            Surface(name,color,maxLines,maxCols) { generateSurface(coords); }
		BSplineSurface(const std::string& name, const GdkRGBA& color, const Coordinates& coords) :
            Surface(name,color) { generateSurface(coords); }

        virtual ObjType getType() const { return ObjType::BSPLINE_SURFACE; }
		virtual std::string getTypeName() const { return "B-Spline Surface"; }

		void generateSurface(const Coordinates& cpCoords);
		Coordinate blendingFunction(float s, double s2, double s3,
                                    float t, double t2, double t3,
                                    double n16, double n23,
                                    int nLine, int nCol,
                                    const Coordinates& coords);
};

#endif // OBJECTS_H
