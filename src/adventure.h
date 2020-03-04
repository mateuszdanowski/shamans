#ifndef SRC_ADVENTURE_H_
#define SRC_ADVENTURE_H_

#include <algorithm>
#include <vector>

#include "../third_party/threadpool/threadpool.h"
#include "./types.h"
#include "./utils.h"

std::atomic<int> counter;

class Adventure {
 public:
  virtual ~Adventure() = default;

  virtual uint64_t packEggs(std::vector<Egg> eggs, BottomlessBag& bag) = 0;

  virtual void arrangeSand(std::vector<GrainOfSand>& grains) = 0;

  virtual Crystal selectBestCrystal(std::vector<Crystal>& crystals) = 0;
};

int partition(std::vector<GrainOfSand>& grains, int left, int right) {
  srand(406125);
  int pivot_idx = rand() % (right - left + 1) + left;
  std::swap(grains[pivot_idx], grains[right]);
  int i = left;
  for (int j = left; j <= right; j++) {
    if (grains[j] < grains[right]) {
      std::swap(grains[i], grains[j]);
      i++;
    }
  }
  std::swap(grains[i], grains[right]);
  return i;
}

void quickSort(std::vector<GrainOfSand>& grains, int start, int end) {
  int idx = partition(grains, start, end);
  if (start < idx - 1) {
    quickSort(grains, start, idx - 1);
  }
  if (idx + 1 < end) {
    quickSort(grains, idx + 1, end);
  }
}

void quickSortParallel(std::vector<GrainOfSand>* grains, int start, int end,
                       ThreadPool* threadPool) {
  int idx = partition(*grains, start, end);
  if (start < idx - 1) {
    counter++;
    (*threadPool)
        .enqueue(quickSortParallel, grains, start, idx - 1, threadPool);
  }
  if (idx + 1 < end) {
    counter++;
    quickSortParallel(grains, idx + 1, end, threadPool);
  }
  counter--;
}

class LonesomeAdventure : public Adventure {
 public:
  LonesomeAdventure() {}

  virtual uint64_t packEggs(std::vector<Egg> eggs, BottomlessBag& bag) {
    size_t n = eggs.size();
    uint64_t capacity = bag.getCapacity();
    std::vector<std::vector<uint64_t>> dp(n + 1);

    for (std::vector<uint64_t>& vec : dp) {
      vec.resize(capacity + 1);
    }

    for (size_t row = 1; row <= n; row++) {
      Egg egg = eggs[row - 1];
      for (uint64_t i = 0; i <= capacity; i++) {
        dp[row][i] = std::max(dp[row][i], dp[row - 1][i]);
        if (uint64_t(i) >= egg.getSize() &&
            (dp[row - 1][i - egg.getSize()] > 0 || i - egg.getSize() == 0)) {
          dp[row][i] = std::max(
              dp[row][i], dp[row - 1][i - egg.getSize()] + egg.getWeight());
        }
      }
    }

    uint64_t result = 0;
    for (uint64_t i = 0; i <= capacity; i++) {
      result = std::max(result, dp[n][i]);
    }
    return result;
  }

  virtual void arrangeSand(std::vector<GrainOfSand>& grains) {
    quickSort(grains, 0, grains.size() - 1);
  }

  virtual Crystal selectBestCrystal(std::vector<Crystal>& crystals) {
    Crystal bestCrystal;
    for (Crystal c : crystals) {
      if (bestCrystal < c) {
        bestCrystal = c;
      }
    }
    return bestCrystal;
  }
};

Crystal findBestCrystal(std::vector<Crystal>& crystals, int left, int right) {
  Crystal bestCrystal;
  for (int i = left; i <= right; i++) {
    if (bestCrystal < crystals[i]) {
      bestCrystal = crystals[i];
    }
  }
  return bestCrystal;
}

void packEggsInSegment(Egg egg, std::vector<std::vector<uint64_t>>* dp, int row,
                       int left, int right) {
  for (int i = left; i <= right; i++) {
    (*dp)[row][i] = std::max((*dp)[row][i], (*dp)[row - 1][i]);
    if (uint64_t(i) >= egg.getSize() &&
        ((*dp)[row - 1][i - egg.getSize()] > 0 || i - egg.getSize() == 0)) {
      (*dp)[row][i] = std::max(
          (*dp)[row][i], (*dp)[row - 1][i - egg.getSize()] + egg.getWeight());
    }
  }
}

class TeamAdventure : public Adventure {
 public:
  explicit TeamAdventure(uint64_t numberOfShamansArg)
      : numberOfShamans(numberOfShamansArg),
        councilOfShamans(numberOfShamansArg) {}

  uint64_t packEggs(std::vector<Egg> eggs, BottomlessBag& bag) {
    uint64_t capacity = bag.getCapacity();
    uint64_t n = eggs.size();

    if (capacity == 0) {
      return 0;
    }

    std::vector<std::vector<uint64_t>> dp(n + 1);

    for (std::vector<uint64_t>& vec : dp) {
      vec.resize(capacity + 1);
    }

    std::vector<std::pair<int, int>> segments;

    uint64_t numOfBlocks = std::min(numberOfShamans, capacity + 1);
    uint64_t blockSize = (capacity + 1) / numOfBlocks;
    uint64_t toAdd = (capacity + 1) % numOfBlocks;

    int last = -1;
    for (uint64_t i = 0; i < numOfBlocks; i++) {
      int left = last + 1;
      int right = left + blockSize - 1;
      if (i < toAdd) {
        right++;
      }
      segments.push_back({left, right});
      last = right;
    }

    std::vector<std::future<void>> packedEggs(numOfBlocks);

    for (size_t i = 0; i < n; i++) {
      for (uint64_t j = 0; j < numOfBlocks; j++) {
        packedEggs[j] =
            councilOfShamans.enqueue(packEggsInSegment, eggs[i], &dp, i + 1,
                                     segments[j].first, segments[j].second);
      }
      for (uint64_t j = 0; j < numOfBlocks; j++) {
        packedEggs[j].get();
      }
    }

    uint64_t result = 0;
    for (uint64_t i = 0; i <= capacity; i++) {
      result = std::max(result, dp[n][i]);
    }
    return result;
  }

  virtual void arrangeSand(std::vector<GrainOfSand>& grains) {
    counter++;
    councilOfShamans.enqueue(quickSortParallel, &grains, 0, grains.size() - 1,
                             &councilOfShamans);

    while (counter != 0) {
    }
  }

  virtual Crystal selectBestCrystal(std::vector<Crystal>& crystals) {
    uint64_t n = crystals.size();
    uint64_t numOfBlocks = std::min(numberOfShamans, n);

    std::vector<std::future<Crystal>> bestCrystals(numOfBlocks);
    uint64_t blockSize = n / numOfBlocks;
    uint64_t toAdd = n % numOfBlocks;

    int last = -1;
    for (uint64_t i = 0; i < numOfBlocks; i++) {
      int left = last + 1;
      int right = left + blockSize - 1;
      if (i < toAdd) {
        right++;
      }

      bestCrystals[i] =
          councilOfShamans.enqueue(findBestCrystal, crystals, left, right);
      last = right;
    }

    Crystal bestCrystal;
    for (uint64_t i = 0; i < numOfBlocks; i++) {
      Crystal c = bestCrystals[i].get();
      if (bestCrystal < c) {
        bestCrystal = c;
      }
    }
    return bestCrystal;
  }

 private:
  uint64_t numberOfShamans;
  ThreadPool councilOfShamans;
};

#endif  // SRC_ADVENTURE_H_
