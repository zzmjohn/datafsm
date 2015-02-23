#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
int history_lookup(IntegerVector x){
  int result;
  int n = x.length();

  for(int i = 0; i < n; i++){
          if (x[i] > 1 || x[i] < 0){
                  Rprintf("Illegal value for: %d.\n", x[i]);
          }
          if (x[i]==1){
                  result = i; // Not using R indexing convention here
          }
  }

  return result;
}

//' Fitness Function
//'
//' @param action_vec Integer Vector.
//' @param state_mat Integer Matrix.
//' @param covariates Integer Matrix.
//' @param period Integer Vector.
//'
//' @export
// [[Rcpp::export]]

IntegerVector fitnessCPP(IntegerVector action_vec, IntegerMatrix state_mat, IntegerMatrix covariates, IntegerVector period){

  int n = covariates.nrow();

  //IntegerMatrix covariates = x(_, Range(1, x.ncol()-1));

  int state = 1;
  int max_state = state_mat.nrow();
  if (action_vec.length() < max_state) Rprintf("Action vector has length %d, but there are %d states\n", action_vec.length(), max_state);
  IntegerVector decision(n);

  for(int i = 0; i < n; i++){
    int old_state;
    if (period[i] > 1){
      IntegerVector these_covariates = covariates(i,_);

      if (these_covariates[0] > 1 || these_covariates[0] < 0 ||
      these_covariates[1] > 1 || these_covariates[1] < 0 ||
      these_covariates[2] > 1 || these_covariates[2] < 0) {
        Rprintf("Looking up history[%d], period %d: (%d, %d, %d)\n", i,
        these_covariates[0], these_covariates[1], these_covariates[2], these_covariates[3]);
      }

      int history = history_lookup(these_covariates);

      if (history > covariates.ncol() || history < 0) Rprintf("Illegal value for history: %d.\n", history);
      // Not using R indexing convention for history anymore, just for state

      old_state = state;
      state = state_mat((state-1), history); // action_vec and state_mat were made for R where indexing starts at 1,
      if (state > max_state) {
        Rprintf("Illegal state[%d](%d,%d) = %d in period %d. max_state = %d\n", i, old_state, history,
        state, period[i], max_state);
        Rcpp::stop("Illegal state matrix");
      }
      // not 0 like in Cpp

      decision[i] = action_vec[(state-1)];
    } else {
      decision[i] = action_vec[0];
      state = 1;
    }
    // TODO: abort the cpp func if the user aborts in R: Rcpp::checkUserInterrupt();
  } // or if saving fitness results, could do: fitness_count[i] = decision == outcome[i] ? 1 : 0;

  return decision;
}
