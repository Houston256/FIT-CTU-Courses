#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct Instance {
  int64_t sumVarWeights() const {
    return std::reduce(varWeights.begin(), varWeights.end(), int64_t{0});
  }

  static Instance parse(std::ifstream &in) {
    Instance inst;
    std::string line;

    while (std::getline(in, line)) {
      if (line.empty() || line[0] == 'c' || line[0] == 'p')
        continue;

      std::istringstream iss(line);
      inst.varWeights.push_back(0);
      if (line[0] == 'w') {
        std::string tmp;
        iss >> tmp;
        int64_t x;
        while (iss >> x && x != 0) {
          inst.varWeights.push_back(x);
        }
        inst.varToClauses.resize(inst.varWeights.size());
        break;
      }
    }
    std::getline(in, line);

    int64_t clause = 1;
    int64_t lit;
    while (in >> lit) {
      if (lit == 0) {
        ++clause;
        continue;
      }
      int64_t var = std::abs(lit);
      int64_t sgn = lit / var;
      inst.varToClauses[var].push_back(sgn * clause);
    }
    inst.numClauses = clause - 1;
    return inst;
  }

  int64_t numVars() const {
    return static_cast<int64_t>(varWeights.size()) - 1;
  }

  std::vector<std::vector<int64_t>> varToClauses;
  std::vector<int64_t> varWeights;
  int64_t numClauses = 0;
};

struct State {
  State(const Instance &inst)
      : varAssignment(inst.numVars() + 1), clausePosWeight(inst.numClauses + 1),
        inst(inst) {}

  void reset() {
    std::fill(varAssignment.begin(), varAssignment.end(), 1);
    std::fill(clausePosWeight.begin(), clausePosWeight.end(), 0);
    numUnsatClauses = 0;
    objective = 0;

    for (int64_t var = 1; var <= inst.numVars(); ++var) {
      objective += inst.varWeights[var];
      for (int64_t signedClause : inst.varToClauses[var]) {
        if (signedClause > 0) {
          clausePosWeight[signedClause] += inst.varWeights[var];
        }
      }
    }
    for (int64_t c = 1; c < clausePosWeight.size(); ++c) {
      numUnsatClauses += clausePosWeight[c] <= 0;
    }

    std::bernoulli_distribution coin(0.5);
    for (int64_t i = 1; i < varAssignment.size(); ++i) {
      if (coin(generator)) {
        flip(i);
      }
    }
  }

  void flip(int64_t var) {
    for (int64_t signedClause : inst.varToClauses[var]) {
      int64_t clause = std::abs(signedClause);
      auto wasSat = clausePosWeight[clause] > 0;
      clausePosWeight[clause] -=
          signedClause / clause * varAssignment[var] * inst.varWeights[var];
      auto isSat = clausePosWeight[clause] > 0;
      numUnsatClauses += wasSat - isSat;
    }
    objective -= varAssignment[var] * inst.varWeights[var];
    varAssignment[var] *= -1;
  }

  int64_t numUnsatClauses = 0;
  int64_t objective = 0;
  std::vector<int64_t> varAssignment;
  std::vector<int64_t> clausePosWeight;
  const Instance &inst;
  std::mt19937_64 generator{std::random_device{}()};
};

struct Annealer {
  double Tmax = 1.0;
  double Tmin = 1e-2;
  double TminRatio = 1e-4;
  double alpha = 0.99;
  int64_t maxAttemptedFlips = 1e9;

  int64_t bestObjective = std::numeric_limits<int64_t>::min();
  std::vector<int64_t> bestAssignment;
  int64_t totalAttempted = 0;
  double lastAccRate = 1.0;
  int64_t M = 1;
  bool recordSteps = false;

  std::vector<std::pair<int64_t, int64_t>> steps;

  std::uniform_real_distribution<double> uniform{std::nextafter(0.0, 1.0), 1.0};
  std::uniform_int_distribution<int64_t> pickVar{1, 1};

  double computeInitialTemp(State &state, double P0 = 0.99,
                            int64_t numSamples = 1000, double epsilon = 0.01,
                            int64_t maxIter = 100) {
    std::vector<double> worseningDeltas;
    worseningDeltas.reserve(numSamples);

    for (int64_t i = 0; i < numSamples; ++i) {
      int64_t var = pickVar(state.generator);
      int64_t oldUnsat = state.numUnsatClauses;
      int64_t oldObj = state.objective;
      state.flip(var);

      double dEnergy = (state.numUnsatClauses - oldUnsat) -
                       static_cast<double>(state.objective - oldObj) / M;

      if (dEnergy > 0) {
        worseningDeltas.push_back(dEnergy);
      }
      state.flip(var);
    }

    double T = 1.0;
    double p = 1.0;

    if (worseningDeltas.empty()) {
      return T;
    }

    for (int64_t iter = 0; iter < maxIter; ++iter) {
      double sumProb = 0.0;
      for (double delta : worseningDeltas) {
        sumProb += std::exp(-delta / T);
      }
      double PT = sumProb / worseningDeltas.size();

      if (std::abs(PT - P0) < epsilon) {
        return T;
      }
      T *= std::pow(std::log(PT) / std::log(P0), 1.0 / p);
    }
    return T;
  }

  void reset(State &state) {
    bestObjective = std::numeric_limits<int64_t>::min();
    bestAssignment.clear();
    steps.clear();
    totalAttempted = 0;
    lastAccRate = 1.0;
    M = state.inst.sumVarWeights() + 1;
    pickVar = std::uniform_int_distribution<int64_t>(1, state.inst.numVars());
    state.reset();
    Tmax = computeInitialTemp(state);
    Tmin = TminRatio * Tmax;
    int64_t K = 900;
    alpha = std::pow(Tmin / Tmax, 1.0 / K);
    if (recordSteps) {
      steps.emplace_back(state.numUnsatClauses, state.objective);
    }
    if (state.numUnsatClauses == 0) {
      bestObjective = state.objective;
      bestAssignment = state.varAssignment;
    }
  }

  bool frozen(double T) const {
    return T < Tmin || totalAttempted >= maxAttemptedFlips;
  }

  double cool(double T) { return T * alpha; }

  bool acceptMove(double dE, double T, std::mt19937_64 &gen) {
    if (dE <= 0.0)
      return true;
    return std::log(uniform(gen)) * T < -dE;
  }

  void equilibrium(State &state, double T) {
    int64_t N = state.inst.numVars();
    int64_t c = 12;
    double a = std::max(lastAccRate, 0.02);
    int64_t Leq = std::llround((c * N) / a);
    Leq = std::clamp(Leq, N, 50 * N);
    int64_t targetAccepted = std::min<int64_t>(Leq, c * N);

    int64_t accepted = 0;
    int64_t attempted = 0;

    for (; attempted < Leq && accepted < targetAccepted &&
           totalAttempted < maxAttemptedFlips;
         ++attempted, ++totalAttempted) {

      int64_t var = pickVar(state.generator);
      int64_t oldUnsat = state.numUnsatClauses;
      int64_t oldObj = state.objective;
      state.flip(var);

      double dE = (state.numUnsatClauses - oldUnsat) -
                  static_cast<double>(state.objective - oldObj) / M;

      if (acceptMove(dE, T, state.generator)) {
        ++accepted;
        if (recordSteps)
          steps.emplace_back(state.numUnsatClauses, state.objective);
        if (state.numUnsatClauses == 0 && state.objective > bestObjective) {
          bestObjective = state.objective;
          bestAssignment = state.varAssignment;
        }
      } else {
        state.flip(var);
      }
    }

    lastAccRate = attempted ? static_cast<double>(accepted) / attempted : 0.0;
  }

  void solve(State &state) {
    reset(state);
    double T = Tmax;

    while (!frozen(T)) {
      equilibrium(state, T);
      if (!bestAssignment.empty() && lastAccRate == 0.0)
        break;
      T = cool(T);
    }
  }

  void writeResult(std::ostream &out) const {
    out << bestObjective;
    for (int64_t var = 1; var < bestAssignment.size(); ++var) {
      out << " " << bestAssignment[var] * var;
    }
    out << " 0\n";
  }

  void writeSteps(const fs::path &path) const {
    fs::create_directories(path.parent_path());
    std::ofstream out(path);
    out << "unsat,objective\n";
    for (const auto &[unsat, obj] : steps) {
      out << unsat << "," << obj << "\n";
    }
  }
};

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0]
              << " <input_file> <num_runs> [write_steps]" << std::endl;
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << argv[1] << std::endl;
    return 1;
  }

  int64_t numRuns = std::stoi(argv[2]);
  bool writeSteps = argc > 3 && std::string(argv[3]) == "1";

  fs::path inputPath = fs::path(argv[1]).lexically_relative("data");
  fs::path outputDir = "results" / inputPath.parent_path();
  std::string stem = inputPath.stem().string();
  fs::create_directories(outputDir);

  auto inst = Instance::parse(file);
  State state(inst);
  Annealer annealer;
  annealer.recordSteps = writeSteps;

  std::ostringstream results;
  for (int64_t i = 1; i <= numRuns; ++i) {
    annealer.solve(state);
    annealer.writeResult(results);
    if (writeSteps) {
      auto step_file =
          outputDir / (stem + "-run_" + std::to_string(i) + ".csv");
      annealer.writeSteps(step_file);
    }
  }

  std::ofstream outFile(outputDir / inputPath.filename());
  outFile << results.str();
}