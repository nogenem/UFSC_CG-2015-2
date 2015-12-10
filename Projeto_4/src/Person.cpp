#include "Person.h"
#include "Geometry.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

Person::Person(int t) :
    m_head(), m_trunk(), m_leftArm(BodyPart::Side::LEFT),
    m_rightArm(BodyPart::Side::RIGHT), m_leftLeg(BodyPart::Side::LEFT),
    m_rightLeg(BodyPart::Side::RIGHT) {

    // Seta a pos inicial
    resetPersonPos();
}

Person::~Person(){
    //dtor
}

void Person::resetPersonPos(){
    m_pos.x = 0.0;
    m_pos.y = (m_leftLeg.totalLength+(m_trunk.totalLength/2.0f)) - .08; //- .08 [calculo ta errado?]
    m_pos.z = 0.0;

    m_rotation = 0.0;
}

void Person::setMovimentType(MovementType type){
    m_movType = type;

    if(type == MovementType::WALKING)
        m_velocity = .2;
    else if(type == MovementType::RUNNING)
        m_velocity = .5;
}

void Person::updatePos(){
    m_pos.x += m_velocity * sin( m_rotation * M_PI / 180 );
    m_pos.z += m_velocity * cos( m_rotation * M_PI / 180 );
}

// Código do exemplo 3.7 do pdf '3d_case_study_using_opengl'
double Person::m_calculateDisplacement(int t){
    // Left full leg
    double  lLength = m_leftLeg.totalLength;
    double  lAngleUp = m_leftLeg.thigh.walkAngles[(int)m_movType][(int)BodyPart::Side::LEFT][t];
    double  lAngleDown = m_leftLeg.leg.walkAngles[(int)m_movType][(int)BodyPart::Side::LEFT][t];

    double  lRho = (M_PI * lAngleUp) / 180.0,
            lPhi = (M_PI * lAngleDown - lAngleUp) / 180.0;

    double result1 = lLength - ( (m_leftLeg.thigh.totalLength * cos(lRho)) +
                                ((m_leftLeg.leg.totalLength+m_leftLeg.foot.totalLength) * cos(lPhi)) );

    // Right full leg
    double  rLength = m_rightLeg.totalLength;
    double  rAngleUp = m_rightLeg.thigh.walkAngles[(int)m_movType][(int)BodyPart::Side::RIGHT][t];
    double  rAngleDown = m_rightLeg.leg.walkAngles[(int)m_movType][(int)BodyPart::Side::RIGHT][t];

    double  rRho = M_PI * rAngleUp / 180.0,
            rPhi = (M_PI * rAngleDown - rAngleUp) / 180.0;

    double result2 = rLength - ( (m_rightLeg.thigh.totalLength * cos(rRho)) +
                                 ((m_rightLeg.leg.totalLength+m_rightLeg.foot.totalLength) * cos(rPhi)) );

    // Displacement
    return (result1 <= result2 ? -result1 : -result2);
}

inline double toRadius(double angle){ return (M_PI * angle) / 180.0; }
// Código da imagem 3.7 'Finding the vertical displacement'
//  do pdf '3d_case_study_using_opengl'
/*double Person::m_calculateDisplacement(int t){
    // Left full leg
    double  lLength = m_leftLeg.totalLength;
    double  lRho = toRadius(m_leftLeg.thigh.walkAngles[(int)m_movType][(int)BodyPart::Side::LEFT][t]),
            lTheta = toRadius(m_leftLeg.leg.walkAngles[(int)m_movType][(int)BodyPart::Side::LEFT][t]),
            lPhi = lRho - lTheta;

    double result1 = lLength - ( (m_leftLeg.thigh.totalLength * cos(lRho)) +
                                 ((m_leftLeg.leg.totalLength+m_leftLeg.foot.totalLength) * cos(lPhi)) );

    // Right full leg
    double  rLength = m_rightLeg.totalLength;
    double  rRho = toRadius(m_rightLeg.thigh.walkAngles[(int)m_movType][(int)BodyPart::Side::RIGHT][t]),
            rTheta = toRadius(m_rightLeg.leg.walkAngles[(int)m_movType][(int)BodyPart::Side::RIGHT][t]),
            rPhi = rRho - rTheta;

    double result2 = rLength - ( (m_rightLeg.thigh.totalLength * cos(rRho)) +
                                 ((m_rightLeg.leg.totalLength+m_rightLeg.foot.totalLength) * cos(rPhi)) );

    std::cout << "R1: " << result1 << " - R2: " << result2 << "\n";
    // Displacement
    return (result1 <= result2 ? -result1 : -result2);
}*/

void Person::draw(int &t){
    double d = 0.0;
    if(m_movType == MovementType::WALKING){
        if(t == -1){ t = 7; }
        t = t % 8;
        d = displacement[(int)m_movType][t%4];
    }else if(m_movType == MovementType::RUNNING){
        if(t == -1){ t = 11; }
        t = t % 12;
        d = displacement[(int)m_movType][t%6];
    }

    std::cout << "t: " << t << "\n";

    if(m_useDisplacementFuncion)
        d = m_calculateDisplacement(t);

    glPushMatrix();
        glColor3f(1.0, 0.0, 0.0);
        glTranslatef(m_pos.x, m_pos.y+d, m_pos.z);
        glRotatef(m_rotation, 0.0, 1.0, 0.0);

        // Trunk
        m_trunk.draw(t, m_movType);

        // Head
        glPushMatrix();
            glTranslatef(0.0, (m_trunk.height/2.0f)+m_head.neckLenght, 0.0);
            m_head.draw(t, m_movType);
        glPopMatrix();

        // Left FullArm
        glPushMatrix();
            glColor3f(0.0, 1.0, 0.0);
            glTranslatef((m_trunk.width/2.0f)+m_leftArm.arm.jointRadius,
                         (m_trunk.height/2.0f)-m_leftArm.arm.jointRadius, 0.0);
            m_leftArm.draw(t, m_movType);
        glPopMatrix();

        // Right FullArm
        glPushMatrix();
            glColor3f(0.0, 0.0, 1.0);
            glTranslatef(-((m_trunk.width/2.0f)+m_rightArm.arm.jointRadius),
                         (m_trunk.height/2.0f)-m_rightArm.arm.jointRadius, 0.0);
            m_rightArm.draw(t, m_movType);
        glPopMatrix();

        // Left FullLeg
        glPushMatrix();
            glColor3f(0.0, 1.0, 0.0);
            glTranslatef((m_trunk.width/2.0f)-m_leftLeg.thigh.jointRadius,
                         -((m_trunk.height/2.0f)+m_leftLeg.thigh.jointRadius), 0.0);
            m_leftLeg.draw(t, m_movType);
        glPopMatrix();

        // Right FullLeg
        glPushMatrix();
            glColor3f(0.0, 0.0, 1.0);
            glTranslatef(-((m_trunk.width/2.0f)-m_rightLeg.thigh.jointRadius),
                         -((m_trunk.height/2.0f)+m_rightLeg.thigh.jointRadius), 0.0);
            m_rightLeg.draw(t, m_movType);
        glPopMatrix();
    glPopMatrix();
}

void Head::draw(int t, MovementType movType){
    // neck
    Geometry::drawCylinder(neckRadius, neckLenght);

    // head
    glTranslatef(0.0, neckLenght, 0.0);
    Geometry::drawSolidSphere(headRadius);
}

void Trunk::draw(int t, MovementType movType){
    Geometry::drawBox(width, height, depth);
}

void Arm::draw(int t, MovementType movType){
    glRotatef(walkAngles[(int)movType][(int)m_side][t], 1.0, 0.0, 0.0);
    // shoulder joint
    Geometry::drawSolidSphere(jointRadius);
    glTranslatef(0.0, -(jointRadius), 0.0);

    // arm
    Geometry::drawCylinder(armRadius, armLenght);
}

void Forearm::draw(int t, MovementType movType){
    glRotatef(walkAngles[(int)movType][(int)m_side][t], 1.0, 0.0, 0.0);
    // elbow joint
    Geometry::drawSolidSphere(jointRadius);
    glTranslatef(0.0, -(jointRadius), 0.0);

    // forearm
    Geometry::drawCylinder(forearmRadius, forearmLenght);
}

void Hand::draw(int t, MovementType movType){
    glRotatef(walkAngles[(int)movType][(int)m_side][t], 1.0, 0.0, 0.0);
    // wrist joint
    Geometry::drawSolidSphere(jointRadius);
    glTranslatef(0.0, -((jointRadius)+(handHeight/2)), 0.0);

    // hand
    glRotatef(90, 0.0, 1.0, 0.0);
    Geometry::drawBox(handWidth, handHeight, handDepth);
}

void FullArm::draw(int t, MovementType movType){
    // Arm
    arm.draw(t, movType);

    // Forearm
    glTranslatef(0.0, -(arm.armLenght+(forearm.jointRadius)), 0.0);
    forearm.draw(t, movType);

    // Hand
    glTranslatef(0.0, -(forearm.forearmLenght+(hand.jointRadius)), 0.0);
    hand.draw(t, movType);
}

void Thigh::draw(int t, MovementType movType){
    glRotatef(walkAngles[(int)movType][(int)m_side][t], 1.0, 0.0, 0.0);
    // joint
    Geometry::drawSolidSphere(jointRadius);
    glTranslatef(0.0, -(jointRadius), 0.0);

    // thigh
    Geometry::drawCylinder(thighRadius, thighLenght);
}

void Leg::draw(int t, MovementType movType){
    glRotatef(walkAngles[(int)movType][(int)m_side][t], 1.0, 0.0, 0.0);
    // knee
    Geometry::drawSolidSphere(jointRadius);
    glTranslatef(0.0, -(jointRadius), 0.0);

    // leg
    Geometry::drawCylinder(legRadius, legLenght);
}

void Foot::draw(int t, MovementType movType){
    glRotatef(walkAngles[(int)movType][(int)m_side][t], 1.0, 0.0, 0.0);
    // ankle
    Geometry::drawSolidSphere(jointRadius);
    glTranslatef(0.0, -((jointRadius)), jointRadius);

    // foot
    glRotatef(90, 1.0, 0.0, 0.0);
    Geometry::drawBox(footWidth, footHeight, footDepth);
}

void FullLeg::draw(int t, MovementType movType){
    // thigh
    thigh.draw(t, movType);

    // leg
    glTranslatef(0.0, -(thigh.thighLenght+(leg.jointRadius)), 0.0);
    leg.draw(t, movType);

    // pé
    glTranslatef(0.0, -(thigh.thighLenght+(foot.jointRadius)), 0.0);
    foot.draw(t, movType);
}
