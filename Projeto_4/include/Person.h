#ifndef PERSON_H
#define PERSON_H

#include "Vector3.h"
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

enum class MovementType { WALKING, RUNNING };

class BodyPart {
    public:
        enum class Side { LEFT=0, RIGHT=1 };

        // nunca deve ser chamado!
        virtual void draw(int t, MovementType movType){}
};

// cabeça + pescoço
class Head : public BodyPart {
    public:
        void draw(int t, MovementType movType);

        double  headRadius = .4,
                neckRadius = .2,
                neckLenght = .25;

        double totalLength = (2.0f * headRadius + neckLenght);
};

// tronco
class Trunk : public BodyPart {
    public:
        void draw(int t, MovementType movType);

        double  width = 1.0,
                height = 1.8,
                depth = .5;

        double totalLength = height;
};

// Ombro + parte de cima do braço
class Arm : public BodyPart {
    public:
        Arm(BodyPart::Side s) : m_side(s){ }
        void draw(int t, MovementType movType);

        double  jointRadius = .2,
                armLenght = .6,
                armRadius = .2;//.2

        double totalLength = (2.0f * jointRadius + armLenght);

        int walkAngles[2][2][12] = {
            //walking
            {   {-15, -30, 0,  30,      30,  45, 0, -15,     0, 0, 0, 0},       //left
                { 30,  45, 0, -15,     -15, -30, 0,  30,     0, 0, 0, 0}    },  //right

            //running
            {   {-45, -30,   0, 15,  30,  60,      75,  60,  30,  0, -15, -30},       //left
                { 75,  60,  30,  0, -15, -30,     -45, -30,   0, 15,  30,  60}    }   //right
        };
    private:
        BodyPart::Side m_side;
};

// Antebraço + cotovelo
class Forearm : public BodyPart {
    public:
        Forearm(BodyPart::Side s) : m_side(s){}
        void draw(int t, MovementType movType);

        double  jointRadius = .15,
                forearmLenght = .6,
                forearmRadius = .15;

        double totalLength = (2.0f * jointRadius + forearmLenght);

        int walkAngles[2][2][12] = {
            //walking
            {   {-30, -15, -5, -45,     -15, -20, -5, -15,     0, 0, 0, 0},     //left
                {-15, -20, -5, -15,     -30, -15, -5, -45,     0, 0, 0, 0}  },  //right

            //running
            {   {-90, -90, -45, -60, -60, -60,     -60, -60, -30, -60, -90, -90},     //left
                {-60, -60, -30, -60, -90, -90,     -90, -90, -45, -60, -60, -60}  }   //right
        };
    private:
        BodyPart::Side m_side;
};

// mão + junta do pulso
class Hand : public BodyPart {
    public:
        Hand(BodyPart::Side s) : m_side(s){}
        void draw(int t, MovementType movType);

        double  jointRadius = .1,//.25
                handWidth = .3,
                handHeight = .45,//.25
                handDepth = .15;

        double totalLength = (2.0f * jointRadius + handHeight);

        int walkAngles[2][2][12] = {
            //walking
            {   {0, 0, 0, 0,     0, 0, 0, 0,     0, 0, 0, 0},      //left
                {0, 0, 0, 0,     0, 0, 0, 0,     0, 0, 0, 0}   },  //right

            //running
            {   {0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0},      //left
                {0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0}   }   //right
        };
    private:
        BodyPart::Side m_side;
};

// braço + antebraço + mão
class FullArm : public BodyPart {
    public:
        FullArm(BodyPart::Side s) :
            arm(s), forearm(s), hand(s), m_side(s){ }
        void draw(int t, MovementType movType);

    public:
        Arm arm;
        Forearm forearm;
        Hand hand;

        double totalLength = (arm.totalLength + forearm.totalLength + hand.totalLength);

    private:
        BodyPart::Side m_side;
};

// coxa + junta do quadril
class Thigh : public BodyPart {
    public:
        Thigh(BodyPart::Side s) : m_side(s){}
        void draw(int t, MovementType movType);

        double  jointRadius = .2,
                thighLenght = .6,
                thighRadius = .2;//.2

        double totalLength = (2.0f * jointRadius + thighLenght);

        int walkAngles[2][2][12] = {
            //walking
            {   { 27,  38, -15, -34,     -30, -53,   0,  35,     0, 0, 0, 0},       //left
                {-30, -53,   0,  35,      27,  38, -15, -34,     0, 0, 0, 0}    },  //right

            //running
            {   { 40,  18, -39, -61, -78, -79,     -37, -50, -30,  24,  48,  40},       //left
                {-37, -50, -30,  24,  44,  40,      40,  18, -39, -61, -78, -79}    }   //right
        };
    private:
        BodyPart::Side m_side;
};

// parte de baixo da perna + joelho
class Leg : public BodyPart {
    public:
        Leg(BodyPart::Side s) : m_side(s){}
        void draw(int t, MovementType movType);

        double  jointRadius = .15,
                legLenght = .6,
                legRadius = .15;//.2

        double totalLength = (2.0f * jointRadius + legLenght);

        int walkAngles[2][2][12] = {
            //walking
            {   {27, 35, 75, 79,      0, 65,  0,  0,     0, 0, 0, 0},       //left
                { 0, 65,  0,  0,     27, 35, 75, 79,     0, 0, 0, 0}    },  //right

            //running
            {   {101, 100, 117, 96, 71, 60,       0,  50,  80, 15,  0, 45},        //left
                {  0,  50,  80, 15,  0, 45,     101, 100, 117, 96, 71, 60}    }    //right
        };
    private:
        BodyPart::Side m_side;
};

// pé + tornozelo
class Foot : public BodyPart {
    public:
        Foot(BodyPart::Side s) : m_side(s){}
        void draw(int t, MovementType movType);

        double  jointRadius = .1,//.25
                footWidth = .3,
                footHeight = .6,//.25
                footDepth = .15;

        // É usado o depth porque o pé é rotacionado
        double totalLength = (2.0f * jointRadius + footDepth);

        int walkAngles[2][2][12] = {
            //walking
            {   {15,  15, 15, 0,      0, -15,  0, 0,     0, 0, 0, 0},       //left
                { 0, -15,  0, 0,     15,  15, 15, 0,     0, 0, 0, 0}    },  //right

            //running
            {   {0, 15,  15, 0,  0, 0,     0,  0, -45, 0, 30, 0},       //left
                {0,  0, -45, 0, 30, 0,     0, 15,  15, 0,  0, 0}    }   //right
        };
    private:
        BodyPart::Side m_side;
};

// coxa + parte de baixo da perna + pé
class FullLeg : public BodyPart {
    public:
        FullLeg(BodyPart::Side s) :
            thigh(s), leg(s), foot(s), m_side(s){}
        void draw(int t, MovementType movType);

    public:
        Thigh thigh;
        Leg leg;
        Foot foot;

        double totalLength = (thigh.totalLength + leg.totalLength + foot.totalLength);

    private:
        BodyPart::Side m_side;
};

// cabeça + tronco + braços + pernas
class Person
{
    public:
        Person(int t);
        virtual ~Person();

        void draw(int &t);

        void addRotation(float rot){ m_rotation += rot; }

        void resetPersonPos();

        void setMovimentType(MovementType mode);

        void movingFoward(){ m_velocity = std::abs(m_velocity); }
        void movingBack(){ m_velocity = -std::abs(m_velocity); }

        void setUseDisplacementFunc(bool v){ m_useDisplacementFuncion = v; }

        void updatePos();
    private:
        double m_calculateDisplacement(int t);

    private:
        Head m_head;
        Trunk m_trunk;
        FullArm m_leftArm;
        FullArm m_rightArm;
        FullLeg m_leftLeg;
        FullLeg m_rightLeg;

        // tamanho total do boneco
        double m_totalLength = (m_head.totalLength+m_trunk.totalLength+m_leftLeg.totalLength);

        // rotação do corpo
        float m_rotation = 0.0;

        // pos inicial do boneco, altura até metade do tronco
        Vector3 m_pos;

        // tipo de movimento, andando ou correndo
        MovementType m_movType = MovementType::WALKING;

        // velocidade de movimento do boneco
        float m_velocity = .2;//.2 = walking, .5 = running

        // usar a função de displacement do pdf?
        bool m_useDisplacementFuncion = false;

        // fake displacement
        double displacement[2][6] = {
            {-.21, -.43,    0, -.15,    0,    0},   //walk
            {-.31, -.34, -.43, -.12, -.14, -.16}    //run
        };
};

#endif // PERSON_H
