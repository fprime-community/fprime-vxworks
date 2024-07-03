// File TaskId.cpp
// Author: Ben Soudry (benjamin.s.soudry@jpl.nasa.gov)
//         Nathan Serafin (nathan.serafin@jpl.nasa.gov)
// Date: 29 June, 2018
//
// VxWorks implementation of the TaskId class.

#include <taskLib.h>

#include <Os/TaskId.hpp>

namespace Os
{
    TaskId::TaskId(void)
    {
        id = taskIdSelf();
    }
    TaskId::~TaskId(void)
    {
    }

    bool TaskId::operator==(const TaskId& T) const
    {
        return (T.id == id);
    }

    bool TaskId::operator!=(const TaskId& T) const
    {
        return (T.id != id);
    }

    TaskIdRepr TaskId::getRepr(void) const
    {
        return this->id;
    }
}
