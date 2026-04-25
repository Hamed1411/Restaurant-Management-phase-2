#pragma once

class Restaurant; // Forward declaration of Restaurant class

class Action {
protected:
    int ActionTime;    // The timestep this action occurs (TS)
    Restaurant* pRest; // Pointer to the restaurant

public:
    Action(int time, Restaurant* r) : ActionTime(time), pRest(r) {}

    virtual void ACT() = 0; // Pure virtual: must be implemented by children

    int getActionTime() const { return ActionTime; }

    virtual ~Action() {}



};
