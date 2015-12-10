#ifndef FILEHANDLERS_HPP
#define FILEHANDLERS_HPP

#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <map>
#include <string>

/*
    Possiveis Diretivas:
        v, o, p, l, f, curv, surf,
        bsp, bzp, cstype, deg, end
        usemtl, newmtl, Kd

    Obs:
        Index dos vertices podem ser positivos
         ou negativos.

        Os arquivos .mtl devem estar na mesma
         pasta que o arquivo .obj.

        Pode-se usar '\' na declaração dos vertices
         dos objetos.
*/

class ColorReader
{
    public:
        // preto é a cor padrão para objetos
        ColorReader(){ m_colors["Padrao"] = GdkRGBA{}; }
        bool loadFile(const std::string& filename);
        const GdkRGBA& getColor(const std::string& colorName) const;

    private:
        void loadColors();
        void addColor(std::string name, std::stringstream& line);

    private:
        //Ex: <"Color0", GdkRGBA(0,0.6,0)>
        std::map<std::string, GdkRGBA> m_colors;
        std::ifstream m_ifs;
};

class ColorWriter
{
    public:
        ColorWriter(){}
        void loadFile(const std::string& filename);
        std::string getColorName(const GdkRGBA& color);

    private:
        void writeColor(const std::string& colorName,
                            const GdkRGBA& color);

    private:
        //Ex: <"(0,0.6,0)", "Color0">
        std::map<std::string, std::string> m_colors;
        std::ofstream m_ofs;
        //Contador de cores ja escritas, usado no nome da cor
        int m_numColors = 0;
};

class ObjStream
{
    public:
        ObjStream(std::string& filename);

    protected:
        std::string m_name;// Nome do arquivo, sem a extensão
        std::string m_path;// Path para o arquivo
        std::fstream m_objsFile;
        std::fstream m_colorsFile;
        GdkRGBA m_color{};// Cor atual [inicializada como preta]
};

class ObjReader : public ObjStream
{
    public:
        ObjReader(std::string& filename);
        std::vector<Object*>& getObjs(){ return m_objs; }

    private:
        void loadObjs();
        void setName(std::stringstream& line);
        void loadColorsFile(std::stringstream& line);
        void changeColor(std::stringstream& line);

        void addCoord(std::stringstream& line);
        void addPoint(std::stringstream& line);
        void addPoly(std::stringstream& line, bool filled = false);
        void addFace(std::stringstream& line);
        void addCurve(std::stringstream& line);
        void addSurface(std::stringstream& line, ObjType type);
        void addObj3D();

        void loadCoordsIndexes(std::stringstream& line, Coordinates& objCoords);

        // Usado para destruir os objs caso de algum erro
        void destroyObjs();

        void setFreeFormType(std::stringstream& line);

    private:
        std::vector<Object*> m_objs;
        Coordinates m_coords;// Todas as coordenadas lidas do arquivo
        ColorReader m_cReader;
        bool m_usingColorsFile = false;// Existe uma chamada 'mtllib' no .obj?
        int m_numSubObjs = 0; // Numero de objetos em seguida com o mesmo nome

        ObjType m_freeFormType = ObjType::OBJECT;

        std::string m_faceName = "";
        FaceList m_faces;
};

class ObjWriter : public ObjStream
{
    public:
        ObjWriter(std::string& filename);
        void writeObjs(World *world);

    private:
        void printObj(Object* obj);
        void printObj3D(Object3D* obj);
        void printCurve(Curve* obj);
        void printSurface(BezierSurface* obj);
        void printSurface(BSplineSurface* obj);

    private:
        int m_numVertex = 0;// Numero de coordenadas ja escritas
        ColorWriter m_cWriter;
};

ObjStream::ObjStream(std::string& filename) {
    if(!filename.find("\\.obj"))
        filename += ".obj";

    std::size_t found = filename.find_last_of("/\\");
    m_path = filename.substr(0,found+1);
    m_name = filename.substr(found+1, filename.size()-found-5);// Nome sem o '.obj'
}

ObjReader::ObjReader(std::string& filename):
    ObjStream(filename){

    m_objsFile.open(filename.c_str());
    if(!m_objsFile.is_open())
        throw MyException("Erro tentando abrir o arquivo:\n\t" + filename + ".\n");
    else
        loadObjs();
}

void ObjReader::destroyObjs(){
    for(auto o : m_objs)
        delete o;
}

void ObjReader::addObj3D(){
    std::string name = m_numSubObjs == 0 ? m_name :
        m_name+"_sub"+std::to_string(m_numSubObjs);

    // cria um obj3D usando as 'faces' ja carregadas
    m_objs.push_back(new Object3D(name, m_faces));
    m_faces.clear();
}

void ObjReader::loadObjs(){
    std::string tmp, keyWord;
    while(std::getline(m_objsFile, tmp)){
        if(tmp.size() <= 1) continue;
        std::stringstream line(tmp);
        line >> keyWord;

        if(keyWord == "#")              { /* Não faz nada... */ }
        else if(keyWord == "mtllib")    { loadColorsFile(line); }                       // mtllib filename
        else if(keyWord == "usemtl")    { if(m_usingColorsFile){changeColor(line);} }   // usemtl material_name
        else if(keyWord == "v")         { addCoord(line); }                             // v x y z
        else if(keyWord == "o")         { setName(line); }                              // o obj_name
        else if(keyWord == "w")         { /* TODO(?) */ }
        else if(keyWord == "p")         { addPoint(line); }                             // p v1 v2 v3 ...
        else if(keyWord == "l")         { addPoly(line, false); }                       // l v1 v2 v3 ...
        else if(keyWord == "f")         { addFace(line); }                              // f v1/vt1/vn1 v2/vt2/vn2 ...
        else if(keyWord == "curv")      { addCurve(line); }                             // curv u1 u2 v1 v2 v3 ...

        else if(keyWord == "surf")      { addSurface(line,ObjType::OBJECT); }           // surf  s0  s1  t0  t1  v1/vt1/vn1   v2/vt2/vn2 . . .
        else if(keyWord == "bzp")       { addSurface(line,ObjType::BEZIER_SURFACE); }   // bzp v1 v2 . . . v16 //bezier
        else if(keyWord == "bsp")       { addSurface(line,ObjType::BSPLINE_SURFACE); }  // bsp v1 v2 . . . v16 //bspline

        else if(keyWord == "cstype")    { setFreeFormType(line); }                      // cstype type

        else if(keyWord == "end")       { /* Não intendi o que os 'param' significam,
                                              então só estou lendo o 'end' e não estou
                                              fazendo nada...*/ }
        else if(keyWord == "deg")       { /* Não faz nada... */ }                       // deg degu
        else if(keyWord == "g")         { /* Não faz nada... */ }                       // g group_name
        else if(keyWord == "vt")        { /* Não faz nada... */ }                       // vt u v w
        else if(keyWord == "vn")        { /* Não faz nada... */ }                       // vn i j k
        else if(keyWord == "vp")        { /* Não faz nada... */ }                       // vp u v w
    }
    // Se chegar ao final e tiver alguma
    //  Face salva, ela pertence ao ultimo
    //  objeto 3D
    if(m_faces.size() != 0)
        addObj3D();
}

void ObjReader::setName(std::stringstream& line){
    // Caso o nome do objeto mude, deve-se checar
    //  se ja foi carregado alguma Face. Caso tenha sido,
    //  deve-se então criar o objeto 3D com as Faces
    //  carregadas até agora
    if(m_faces.size() != 0)
        addObj3D();

    line >> m_name;
    m_numSubObjs = 0;
}

void ObjReader::loadColorsFile(std::stringstream& line){
    std::string file;
    line >> file;
    m_usingColorsFile = m_cReader.loadFile(m_path+file);
}

void ObjReader::changeColor(std::stringstream& line){
    std::string colorName;
    line >> colorName;
    m_color = m_cReader.getColor(colorName);
}

void ObjReader::addCoord(std::stringstream& line){
    double x=0.0,y=0.0,z=0.0;
    line >> x >> y >> z;
    m_coords.emplace_back(x,y,z);
}

void ObjReader::addPoint(std::stringstream& line){
    if(m_faces.size() != 0)
        addObj3D();

    Coordinates objCoords;
    loadCoordsIndexes(line, objCoords);

    std::string name = m_numSubObjs == 0 ? m_name :
        m_name+"_sub"+std::to_string(m_numSubObjs);

    // Pode-se declarar varios pontos
    //  em uma mesma linha 'p'
    for(auto &c : objCoords){
        m_objs.push_back(new Point(name, m_color, c));
        m_numSubObjs++;
        name = m_name+"_sub"+std::to_string(m_numSubObjs);
    }
}

void ObjReader::addPoly(std::stringstream& line, bool filled){
    if(m_faces.size() != 0)
        addObj3D();

    Coordinates objCoords;
    loadCoordsIndexes(line, objCoords);

    std::string name = m_numSubObjs == 0 ? m_name :
        m_name+"_sub"+std::to_string(m_numSubObjs);

    if(objCoords.size() == 2)
        m_objs.push_back(new Line(name, m_color, objCoords));
    else
        m_objs.push_back(new Polygon(name, m_color, filled, objCoords));
    m_numSubObjs++;
}

void ObjReader::addFace(std::stringstream& line){
    Coordinates objCoords;
    loadCoordsIndexes(line, objCoords);

    std::string name = "face"+std::to_string(m_faces.size()+1);
    m_faces.emplace_back(name, objCoords);
}

void ObjReader::addCurve(std::stringstream& line){
    if(m_freeFormType == ObjType::OBJECT){
        destroyObjs();
        throw MyException("Tentativa de criar uma curva sem 'cstype' na linha: "+
                          line.str() +".\n");
    }

    if(m_faces.size() != 0)
        addObj3D();

    double tmp=0.0;
    line >> tmp;// Remove o u1 e u2 que não sei para que servem...
    line >> tmp;

    Coordinates objCoords;
    loadCoordsIndexes(line, objCoords);

    std::string name = m_numSubObjs == 0 ? m_name :
        m_name+"_sub"+std::to_string(m_numSubObjs);

    if(m_freeFormType == ObjType::BEZIER_CURVE){
        if(objCoords.size() < 4 || (objCoords.size()-4)%3 != 0){
            destroyObjs();
            throw MyException("Uma curva de Bezier deve ter 4, 7, 10, 13... coordenadas.");
        }

        m_objs.push_back(new BezierCurve(name, m_color, objCoords));
    }else if(m_freeFormType == ObjType::BSPLINE_CURVE){
        if(objCoords.size() < 4){
            destroyObjs();
            throw MyException("Uma curva B-Spline deve ter no minimo 4 coordenadas.");
        }

        m_objs.push_back(new BSplineCurve(name, m_color, objCoords));
    }
    m_numSubObjs++;
}

void ObjReader::addSurface(std::stringstream& line, ObjType type){
    if(type == ObjType::OBJECT){//parametro 'surf'
        if(m_freeFormType == ObjType::OBJECT){
            destroyObjs();
            throw MyException("Tentativa de criar uma superficie sem 'cstype' na linha: "+
                              line.str() +".\n");
        }

        double tmp=0.0;
        line >> tmp;// Remove o s1,s2,t1,t2...
        line >> tmp;
        line >> tmp;
        line >> tmp;
    }

    Coordinates objCoords;
    loadCoordsIndexes(line, objCoords);

    std::string name = m_numSubObjs == 0 ? m_name :
        m_name+"_sub"+std::to_string(m_numSubObjs);

    if(objCoords.size() < 16){
        destroyObjs();
        throw MyException("Superficies devem ter 16 coordenadas.");
    }else if(objCoords.size() > 16){
        destroyObjs();
        throw MyException(
            "Para formar mais do que uma superficie, declare as superficies compartilhando os vertices em comum.");
    }

    if(type == ObjType::BEZIER_SURFACE ||
       (type == ObjType::OBJECT && m_freeFormType == ObjType::BEZIER_CURVE)){
        m_objs.push_back(new BezierSurface(name, m_color, objCoords));
    }else if(type == ObjType::BSPLINE_SURFACE ||
       (type == ObjType::OBJECT && m_freeFormType == ObjType::BSPLINE_CURVE)){
        m_objs.push_back(new BSplineSurface(name, m_color, objCoords));
    }

    m_numSubObjs++;
}

void ObjReader::loadCoordsIndexes(std::stringstream& line, Coordinates& objCoords){
    std::string pointString;
    int index = 0;
    int size = m_coords.size();

    while(true){
        while(line >> pointString){
            // Algoritmo vai pegar o vertice 'v' e vai
            //  ignorar os outros [v/vt/vn]
            std::stringstream point(pointString);

            if(point.str() == "\\")
                continue;

            point >> index;

            if(!point){// É obrigado a ter um vertice
                destroyObjs();
                throw MyException("Indice de vertice invalido na linha: "+ line.str() + ".\n");
            }

            // Processa o index do vertice
            if(index < 0)
                index = size + index;
            else
                index--;

            if(index < 0 || index >= size){
                destroyObjs();
                throw MyException("Indice de vertice invalido na linha: "+ line.str() + ".\n");
            }
            objCoords.push_back(m_coords[index]);
        }
        if(line.str().find("\\") == std::string::npos)
            break;
        else{
            // Pegue a proxima linha caso ache '\'
            //  ao final da linha atual
            std::string tmp;
            if(std::getline(m_objsFile, tmp)){
                //line = std::stringstream(tmp);
                line.str(tmp);
                line.clear();
            }else
                break;
        }
    }
}

void ObjReader::setFreeFormType(std::stringstream& line){
    std::string type;
    line >> type;

    if(type == "rat"){ line >> type; }// Nao intendi bem o que isto quer dizer...

    if(type == "bezier"){ m_freeFormType = ObjType::BEZIER_CURVE; }
    else if(type == "bspline"){ m_freeFormType = ObjType::BSPLINE_CURVE; }
    else{
        destroyObjs();
        throw MyException("cstype igual a: '"+
                        type+"', nao eh suportado por esta aplicacao.\n");
    }
}

ObjWriter::ObjWriter(std::string& filename):
    ObjStream(filename) {

    m_objsFile.open(filename.c_str(), std::fstream::out);

    if(!m_objsFile.is_open())
        throw MyException("Erro tentando abrir arquivo:\n\t" + filename + ".\n");
}

void ObjWriter::writeObjs(World *world){
    m_cWriter.loadFile(m_path+m_name+".mtl");
    m_objsFile << "mtllib " << m_name << ".mtl\n\n";

    Object *obj;
    int size = world->numObjs();
    for(int i = 0; i<size; i++){
        obj = world->getObj(i);
        switch(obj->getType()){
        case ObjType::OBJECT3D:
            printObj3D((Object3D*) obj);
            break;
        case ObjType::BEZIER_SURFACE:
            printSurface((BezierSurface*) obj);
            break;
        case ObjType::BSPLINE_SURFACE:
            printSurface((BSplineSurface*) obj);
            break;
        case ObjType::BEZIER_CURVE:
        case ObjType::BSPLINE_CURVE:
            printCurve((Curve*) obj);
            break;
        default:
            printObj(obj);
        }
    }
}

void ObjWriter::printObj(Object* obj){
    auto &coords = obj->getCoords();

    for(const auto &c : coords)
        m_objsFile << "v " << c.x << " " << c.y << " " << c.z << "\n";

    m_objsFile << "\no " << obj->getName() << "\n";

    const std::string colorName = m_cWriter.getColorName(obj->getColor());
    if(colorName != "none")
        m_objsFile << "usemtl " << colorName << "\n";

    // Só printa pontos, linhas e poligonos
    if(obj->getType() == ObjType::POINT)
        m_objsFile << "p";
    else
        m_objsFile << "l";

    int size = coords.size();
    for(int i = 0; i<size; i++)
        m_objsFile << " " << m_numVertex+(i+1);

    m_objsFile << "\n" << std::endl;
    m_numVertex += size;
}

void ObjWriter::printObj3D(Object3D* obj){

    for(auto face : obj->getFaceList()){
        auto &coords = face.getCoords();
        for(const auto &c : coords)
            m_objsFile << "v " << c.x << " " << c.y << " " << c.z << "\n";
    }
    m_objsFile << "\no " << obj->getName() << "\n";

    unsigned int faceSize = 0;
    for(auto &face : obj->getFaceList()){
        faceSize = face.getCoords().size();

        const std::string colorName = m_cWriter.getColorName(face.getColor());
        if(colorName != "none")
            m_objsFile << "usemtl " << colorName << "\n";

        m_objsFile << "f";
        for(unsigned int i = 0; i<faceSize; i++){
            m_objsFile << " " << m_numVertex+(i+1);
        }
        m_objsFile << "\n";
        m_numVertex += faceSize;
    }
}

void ObjWriter::printCurve(Curve* obj){
    auto &coords = obj->getControlPoints();

    for(const auto &c : coords)
        m_objsFile << "v " << c.x << " " << c.y << " " << c.z << "\n";

    m_objsFile << "\no " << obj->getName() << "\n";

    const std::string colorName = m_cWriter.getColorName(obj->getColor());
    if(colorName != "none")
        m_objsFile << "usemtl " << colorName << "\n";

    if(obj->getType() == ObjType::BEZIER_CURVE)
        m_objsFile << "cstype bezier\n";
    else
        m_objsFile << "cstype bspline\n";

    m_objsFile << "deg 3\n";
    m_objsFile << "curv 0.0 0.0";

    int size = coords.size();
    for(int i = 0; i<size; i++)
        m_objsFile << " " << m_numVertex+(i+1);

    m_objsFile << "\nend\n" << std::endl;
    m_numVertex += size;
}

void ObjWriter::printSurface(BezierSurface* obj){
    for(const auto& c : obj->getControlPoints()){
        m_objsFile << "v " << c.x << " " << c.y << " " << c.z << "\n";
    }

    m_objsFile << "\no " << obj->getName() << "\n";

    const std::string colorName = m_cWriter.getColorName(obj->getColor());
    if(colorName != "none")
        m_objsFile << "usemtl " << colorName << "\n";

    m_objsFile << "cstype bezier\n";
    m_objsFile << "deg 3 3\n";

    int maxLines = obj->getMaxLines(),
        maxCols = obj->getMaxCols();
    int tmp3xMaxLines = 3*maxLines,
        tmp3xMaxCols = 3*maxCols;
    for(int nLine = 0; (maxLines != 4 && nLine <= tmp3xMaxLines) ||
            (nLine < tmp3xMaxLines); nLine += tmp3xMaxCols){
        for(int nCol = 0; nCol < maxCols-1; nCol += 3){
            m_objsFile << "surf 0.0 1.0 0.0 1.0";
            for(int i = 0; i < 4; i++){
                m_objsFile << " " << m_numVertex+(i*maxCols)  +nLine+nCol+1;
                m_objsFile << " " << m_numVertex+(i*maxCols)+1+nLine+nCol+1;
                m_objsFile << " " << m_numVertex+(i*maxCols)+2+nLine+nCol+1;
                m_objsFile << " " << m_numVertex+(i*maxCols)+3+nLine+nCol+1;
            }
            m_objsFile << "\nend\n\n";
        }
    }
    m_numVertex += maxLines*maxCols;
}

void ObjWriter::printSurface(BSplineSurface* obj){
    for(const auto& c : obj->getControlPoints()){
        m_objsFile << "v " << c.x << " " << c.y << " " << c.z << "\n";
    }

    m_objsFile << "\no " << obj->getName() << "\n";

    const std::string colorName = m_cWriter.getColorName(obj->getColor());
    if(colorName != "none")
        m_objsFile << "usemtl " << colorName << "\n";

    m_objsFile << "cstype bspline\n";
    m_objsFile << "deg 3 3\n";

    int maxLines = obj->getMaxLines(),
        maxCols = obj->getMaxCols();
    for(int nLine = 0; (maxLines != 4 && nLine <= maxLines) ||
                (nLine < maxLines); nLine += maxCols){
        for(int nCol = 0; nCol <= maxCols-4; nCol += 1){
            m_objsFile << "surf 0.0 1.0 0.0 1.0";
            for(int i = 0; i < 4; i++){
                m_objsFile << " " << m_numVertex+(i*maxCols)  +nLine+nCol+1;
                m_objsFile << " " << m_numVertex+(i*maxCols)+1+nLine+nCol+1;
                m_objsFile << " " << m_numVertex+(i*maxCols)+2+nLine+nCol+1;
                m_objsFile << " " << m_numVertex+(i*maxCols)+3+nLine+nCol+1;
            }
            m_objsFile << "\nend\n\n";
        }
    }
    m_numVertex += maxLines*maxCols;
}

bool ColorReader::loadFile(const std::string& filename){
    m_ifs.open(filename);

    if(!m_ifs.is_open()){ //dar throw?
        std::cout << "Erro tentando abrir arquivo:\n\t" + filename + ".\n";
        return false;
    }else{
        loadColors();
        return true;
    }
}

void ColorReader::loadColors(){
    std::string tmp, keyWord, colorName;
    while(std::getline(m_ifs, tmp)){
        if(tmp.size() <= 1) continue;
        std::stringstream line(tmp);
        line >> keyWord;

        if(keyWord == "newmtl"){ line >> colorName; }
        if(keyWord == "Kd"){ addColor(colorName, line); }
    }
}

void ColorReader::addColor(std::string name, std::stringstream& line){
    double r=0,g=0,b=0;
    line >> r >> g >> b;
    m_colors[name] = GdkRGBA{r,g,b};
}

const GdkRGBA& ColorReader::getColor(const std::string& colorName) const{
    auto iter = m_colors.find(colorName);
    if(iter != m_colors.end())
        return iter->second;
    else{
        auto iter2 = m_colors.find("Padrao");
        return iter2->second;
    }
}

void ColorWriter::loadFile(const std::string& filename){
    m_ofs.open(filename);

    if(!m_ofs.is_open()) //dar throw?
        std::cout << "Erro tentando abrir arquivo:\n\t" + filename + ".\n";
}

std::string ColorWriter::getColorName(const GdkRGBA& color){
    if(!m_ofs.is_open())
        return "none";

    std::string colorString = gdk_rgba_to_string(&color);

    auto iter = m_colors.find(colorString);
    if(iter != m_colors.end())
        return iter->second;
    else{ //Se nao existir, cria um novo nome para a cor, e retorna ele...
        std::string colorName = "Color" + std::to_string(m_numColors++);

        m_colors[colorString] = colorName;
        writeColor(colorName, color);
        return colorName;
    }
}

void ColorWriter::writeColor(const std::string& colorName, const GdkRGBA& color){
    m_ofs << "newmtl " << colorName << "\n";
    m_ofs << "Kd " << color.red << " " << color.green << " " << color.blue << "\n\n";
}

#endif // FILEHANDLERS_HPP