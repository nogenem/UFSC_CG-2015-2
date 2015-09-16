#ifndef DISPLAYFILE_HPP
#define DISPLAYFILE_HPP

#include "ListaEnc.hpp"
#include "Objects.hpp"
#include "MyException.hpp"

class DisplayFile : public ListaEnc<Object*>
{
    public:
        DisplayFile(){}
        virtual ~DisplayFile() {}

        Object* getObj(int pos){ return getFromPos(pos); }
        Object* getObj(Object *obj){ return *posicaoMem(obj); }
        Elemento<Object*>* getFirstElement(){ return getHead(); }

        void addObj(Object *obj){ adiciona(obj); }
        void removeObj(Object *obj){ delete retiraEspecifico(obj); }
        int size() const { return getSize(); }
        bool contains(Object *obj){ return contem(obj); }
};

#endif // DISPLAYFILE_HPP
