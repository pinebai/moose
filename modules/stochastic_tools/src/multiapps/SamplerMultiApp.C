/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

// MOOSE includes
#include "SamplerMultiApp.h"
#include "Sampler.h"

template <>
InputParameters
validParams<SamplerMultiApp>()
{
  InputParameters params = validParams<TransientMultiApp>();
  params.addClassDescription("Creates a sub-application for each row of each Sampler matrix.");
  params.addParam<SamplerName>("sampler", "The Sampler object to utilize for creating MultiApps.");
  params.suppressParameter<std::vector<Point>>("positions");
  params.suppressParameter<bool>("output_in_position");
  params.suppressParameter<std::vector<FileName>>("positions_file");
  params.suppressParameter<Real>("move_time");
  params.suppressParameter<std::vector<Point>>("move_positions");
  params.suppressParameter<std::vector<unsigned int>>("move_apps");
  params.set<bool>("use_positions") = false;
  return params;
}

SamplerMultiApp::SamplerMultiApp(const InputParameters & parameters)
  : TransientMultiApp(parameters), SamplerInterface(this), _sampler(getSampler("sampler"))
{
  std::vector<DenseMatrix<Real>> out = _sampler.getSamples();
  unsigned int num = 0;
  for (const DenseMatrix<Real> & mat : out)
    num += mat.m();
  init(num);
}
