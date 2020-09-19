#ifndef __RENDER_CONTEXT_H
#define __RENDER_CONTEXT_H

#include "SwapChain.h"

class RenderContext
{
private:
  SwapChain *_swapChain = nullptr;
public:
  RenderContext();
  ~RenderContext();
};


#endif
