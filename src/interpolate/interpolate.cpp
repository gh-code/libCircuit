/****************************************************************************
**
** This file is the source of interpolation.
**
** Author: Gary Huang <gh.nctu+code AT gmail DOT com>
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
#include "interpolate.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>    // std::lower_bound
#include <stdexcept>    // std::out_of_range
#include <cfloat>       // DBL_MIN

using namespace interpolate;

/**************************************************************
 *
 * Private class declerations
 *
 **************************************************************/

class Method1d
{
public:
    Method1d(std::vector<double>& x, std::vector<double>& y, bool copy=true)
        : vx(&x), vy(&y), copied(copy)
    {
        if (copy)
        {
            vx = new std::vector<double>(x.begin(), x.end());
            vy = new std::vector<double>(y.begin(), y.end());
        }
    }
    virtual ~Method1d()
    {
        if (copied)
        {
            delete vx;
            delete vy;
        }
    }

    static Method1d* create(std::vector<double>& x, std::vector<double>& y, Type kind, bool copy=true);

    virtual double calculateAt(double x) = 0;
    virtual double calculate(double x) = 0;

protected:
    std::vector<double>* vx;
    std::vector<double>* vy;
    bool copied;
};

class Method2d : public Method1d
{
public:
    Method2d(std::vector<double>& x, std::vector<double>& y, std::vector<std::vector<double> >& z, bool copy=true)
        : Method1d(x, y, copy), vz(&z)
    {
        if (copy)
            vz = new std::vector<std::vector<double> >(z.begin(), z.end());
    }
    virtual ~Method2d()
    {
        if (copied)
            delete vz;
    }

    static Method2d* create(std::vector<double>& x, std::vector<double>& y, std::vector<std::vector<double> >& z, Type kind, bool copy=true);

    virtual double calculateAt(double x, double y) = 0;
    virtual double calculate(double x, double y) = 0;

private:
    // delete these methods
    double calculateAt(double x) { return x; }
    double calculate(double x) { return x; }

protected:
    std::vector<std::vector<double> >* vz;
};

class Linear1d : public Method1d
{
public:
    Linear1d(std::vector<double>& x, std::vector<double>& y, bool copy=true) : Method1d(x, y, copy) {}

    double calculateAt(double x)
    {
        std::vector<double>::iterator it = std::lower_bound((*vx).begin(), (*vx).end(), x);
        if (it == (*vx).end() || (it == (*vx).begin() && (*it) != x))
            throw OUT_OF_RANGE_EXCEPTION;
        size_t pos = it - (*vx).begin();
        return formula(x, pos);
    }

    double calculate(double x)
    {
        std::vector<double>::iterator it = std::lower_bound((*vx).begin(), (*vx).end(), x);
        size_t pos = it - (*vx).begin();
        return formula(x, pos);
    }

    double formula(double x, size_t pos)
    {
        return (x - (*vx)[pos-1]) * ((*vy)[pos] - (*vy)[pos-1]) / ((*vx)[pos] - (*vx)[pos-1]) + (*vy)[pos-1];
    }
};

class Linear2d : public Method2d
{
public:
    Linear2d(std::vector<double>& x, std::vector<double>& y, std::vector<std::vector<double> >& z, bool copy=true) : Method2d(x, y, z, copy) {}

    double calculateAt(double x, double y)
    {
        std::vector<double>::iterator it = std::lower_bound((*vx).begin(), (*vx).end(), x);
        if (it == (*vx).end() || (it == (*vx).begin() && (*it) != x))
            throw OUT_OF_RANGE_EXCEPTION;
        std::vector<double>::iterator jt = std::lower_bound((*vy).begin(), (*vy).end(), y);
        if (jt == (*vy).end() || (jt == (*vy).begin() && (*jt) != y))
            throw OUT_OF_RANGE_EXCEPTION;
        size_t pos_x = it - (*vx).begin();
        size_t pos_y = jt - (*vy).begin();
        return formula(x, y, pos_x, pos_y);
    }

    double calculate(double x, double y)
    {
        std::vector<double>::iterator it = std::lower_bound((*vx).begin(), (*vx).end(), x);
        std::vector<double>::iterator jt = std::lower_bound((*vy).begin(), (*vy).end(), y);
        if (it == (*vx).end()) it -= 1;
        if (jt == (*vy).end()) jt -= 1;
        size_t pos_x = it - (*vx).begin();
        size_t pos_y = jt - (*vy).begin();
        if (pos_x == 0) { pos_x = 1; }
        if (pos_y == 0) { pos_y = 1; }
        return formula2(x, y, pos_x, pos_y);
    }

    double formula2(double x, double y, size_t pos_x1, size_t pos_y1)
    {
        size_t pos_x0 = pos_x1 - 1;
        size_t pos_y0 = pos_y1 - 1;
        double x0 = (*vx)[pos_x0];
        double y0 = (*vy)[pos_y0];
        double x1 = (*vx)[pos_x1];
        double y1 = (*vy)[pos_y1];
        double wx = (x - x0) / (x1 - x0);
        double wy = (y - y0) / (y1 - y0);
        // std::cout << "           "<< "(X) " << x0 << "(X) " << x1 << std::endl;
        // std::cout << "(Y) " << y0 << "(Z) " << (*vz)[pos_y0][pos_x0] << "(Z) " << (*vz)[pos_y0][pos_x1] << std::endl;
        // std::cout << "(Y) " << y1 << "(Z) " << (*vz)[pos_y1][pos_x0] << "(Z) " << (*vz)[pos_y1][pos_x1] << std::endl;
        double A = (1 - wx) * (1 - wy);
        double B = wx * (1 - wy);
        double C = (1 - wx) * wy;
        double D = wx * wy;
        // std::cout << "A = " << A << std::endl;
        // std::cout << "B = " << B << std::endl;
        // std::cout << "C = " << C << std::endl;
        // std::cout << "D = " << D << std::endl;
        return A * (*vz)[pos_y0][pos_x0]
             + B * (*vz)[pos_y0][pos_x1]
             + C * (*vz)[pos_y1][pos_x0]
             + D * (*vz)[pos_y1][pos_x1];
    }

    double formula(double x, double y, size_t pos_x1, size_t pos_y1)
    {
        size_t pos_x0 = pos_x1 - 1;
        size_t pos_y0 = pos_y1 - 1;
        double x0 = (*vx)[pos_x0];
        double y0 = (*vy)[pos_y0];
        double x1 = (*vx)[pos_x1];
        double y1 = (*vy)[pos_y1];
        if (pos_x1 == 0 && pos_y1 == 0)
            return (*vz)[0][0];
        if (pos_x1 == 0)
        {
            double D = (y1 - y0);
            double Q = (*vz)[pos_y0][0] * (y1 - y)
                     + (*vz)[pos_y1][0] * (y - y0);
            return (Q / D);
        }
        if (pos_y1 == 0)
        {
            double D = (x1 - x0);
            double Q = (*vz)[0][pos_x0] * (x1 - x)
                     + (*vz)[0][pos_x1] * (x - x0);
            return (Q / D);
        }
        double D = (x1 - x0) * (y1 - y0);
        double Q = (*vz)[pos_y0][pos_x0] * ((x1 - x) * (y1 - y))
                 + (*vz)[pos_y0][pos_x1] * ((x - x0) * (y1 - y))
                 + (*vz)[pos_y1][pos_x0] * ((x1 - x) * (y - y0))
                 + (*vz)[pos_y1][pos_x1] * ((x - x0) * (y - y0));
        return (Q / D);
    }
};

Method1d* Method1d::create(std::vector<double>& x, std::vector<double>& y, Type kind, bool copy)
{
    switch (kind)
    {
        case linear:
            return new Linear1d(x, y, copy);
        default:
            std::cerr << "Unknown kind of interpolation" << std::endl;
            return 0;
    }
}

Method2d* Method2d::create(std::vector<double>& x, std::vector<double>& y, std::vector<std::vector<double> >& z, Type kind, bool copy)
{
    switch (kind)
    {
        case linear:
            return new Linear2d(x, y, z, copy);
        default:
            std::cerr << "Unknown kind of interpolation" << std::endl;
            return 0;
    }
}


/**************************************************************
 *
 * interp1d
 *
 **************************************************************/

static bool monoIncr(const std::vector<double>& v)
{
    std::vector<double>::const_iterator it = v.begin();
    double last = DBL_MIN;
    while (it != v.end())
    {
        if ((*it) <= last)
            break;
        last = (*it);
        ++it;
    }
    return (it == v.end());
}

interp1d::interp1d(std::vector<double>& x, std::vector<double>& y, Type kind, bool copy)
{
    if (x.size() != y.size())
    {
        std::cerr << "Error: y along x axis must have the same size" << std::endl;
        return ;
    }
    if (!monoIncr(x))
    {
        std::cerr << "Error: x is not increasing monotonically" << std::endl;
        return ;
    }
    method = Method1d::create(x, y, kind, copy);
}

interp1d::~interp1d()
{
    if (method != 0)
        delete method;
}

double interp1d::at(double x)
{
    return method->calculateAt(x);
}

double interp1d::operator() (double x)
{
    return method->calculate(x);
}


/**************************************************************
 *
 * interp2d
 *
 **************************************************************/

interp2d::interp2d(std::vector<double>& x, std::vector<double>& y, std::vector<std::vector<double> >& z, Type kind, bool copy)
{
    if (z.size() != y.size())
    {
        std::cerr << "Error: z along y axis must have the same size" << std::endl;
        return ;
    }

    // Performance issue while y >> x
    for (size_t i = 0; i < y.size(); i++)
    {
        if (z[i].size() != x.size())
        {
            std::cerr << "Error: z along x axis must have the same size" << std::endl;
            return ;
        }
    }

    if (!monoIncr(x) || !monoIncr(y))
    {
        std::cerr << "Error: x or y is not increasing monotonically" << std::endl;
        return ;
    }
    method = Method2d::create(x, y, z, kind, copy);
}

interp2d::~interp2d()
{
    if (method != 0)
        delete method;
}

double interp2d::at(double x, double y)
{
    return method->calculateAt(x, y);
}

double interp2d::operator() (double x, double y)
{
    return method->calculate(x, y);
}
