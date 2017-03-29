/****************************************************************************
**
** This file is the interpolation header.
**
** Copyright (c) 2017, National Chiao Tung University
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the National Chiao Tung University nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL NATIONAL CHIAO TUNG UNIVERSITY BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/
#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include <vector>
#include <string>

#define OUT_OF_RANGE_EXCEPTION std::out_of_range("Out of Range error: interpolate::_M_range_check")

// Forward declaration
class Method1d;
class Method2d;

namespace interpolate
{

enum Type
{
    linear,
    //cubic
};

class interp1d
{
public:
    interp1d(std::vector<double>& x, std::vector<double>& y, Type kind=linear, bool copy=true);
    ~interp1d();

    double operator() (double x);
    double at(double x);

private:
    Method1d* method;
};


class interp2d
{
public:
    interp2d(std::vector<double>& x, std::vector<double>& y, std::vector<std::vector<double> >& z, Type kind=linear, bool copy=true);
    ~interp2d();

    double operator() (double x, double y);
    double at(double x, double y);

private:
    Method2d* method;
};

};

#endif // INTERPOLATE_H
