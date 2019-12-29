#ifndef SRC_ADVENTURE_H_
#define SRC_ADVENTURE_H_

#include <algorithm>
#include <vector>

#include "../third_party/threadpool/threadpool.h"

#include "./types.h"
#include "./utils.h"

class Adventure {
 public:
  virtual uint64_t packEggs(std::vector<Egg> eggs, BottomlessBag& bag) = 0;

  virtual void arrangeSand(std::vector<GrainOfSand>& grains) = 0;

  virtual Crystal selectBestCrystal(std::vector<Crystal>& crystals) = 0;
};

class LonesomeAdventure : public Adventure {
 public:
  LonesomeAdventure() {}

  virtual uint64_t packEggs(std::vector<Egg> eggs, BottomlessBag& bag) {
    // TODO(jackowski) Implement this methos
    return 0;
  }

  virtual void arrangeSand(std::vector<GrainOfSand>& grains) {
    // TODO(jackowski) Implement this methos
  }

  virtual Crystal selectBestCrystal(std::vector<Crystal>& crystals) {
    // TODO(jackowski) Implement this methos
    return Crystal(0);
  }
};

class TeamAdventure : public Adventure {
 public:
  explicit TeamAdventure(uint64_t numberOfShamansArg)
      : numberOfShamans(numberOfShamansArg),
        councilOfShamans(numberOfShamansArg) {}

  uint64_t packEggs(std::vector<Egg> eggs, BottomlessBag& bag) {
    // TODO(jackowski) Implement this methos
    return 0;
  }

  virtual void arrangeSand(std::vector<GrainOfSand>& grains) {
    // TODO(jackowski) Implement this methos
  }

  virtual Crystal selectBestCrystal(std::vector<Crystal>& crystals) {
    // TODO(jackowski) Implement this methos
    return Crystal(0);
  }

 private:
  uint64_t numberOfShamans;
  ThreadPool councilOfShamans;
};

#endif  // SRC_ADVENTURE_H_
