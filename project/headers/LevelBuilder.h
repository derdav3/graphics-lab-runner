//
//  LevelBuilder.h
//  bRenderer_ios
//
//  Created by dev_lab on 10/05/16.
//
//

#ifndef bRenderer_ios_LevelBuilder_h
#define bRenderer_ios_LevelBuilder_h

#include "bRenderer.h"
#include "EntityBuilder.h"
#include "PlayerMovement.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iterator>

class LevelBuilder {
private:
    Renderer bRenderer;
    Entity player;
    Entity skyplane;
    
    std::list<Entity> buttons;
    
    std::list<Entity> entities;
    std::list<MoveableEntity> moveableEntities;
    
    GLfloat imageScale;
    
    std::string level;
    // Center of first screen
    int rowCenter = 3;
    int colCenter = 9;
    
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }
    
    std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, elems);
        return elems;
    }
    
    void addElement(std::string index, int row, int column) {
        //1 = Grassblock, 2 = Mario
        EntityBuilder builder;
        if (std::strcmp(index.c_str(),"1") ==0) {
            builder.setObjectName("block").setShaderName("guy");
        } else if (std::strcmp(index.c_str(),"2") ==0) {
            //PlayerMovement movement;
            builder.setObjectName("minecraftcharacter").setShaderName("guy").setIsMoving(true).setFacing(2).setMovement(new PlayerMovement(bRenderer,buttons));
        } else if (std::strcmp(index.c_str(),"3") ==0) {
            builder.setObjectName("minecraftcharacter").setShaderName("guy").setIsMoving(true).setFacing(1);
        }
        //There is an element
        if (std::strcmp(index.c_str(),"0") !=0) {
            std::cout<<builder.getObjectName()<<std::endl;
            vmml::AABBf boundingBox = bRenderer.getObjects()->getModel(builder.getObjectName())->getBoundingBoxObjectSpace();
            builder.setScale(vmml::Vector3f(1/boundingBox.getDimension().find_max())).setTranslation(vmml::Vector3f(column-colCenter,row+rowCenter,0));
            if (builder.isMoving()) {
                MoveableEntity entity = builder.createMoveableEntity();
                moveableEntities.push_back(entity);
                if (std::strcmp(index.c_str(),"2") ==0) {
                    player = entity;
                }
            } else {
                entities.push_back(builder.createEntity());
            }
        }
    }
    
    void addButton(std::string name, std::string image, vmml::Vector3f translation) {
        bRenderer.getObjects()->createSprite(name, image);
        EntityBuilder builder;
        vmml::Matrix4f _viewMatrixHUD = Camera::lookAt(vmml::Vector3f(0.0f, 0.0f, 0.25f), vmml::Vector3f::ZERO, vmml::Vector3f::UP);
        builder.setScale(vmml::Vector3f(imageScale / bRenderer.getView()->getAspectRatio(), imageScale, imageScale)).setTranslation(translation).setViewMatrix(_viewMatrixHUD).setObjectName(name).setImage(image);
        buttons.push_back(builder.createEntity());
    }
    
public:
    
    LevelBuilder(Renderer _brenderer):bRenderer(_brenderer){
        
        imageScale = 0.1f;
        
        addButton("bLeft","arrowL.png",vmml::Vector3f(0.55f, -0.9f, -0.00f));
        addButton("bRight","arrowR.png",vmml::Vector3f(0.85f, -0.9f, -0.00f));
        addButton("bUp","arrowU.png",vmml::Vector3f(-0.55f, -0.9f, -0.00f));
        addButton("bTarget","target.png",vmml::Vector3f(-0.85f, -0.9f, -0.00f));
        
        
        std::string line;
        std::ifstream myfile (bRenderer::getFilePath("level.txt"));
        if (myfile.is_open())
        {
            int row = 0;
            while ( std::getline (myfile,line) )
            {
                int column = 0;
                std::vector<std::string> elements = split(line,' ');
                for(std::vector<std::string>::iterator it2 = elements.begin(); it2 != elements.end(); ++it2) {
                    addElement(*it2,row,column);
                    column++;
                }
                row--;
            }
            myfile.close();
        }
        
        //_brenderer.getObjects()->createSprite("bLeft", "arrowL.png");
        //_brenderer.getObjects()->createSprite("bRight", "arrowR.png");
        //_brenderer.getObjects()->createSprite("bUp", "arrowU.png");
        
        
        EntityBuilder builder;
        builder.setObjectName("backgroundPlane").setShaderName("background").setScale(vmml::Vector3f(3.0f)).setTranslation(vmml::Vector3f(0.,0.,0.)).setFacing(1);
        skyplane = builder.createEntity();
        
    }
    
    ~LevelBuilder(){
        
    }
    
    std::list<Entity> getEntities() {
        return entities;
    }
    
    std::list<MoveableEntity> getMoveableEntities() {
        return moveableEntities;
    }
    
    std::list<Entity> getButtons() {
        return buttons;
    }
    
    Entity getPlayer() {
        return player;
    }
    
    Entity getSkyplane() {
        return skyplane;
    }
    
    
    
};

#endif