/*
 * OgreMax Sample Viewer and Scene Loader - Ogre3D-based viewer and code for loading and displaying .scene files
 * Copyright 2012 AND Entertainment
 *
 * This code is available under the OgreMax Free License:
 *   -You may use this code for any purpose, commercial or non-commercial.
 *   -If distributing derived works (that use this source code) in binary or source code form,
 *    you must give the following credit in your work's end-user documentation:
 *        "Portions of this work provided by OgreMax (www.ogremax.com)"
 *
 * AND Entertainment assumes no responsibility for any harm caused by using this code.
 *
 * The OgreMax Sample Viewer and Scene Loader were released at www.ogremax.com
 */


//Includes---------------------------------------------------------------------
#include "ProgressCalculator.hpp"

using namespace OgreMax;


//Implementation---------------------------------------------------------------
ProgressCalculator::ProgressCalculator() {
    this->progress = 0;
    this->range = 0;
}

ProgressCalculator::ProgressCalculator(const Ogre::String &name) {
    this->name = name;
    this->progress = 0;
    this->range = 0;
}

ProgressCalculator::~ProgressCalculator() {
    //Delete child calculators
    for(Calculators::iterator calculator = this->childCalculators.begin();
            calculator != this->childCalculators.end();
            ++calculator) {
        delete *calculator;
    }
}

const Ogre::String &ProgressCalculator::GetName() const {
    return this->name;
}

Ogre::Real ProgressCalculator::GetProgress() {
    if(!this->childCalculators.empty()) {
        //Reset progress
        this->progress = 0;

        //Determine the influence of each child calculator
        Ogre::Real influence = (Ogre::Real)1.0 / this->childCalculators.size();

        //Add the progress of all child calculators
        for(Calculators::iterator calculator = this->childCalculators.begin();
                calculator != this->childCalculators.end();
                ++calculator) {
            this->progress += (*calculator)->GetProgress() * influence;
        }
    }

    return this->progress;
}

void ProgressCalculator::SetProgress(Ogre::Real progress) {
    this->progress = progress;
}

void ProgressCalculator::Update(Ogre::Real amount) {
    if(this->range > 0) {
        //Update the progress, scaling it by the inverse range
        this->progress += amount / this->range;

        //Limit the progress to [0, 1]
        this->progress = std::max(this->progress, (Ogre::Real)0);
        this->progress = std::min(this->progress, (Ogre::Real)1);
    }
}

Ogre::Real ProgressCalculator::GetRange() const {
    return this->range;
}

void ProgressCalculator::SetRange(Ogre::Real range) {
    this->progress = 0;
    this->range = range;
}

ProgressCalculator *ProgressCalculator::AddCalculator(const Ogre::String &name) {
    ProgressCalculator *calculator = new ProgressCalculator(name);
    this->childCalculators.push_back(calculator);
    return calculator;
}

