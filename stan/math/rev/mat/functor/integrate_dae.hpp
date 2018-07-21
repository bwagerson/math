#ifndef STAN_MATH_REV_MAT_FUNCTOR_INTEGRATOR_DAE_HPP
#define STAN_MATH_REV_MAT_FUNCTOR_INTEGRATOR_DAE_HPP

#include <stan/math/rev/scal/meta/is_var.hpp>
#include <stan/math/prim/scal/meta/return_type.hpp>
#include <stan/math/rev/mat/functor/idas_forward_system.hpp>
#include <stan/math/rev/mat/functor/idas_integrator.hpp>
#include <stan/math/prim/scal/err/check_less.hpp>
#include <algorithm>
#include <ostream>
#include <vector>

namespace stan {
namespace math {
/**
 * Return the solutions for a semi-explicit DAE system with residual
 * specified by functor F,
 * given the specified consistent initial state yy0 and yp0.
 * Though IDAS support the class of index-1 system where
 * consistent IC can be calculated based on either yy0 or
 * yp0, user must supply good enough initial value for
 * newton-solver to succeed, and this cannot be easily
 * automated when yy0 or yp0 is parameter and changes in
 * MCMC iteration. Therefore here we don't allow ICs to be
 * parameters, and user must make sure the given IC data are
 * consistent.
 *
 * @tparam DAE type of DAE system
 * @tparam Tpar type of parameter theta.
 * @param[in] f functor for the base ordinary differential equation.
 * @param[in] yy0 initial state.
 * @param[in] yp0 initial derivative state.
 * @param[in] t0 initial time.
 * @param[in] ts times of the desired solutions, in strictly
 * increasing order, all greater than the initial time.
 * @param[in] theta parameter
 * @param[in] x_r real data
 * @param[in] x_i int data
 * @param[in] rtol relative tolerance passed to IDAS,
 * recommend <10^3.
 * @param[in] atol absolute tolerance passed to IDAS, problem-dependent.
 * @param[in] max_num_steps maximal number of admissable steps
 * between time-points
 * @param[in] check_ic if check IC consistency, for debug & test
 * @param[in] msgs message
 * @return a vector of states, each state being a vector of the
 * same size as the state variable, corresponding to a time in ts.
 */
template <typename F, typename Tpar>
std::vector<std::vector<Tpar> >
integrate_dae(const F& f,
              const std::vector<double>& yy0,
              const std::vector<double>& yp0,
              double t0, const std::vector<double>& ts,
              const std::vector<Tpar>& theta, const std::vector<double>& x_r,
              const std::vector<int>& x_i, const double rtol, const double atol,
              const int64_t max_num_steps = idas_integrator::IDAS_MAX_STEPS,
              bool check_ic = true,
              std::ostream* msgs = nullptr) {

  const std::vector<int> dummy_eq_id(yy0.size(), 0);
  stan::math::idas_integrator solver(rtol, atol, max_num_steps);
  stan::math::idas_forward_system<F, double, double, Tpar> dae{
      f, dummy_eq_id, yy0, yp0, theta, x_r, x_i, msgs};

  if (check_ic) dae.check_ic_consistency(t0, atol);

  return solver.integrate(dae, t0, ts);
}
}  // namespace math
}  // namespace stan

#endif
