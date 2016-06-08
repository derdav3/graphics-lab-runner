//
//  Controller.h
//  bRenderer_ios
//
//  Created by Orell Basil Endres (oendre) on 02/05/16.
//
//

#ifndef bRenderer_ios_Controller_h
#define bRenderer_ios_Controller_h

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>


#include "GameController.h"
#include "bRenderer.h"
#include "Entity.h"
#include "Player.h"
#include "LevelBuilder.h"


class Controller {
private:
    std::list<Entity> entities;
    std::list<MoveableEntity> moveableEntities;
    std::vector<Entity> buttons;
    
    Entity test;
    GameController gameController;
    Renderer brenderer;
    MoveableEntity player;
    Entity skyplane;
    bool jumpable;
    double timeSinceLast;
    int totalGoldcoins;
    int totalSilvercoins;
    double lastUpdate;
    int iteration;

    float GameTime = 0.0;
    bool goingUp = true;

    float timeRunning;
    
    void drawEntity(Entity entity, vmml::Matrix4f viewMatrix, vmml::Matrix4f projectionMatrix,double left, double top, vmml::Vector4f borders) {
        vmml::Matrix4f modelMatrix = entity.getPos();
        std::string shaderName = entity.getShaderName();
        std::string objName = entity.getObjName();
        //        std::cout<<"SHADER: "<<shaderName<<std::endl;
        
        ShaderPtr shader = brenderer.getObjects()->getShader(shaderName);
        
        
        if (shader.get())
        {
            std::cout<<"PRINT SHADER "<<shaderName<<std::endl;
            shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
            shader->setUniform("ViewMatrix", viewMatrix);
            shader->setUniform("ModelMatrix", modelMatrix);
            
            vmml::Matrix3f normalMatrix;
            vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrix)), normalMatrix);
            shader->setUniform("NormalMatrix", normalMatrix);
            
            //This is the same as setting the camera
            vmml::Vector4f eyePos = getPlayerTrans() - vmml::Vector3f(0.0f,0.0f,-10.0f);
            
            shader->setUniform("EyePos", eyePos);
            
            //Setting up the sun - so it walks with him just a little bit
            float f = (GameTime - 0.1)*(1/0.8);
            std::cout<<"SUN POSITION: "<<f<<std::endl;
            vmml::Vector4f lightPos = vmml::Vector4f(-30.0 + eyePos.x()/2 , 25.0 , 1.0 , 1.0 );
            
            shader->setUniform("LightPos", lightPos);
            
            shader->setUniform("Ia", vmml::Vector3f(1.f));
            shader->setUniform("Id", vmml::Vector3f(1.f));
            shader->setUniform("borders", borders);
            shader->setUniform("Is", vmml::Vector3f(1.f));
            shader->setUniform("moveLeft",left*50);
            shader->setUniform("moveTop",top*50);
            //shader->setUniform("borders",borders);
            shader->setUniform("GameTime", GameTime);
            
            vmml::Vector4f PlayerPos = -getPlayerPosition();
            
            shader->setUniform("PlayerPosition", PlayerPos);
            
            //std::cout<<"POSITION "<<shader->getAttribLocation("Position")<<std::endl;
            std::cout << "PLAYER POS " << getPlayerPosition() << std::endl;
            
        }
        else
        {
            bRenderer::log("No shader available.");
        }
        //GLfloat titleScale = 2.0f;
        //vmml::Matrix4f scaling = vmml::create_scaling(vmml::Vector3f(titleScale / brenderer.getView()->getAspectRatio(), titleScale, titleScale));
        //modelMatrix = vmml::create_translation(vmml::Vector3f(-0.4f, 0.0f, -10.0f)) * scaling;
        brenderer.getModelRenderer()->drawModel(brenderer.getObjects()->getModel(objName), modelMatrix,viewMatrix, projectionMatrix, std::vector<std::string>({ }),false,false);
        //brenderer.getModelRenderer()->drawModel(objName, "camera", modelMatrix, std::vector<std::string>({ }));
        //         std::cout << "object " << objName << brenderer.getObjects()->getModel("block")->getBoundingBoxObjectSpace();

    }

    void drawEntity(Entity entity) {
        vmml::Matrix4f viewMatrix = brenderer.getObjects()->getCamera("camera")->getViewMatrix();
        vmml::Matrix4f projectionMatrix = brenderer.getObjects()->getCamera("camera")->getProjectionMatrix();
        drawEntity(entity,viewMatrix,projectionMatrix,0,0,vmml::Vector4f(0.0f,1.0f,0.0f,1.0f));
    }
    
    vmml::Vector<4,bool> checkCollision(vmml::Vector3f oldCenter, vmml::Vector3f newCenter, vmml::Vector3f boxMin,vmml::Vector3f boxMax, vmml::Vector3f posMin, vmml::Vector3f posMax) {
        bool left= false;
        bool right = false;
        bool top = false;
        bool bottom = false;
        //bool yCollision = boxMax.at(1) > posMin.at(1) && boxMin.at(1) < posMax.at(1);
        //bool xCollision = boxMax.at(0) > posMin.at(0);
        bool xCollision = (boxMax.at(0) > posMin.at(0) && boxMin.at(0) < posMin.at(0)) || (boxMax.at(0) > posMax.at(0) && boxMin.at(0) < posMax.at(0));
        bool yCollision = (boxMax.at(1) > posMin.at(1) && boxMin.at(1) < posMin.at(1)) || (boxMax.at(1) > posMax.at(1) && boxMin.at(1) < posMax.at(1));
        if (yCollision) {
            if (boxMax.at(1) > posMin.at(1) && boxMin.at(1) < posMin.at(1)) {
                top = true;
            } else {
                bottom = true;
            }
        }
        if (xCollision) {
            if (boxMax.at(0) > posMin.at(0) && boxMin.at(0) < posMin.at(0)) {
                right = true;
            } else {
                left = true;
            }
        }
        return vmml::Vector<4,bool>(left,right,top,bottom);
    }
    
    bool isIn(vmml::Vector3f oldCenter, vmml::Vector3f newCenter, vmml::Vector3f boxMin,vmml::Vector3f boxMax, vmml::Vector3f posMin, vmml::Vector3f posMax) {
    
        //return oldCenter.at(0) < newCenter.at(0) + newwidth && oldCenter.at(0) + oldwidth > newCenter.at(0) && oldCenter.at(1) < newCenter.at(1) + newheight && oldheight + oldCenter.at(1) > newCenter.at(1) && oldCenter.at(2) < newCenter.at(2) + newdepth && olddepth + oldCenter.at(2) > newCenter.at(2);
        return (((boxMax.at(0) > posMin.at(0) && boxMin.at(0) < posMin.at(0)) || (boxMax.at(0) > posMax.at(0) && boxMin.at(0) < posMax.at(0))) &&
                ((boxMax.at(1) > posMin.at(1) && boxMin.at(1) < posMin.at(1)) || (boxMax.at(1) > posMax.at(1) && boxMin.at(1) < posMax.at(1))) &&
                ((boxMax.at(2) > posMin.at(2) && boxMin.at(2) < posMin.at(2)) || (boxMax.at(2) > posMax.at(2) && boxMin.at(2) < posMax.at(2))));
    }
    
    bool hasCollision(vmml::Vector<4,bool> collision) {
        return collision.at(0) || collision.at(1) || collision.at(2) || collision.at(3);
    }
    
    vmml::Vector3f getMax(vmml::Vector3f a,vmml::Vector3f b) {
        return vmml::Vector3f(std::max(a.at(0),b.at(0)),std::max(a.at(1),b.at(1)),std::max(a.at(2),b.at(2)));
    }
    
    vmml::Vector3f getMin(vmml::Vector3f a,vmml::Vector3f b) {
        return vmml::Vector3f(std::min(a.at(0),b.at(0)),std::min(a.at(1),b.at(1)),std::min(a.at(2),b.at(2)));
    }
    
    
    
    vmml::Vector3f getCollision(vmml::AABBf oldbox,vmml::AABBf newbox) {
        std::list<Entity>::iterator iterator;
        float xtrans = 0;
        float ytrans = 0;
        for (iterator = entities.begin(); iterator != entities.end(); ++iterator) {
            
            vmml::AABBf boundingBox2 = brenderer.getObjects()->getModel(iterator->getObjName())->getBoundingBoxObjectSpace();
            vmml::AABBf box2(iterator->getPos() * boundingBox2.getMin(),iterator->getPos() * boundingBox2.getMax());
            vmml::Vector3f boxMax = getMax(box2.getMax(),box2.getMin());
            vmml::Vector3f boxMin = getMin(box2.getMax(),box2.getMin());
            vmml::Vector3f posMax = getMax(newbox.getMax(),newbox.getMin());
            vmml::Vector3f posMin = getMin(newbox.getMax(),newbox.getMin());
            // Upon collision, move 0.01 up, until no collision is detected (still need to check with all other objects for collision, therefore no break)
            if (isIn(box2.getCenter(),newbox.getCenter(),boxMin,boxMax,posMin,posMax)){
                if (iterator->getIsCollectible()) {
                    entities.erase(iterator++);
                    continue;
                }
                vmml::Vector<4,bool> collision = checkCollision(oldbox.getCenter(),newbox.getCenter(),boxMin,boxMax,posMin,posMax );
                if (collision.at(3)) {
                    ytrans = box2.getMin().at(1) - newbox.getMax().at(1)-0.001;
                }
                if (collision.at(2)) {
                    ytrans = box2.getMax().at(1) - newbox.getMin().at(1)+0.001;
                }
                if (collision.at(1)) {
                    xtrans = box2.getMax().at(0)-newbox.getMax().at(0)+ 0.001;
                }
                if (collision.at(0)) {
                    xtrans = box2.getMin().at(0)-newbox.getMin().at(0)-0.001;
                }

            }
        }
        //if (std::abs(movedBy.at(0)) > std::abs(movedBy.at(1))) {
        //    return vmml::Vector3f(xtrans,0.0f,0.0f);
        //}
        float resX = 0.0f;
        float resY = 0.0f;
        
        if (newbox.getCenter().at(1) != oldbox.getCenter().at(1)) {
            resY = ytrans;
        } else if (newbox.getCenter().at(0) != oldbox.getCenter().at(0)) {
            resX = xtrans;
        }
        
        return vmml::Vector3f(resX,resY,0.0f);
    }
    
    
    
    
    
    
public:
    
    Controller():timeSinceLast(0),totalSilvercoins(-1),totalGoldcoins(-1),lastUpdate(0),iteration(0){
        
    }
    
    ~Controller(){
        
    }
    
    vmml::Vector3f getPlayerPosition() {
//        std::cout<<"POS"<<vmml::Vector3f(moveableEntities.begin()->getCurrentXPos(), 0.f,0.f)<<std::endl;
        return vmml::Vector3f(-moveableEntities.begin()->getPos().x(), 0.f,0.f);
    }
    
    vmml::Vector3f getPlayerTrans() {
        //        std::cout<<"POS"<<vmml::Vector3f(moveableEntities.begin()->getCurrentXPos(), 0.f,0.f)<<std::endl;
        return vmml::Vector3f(moveableEntities.begin()->getPos().x(), 0.f, 0.f);
    }
    
    void initialize(Renderer _brenderer) {
        brenderer = _brenderer;
        // TODO: Matrix to initialize objects
        LevelBuilder levelBuilder(_brenderer);
        
        
        entities = levelBuilder.getEntities();
        moveableEntities = levelBuilder.getMoveableEntities();
        player = levelBuilder.getPlayer();
        skyplane = levelBuilder.getSkyplane();
        buttons = levelBuilder.getButtons();
        test = levelBuilder.getTest();
    }
    
    void update(double elapsedTime, int direction) {
        int silvercoins = 0;
        int goldcoins = 0;
        timeRunning += elapsedTime;
        timeSinceLast = elapsedTime - timeSinceLast;
        std::list<Entity>::iterator iterator;
        std::list<MoveableEntity>::iterator moveableIterator;
        std::list<Entity>::iterator buttonIterator;

        
        if(GameTime<1.5 && goingUp == true) {
            GameTime += 0.1;
        }
        if(GameTime>=1.5 && goingUp == true){
            goingUp = false;
        }
        if(goingUp==false){
            GameTime -= 0.1;
        }
        if(GameTime<=0.1){
            goingUp=true;
        }
       
        std::cout<<"GameTime: "<<GameTime<<std::endl;
        
        
        for (iterator = entities.begin(); iterator != entities.end(); ++iterator) {
            std::string name = iterator->getObjName();
            if( std::strcmp(name.c_str(),"coin50") ==0) {
             //Implement Coin Rotation here
                iterator->setRotation(1.2*timeRunning);
                goldcoins+=1;

            }
            if( std::strcmp(name.c_str(),"coin20") ==0) {
                iterator->setRotation(1.2*timeRunning);
                silvercoins+=1;

            }
            if( std::strcmp(name.c_str(),"floating_tree1") ==0) {
                
                iterator -> setTranslate(vmml::Vector3f(0., sin(timeRunning),0.));
            }
            drawEntity(*iterator);
        }
        // Move default -0.9 to floor
        float gravity = -0.15f;
        for (moveableIterator = moveableEntities.begin(); moveableIterator != moveableEntities.end(); ++moveableIterator) {
            Movement *movement = moveableIterator->getMovement();
            
            int row = 4 - (iteration-1)/4;
            int column = (iteration - 1)%4;
            
            double yMax = row / 4.0f;
            double yMin = (row - 1) / 4.0f;
            double xMax = (column + 1) / 4.0f;
            double xMin = (column) / 4.0f;

            vmml::Vector<4, bool > move = movement->getMove();
            float x = 0.0f;
            float y = 0.0f;
            
            //work on accellerate / decellerate to not make it schlitschuuuue style
            if (move.at(0)) {
                x -= movement->getStepAccellerate(elapsedTime/3.0,0);
                

                     row = 4 - (iteration-1)/4;
                     column = (iteration - 1)%4;
     
                
                     yMax = row / 4.0f;
                     yMin = (row - 1) / 4.0f;
                     xMax = (column + 1) / 4.0f;
                     xMin = (column) / 4.0f;
                    
                    iteration = iteration + 1;
                    if (iteration > 16) {
                        iteration = 1;
                    }
//                }
                
                
            } else {
                x -= movement->getStepDeccellerate(elapsedTime*8.0,0);
            }
            if (move.at(1)) {
                x += movement->getStepAccellerate(elapsedTime/3.0,1);
                
                 row = 4 - (iteration-1)/4;
                 column = (iteration - 1)%4;
                yMax = row / 4.0f;
                yMin = (row - 1) / 4.0f;
                xMax = (column + 1) / 4.0f;
                xMin = (column) / 4.0f;
                
                iteration = iteration + 1;
                if (iteration > 16) {
                    iteration = 1;
                }
                
                
            } else {
                x += movement->getStepDeccellerate(elapsedTime*8.0,1);
            }
            if (move.at(2) && movement->getDurationFlying() < 1.f) {
                y += 0.1f;
            } else {
                movement->setDurationFlying(1.f);
                y += gravity;
            }
            if (move.at(3)) {
                if (movement->getTargetReleased() == true) {
                    movement->setTargetClicked(movement->getTargetClicked() + 1.0f);
                }
                movement->setTargetReleased(false);
            } else {
                movement->setTargetReleased(true);
            }
            vmml::AABBf boundingBox = brenderer.getObjects()->getModel(moveableIterator->getObjName())->getBoundingBoxObjectSpace();
            vmml::AABBf oldbox(moveableIterator->getPos() * boundingBox.getMin(),moveableIterator->getPos() * boundingBox.getMax());
            moveableIterator->move(vmml::Vector3f(x,y,0.f));
            vmml::AABBf newbox(moveableIterator->getPos() * boundingBox.getMin(),moveableIterator->getPos() * boundingBox.getMax());
            vmml::Vector3f col = getCollision(oldbox,newbox);
            if (col.at(1) != 0) {
                movement->setDurationFlying(0);
            } else {
                movement->setDurationFlying(movement->getDurationFlying() + elapsedTime);
            }
            while(col.at(0) != 0 || col.at(1) != 0) {
                std::cout<<"COLLISION "<<col<<std::endl;
                moveableIterator->move(col);
                newbox.set(moveableIterator->getPos() * boundingBox.getMin(),moveableIterator->getPos() * boundingBox.getMax());
                col = getCollision(oldbox,newbox);
            }
            
            
            std::cout << moveableIterator->getObjName();
            
            //Todo: move skyplane with player
            skyplane.setTranslate(vmml::Vector3f(getPlayerTrans().x()/2.f,0.,0.));
            skyplane.move(skyplane.getTranslate());
            drawEntity(skyplane);

            
            drawEntity(*moveableIterator,
                       brenderer.getObjects()->getCamera("camera")->getViewMatrix(),
                       brenderer.getObjects()->getCamera("camera")->getProjectionMatrix(),0,0,
                       vmml::Vector4f(xMin,xMax,yMin,yMax));
            
        }
        if (totalSilvercoins < 0) {
            totalSilvercoins = silvercoins;
            totalGoldcoins = goldcoins;
        }
        
        drawEntity(buttons.at(0),buttons.at(0).getViewMatrix(),vmml::Matrix4f::IDENTITY,-player.getMovement()->getDurationRight(),0,vmml::Vector4f(0.0f,1.0f,0.0f,1.0f));
        drawEntity(buttons.at(1),buttons.at(1).getViewMatrix(),vmml::Matrix4f::IDENTITY,player.getMovement()->getDurationLeft(),0,vmml::Vector4f(0.0f,1.0f,0.0f,1.0f));
        drawEntity(buttons.at(2),buttons.at(2).getViewMatrix(),vmml::Matrix4f::IDENTITY,0,player.getMovement()->getDurationFlying(),vmml::Vector4f(0.0f,1.0f,0.0f,1.0f));
        drawEntity(buttons.at(3),buttons.at(3).getViewMatrix(),vmml::Matrix4f::IDENTITY,2,2,vmml::Vector4f(0.0f,1.0f,0.0f,1.0f));
//        vmml::Matrix4f viewMatrix2 = brenderer.getObjects()->getCamera("camera")->getViewMatrix();
//        vmml::Matrix4f projectionMatrix2 = brenderer.getObjects()->getCamera("camera")->getProjectionMatrix();
//        
//        
//        if (lastUpdate < timeRunning/100) {
//            lastUpdate = timeRunning / 100;
//            int row = 4 - (iteration-1)/4;
//            int column = (iteration - 1)%4;
//            double yMax = row / 4.0f;
//            double yMin = (row - 1) / 4.0f;
//            double xMax = (column + 1) / 4.0f;
//            double xMin = (column) / 4.0f;
//            
//            
//            //drawEntity(buttons.at(4),buttons.at(4).getViewMatrix(),vmml::Matrix4f::IDENTITY,0,0,vmml::Vector4f(xMin,xMax,yMin,yMax));
//            drawEntity(buttons.at(4),viewMatrix2,projectionMatrix2,0,0,vmml::Vector4f(xMin,xMax,yMin,yMax));
//            iteration = iteration + 1;
//            if (iteration > 16) {
//                iteration = 1;
//            }
//        }
//
//        std::cout<<"PLAYER POS: "<<player.getTranslate()<<std::endl;
//        std::cout<<"SMURF POS:  XXXXX"<<buttons.at(4).getPos()<<std::endl;
        vmml::Matrix4f ma = buttons.at(4).getPos();
        //player.setPos(ma);

        
        
        GLfloat scale = 0.1f;
        vmml::Matrix4f scalingMatrix = vmml::create_scaling(vmml::Vector3f(scale / brenderer.getView()->getAspectRatio(), scale, scale));
        vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.85f, 0.85f, 0.f)) * scalingMatrix;
        vmml::Matrix4f viewMatrix = Camera::lookAt(vmml::Vector3f(0.0f, 0.0f, 0.25f), vmml::Vector3f::ZERO, vmml::Vector3f::UP);
        vmml::Matrix4f projectionMatrix = vmml::Matrix4f::IDENTITY;
        brenderer.getObjects()->getTextSprite("totalGoldCoins")->setText(std::to_string(totalGoldcoins-goldcoins) + "/" + std::to_string(totalGoldcoins));
        
        brenderer.getModelRenderer()->drawModel(brenderer.getObjects()->getTextSprite("totalGoldCoins"), modelMatrix, viewMatrix, projectionMatrix, std::vector<std::string>({}));
        
        vmml::Matrix4f modelMatrix2 = vmml::create_translation(vmml::Vector3f(0.65f, 0.85f, 0.f)) * scalingMatrix;
        brenderer.getObjects()->getTextSprite("totalSilverCoins")->setText(std::to_string(totalSilvercoins-silvercoins) + "/" + std::to_string(totalSilvercoins));
        brenderer.getModelRenderer()->drawModel(brenderer.getObjects()->getTextSprite("totalSilverCoins"), modelMatrix2, viewMatrix, projectionMatrix, std::vector<std::string>({}));
  
    }
    
    
};

#endif