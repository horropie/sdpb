#include "../../SDP_Solver.hxx"
#include "../../../Timers.hxx"

// Centering parameter \beta_c for the corrector step
Real corrector_centering_parameter(const SDP_Solver_Parameters &parameters,
                                   const Block_Diagonal_Matrix &X,
                                   const Block_Diagonal_Matrix &dX,
                                   const Block_Diagonal_Matrix &Y,
                                   const Block_Diagonal_Matrix &dY,
                                   const Real &mu,
                                   const bool is_primal_dual_feasible)
{
  timers["run.correctorStep.frobeniusProduct"].resume();
  Real r = frobenius_product_of_sums(X, dX, Y, dY) / (mu * X.dim);
  timers["run.correctorStep.frobeniusProduct"].stop();
  Real beta = r < 1 ? r * r : r;

  if(is_primal_dual_feasible)
    {
      return min(max(parameters.feasible_centering_parameter, beta), Real(1));
    }
  else
    {
      return max(parameters.infeasible_centering_parameter, beta);
    }
}

// Centering parameter \beta_c for the corrector step
El::BigFloat corrector_centering_parameter(
  const SDP_Solver_Parameters &parameters, const Block_Diagonal_Matrix &X,
  const Block_Diagonal_Matrix &dX, const Block_Diagonal_Matrix &Y,
  const Block_Diagonal_Matrix &dY, const El::BigFloat &mu,
  const bool is_primal_dual_feasible)
{
  timers["run.correctorStep.frobeniusProduct"].resume();
  El::BigFloat r = frobenius_product_of_sums_elemental(X, dX, Y, dY)
                   / (mu * X.dim);
  timers["run.correctorStep.frobeniusProduct"].stop();
  El::BigFloat beta = r < 1 ? r * r : r;

  if(is_primal_dual_feasible)
    {
      return Min(Max(parameters.feasible_centering_parameter_elemental, beta),
                 El::BigFloat(1));
    }
  else
    {
      return Max(parameters.infeasible_centering_parameter_elemental, beta);
    }
}
