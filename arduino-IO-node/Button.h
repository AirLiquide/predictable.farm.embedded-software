/*
  Copyright (C) Air Liquide S.A,  2017
  Author: Sébastien Lalaurette, La Factory, Creative Foundry
  This file is part of Predictable Farm project.

  The MIT License (MIT)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
   
  See the LICENSE.txt file in this repository for more information.
*/
#ifndef _BUTTON_H_
#define _BUTTON_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/*=========================================================================
    VALUE OF THE BUTTON
    -----------------------------------------------------------------------*/
#define BTN_UP            (840)
#define BTN_DOWN          (900)
#define BTN_BACK          (700)
#define BTN_ENTER         (980)

#define UP            (1)
#define DOWN          (2)
#define BACK          (3)
#define ENTER         (4)

#define BTN1           (1)
#define BTN2           (2)
#define BTN3           (3)
#define BTN4           (4)

class Button{
 public:
  Button();
  unsigned char get(void);
  unsigned char getCurrent(void);
  unsigned short getRawCurrent(void) ;
  unsigned char getPrev(unsigned char current);

 private:
   unsigned short prev;
};


#endif /*_BUTTON_H_*/
