/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/
#include "UpdateErrorVectorsThread.h"

// MOOSE includes
#include "AuxiliarySystem.h"
#include "FEProblem.h"
#include "Marker.h"
#include "MooseVariable.h"
#include "Problem.h"

// libMesh includes
#include "libmesh/threads.h"
#include "libmesh/error_vector.h"

UpdateErrorVectorsThread::UpdateErrorVectorsThread(
    FEProblemBase & fe_problem,
    const std::map<std::string, std::unique_ptr<ErrorVector>> & indicator_field_to_error_vector)
  : ThreadedElementLoop<ConstElemRange>(fe_problem),
    _indicator_field_to_error_vector(indicator_field_to_error_vector),
    _aux_sys(fe_problem.getAuxiliarySystem()),
    _system_number(_aux_sys.number()),
    _adaptivity(fe_problem.adaptivity()),
    _solution(_aux_sys.solution())
{
  // Build up this map once so we don't have to do these lookups over and over again
  for (const auto & it : _indicator_field_to_error_vector)
  {
    unsigned int var_num = _aux_sys.getVariable(0, it.first).number();
    _indicator_field_number_to_error_vector.emplace(var_num, it.second.get());
  }
}

// Splitting Constructor
UpdateErrorVectorsThread::UpdateErrorVectorsThread(UpdateErrorVectorsThread & x,
                                                   Threads::split split)
  : ThreadedElementLoop<ConstElemRange>(x, split),
    _indicator_field_to_error_vector(x._indicator_field_to_error_vector),
    _aux_sys(x._aux_sys),
    _system_number(x._system_number),
    _adaptivity(x._adaptivity),
    _solution(x._solution),
    _indicator_field_number_to_error_vector(x._indicator_field_number_to_error_vector)
{
}

void
UpdateErrorVectorsThread::onElement(const Elem * elem)
{
  for (const auto & it : _indicator_field_number_to_error_vector)
  {
    unsigned int var_num = it.first;
    ErrorVector & ev = *(it.second);

    dof_id_type dof_number = elem->dof_number(_system_number, var_num, 0);
    Real value = _solution(dof_number);
    ev[elem->id()] = value;
  }
}

void
UpdateErrorVectorsThread::join(const UpdateErrorVectorsThread & /*y*/)
{
}
