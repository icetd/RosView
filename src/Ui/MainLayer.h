﻿#ifndef MAIN_LAYER_H
#define MAIN_LAYER_H

#include "Layer.h"

class MainLayer : public Layer
{
protected:
	virtual void OnUpdate(float ts) override;
};

#endif