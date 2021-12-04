#ifndef CLOCKCANVAS_H
#define CLOCKCANVAS_H

#include "Canvas.h"

class ClockCanvas : public Canvas {

    CanvasType type() override { return CanvasType::Clock; }

    void update() override;
};

#endif