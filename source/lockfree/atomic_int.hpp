//  $Id$
//
//  Copyright (C) 2007 Tim Blechmann & Thomas Grill
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

//  $Revision$
//  $LastChangedRevision$
//  $LastChangedDate$
//  $LastChangedBy$

#ifndef __LOCKFREE_ATOMIC_INT_HPP
#define __LOCKFREE_ATOMIC_INT_HPP

#include "cas.hpp"

namespace lockfree
{

template <typename T>
class atomic_int
{
public:
    explicit atomic_int(T v = 0)
    {
        *this = v;
    }

    operator T(void) const
    {
        memory_barrier();
        return value;
    }

    void operator =(T v)
    {
        value = v;
        memory_barrier();
    }

#if defined(__GNUC__) && ( (__GNUC__ > 4) || ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 1)) )
    T operator +=(T v)
    {
        return __sync_add_and_fetch(&value, v);
    }

    T operator -=(T v)
    {
        return __sync_sub_and_fetch(&value, v);
    }

    /* postfix operator */
    T operator ++(int)
    {
        return __sync_fetch_and_add(&value, 1);
    }

    /* postfix operator */
    T operator --(int)
    {
        return __sync_fetch_and_sub(&value, 1);
    }
#else
    T operator +=(T v)
    {
        for(;;) {
            T oldv = value;
            memory_barrier();
            if(CAS(&value,oldv,oldv+v))
                return oldv+v;
        }
    }

    T operator -=(T v)
    {
        for(;;) {
            T oldv = value;
            memory_barrier();
            if(CAS(&value,oldv,oldv-v))
                return oldv-v;
        }
    }

    /* postfix operator */
    T operator ++(int)
    {
        for(;;) {
            T oldv = value;
            memory_barrier();
            if(CAS(&value,oldv,oldv+1))
                return oldv;
        }
    }

    /* postfix operator */
    T operator --(int)
    {
        for(;;) {
            T oldv = value;
            memory_barrier();
            if(CAS(&value,oldv,oldv-1))
                return oldv;
        }
    }
#endif

    /* prefix operator */
    T operator ++()
    {
        return *this += 1;
    }

    /* prefix operator */
    T operator --()
    {
        return *this -= 1;
    }


private:
    T value;
};

} // namespace lockfree

#endif /* __LOCKFREE_ATOMIC_INT_HPP */
