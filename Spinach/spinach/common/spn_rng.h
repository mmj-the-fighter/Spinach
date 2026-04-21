// ============================================
// Spine Random Number Generator: Generated using claude.ai and chatgpt.com
// ============================================
#ifndef SPNRNG_H
#define SPNRNG_H

#include <random>
#include <chrono>
#include <cstdint>

namespace spn {

    class RandomGen {
    public:
        static RandomGen& GetInstance() {
            static RandomGen instance;
            return instance;
        }

        // Generate random int across full int range
        int Generate() {
            return distribution(rng);
        }

        // Generate random int in [min, max] inclusive
        int GenerateInRange(int min, int max) {
            return std::uniform_int_distribution<int>{min, max}(rng);
        }

        // Generate random float in [0.0, 1.0]
        float GenerateFloat() {
            return std::uniform_real_distribution<float>{0.0f, 1.0f}(rng);
        }

        // Re-seed manually if needed (e.g. for reproducible tests)
        void Seed(uint32_t seed) {
            rng.seed(seed);
        }

        std::mt19937& GetRng() { return rng; }

    private:
        RandomGen() : rng(MakeSeed()) {}
        RandomGen(const RandomGen&) = delete;
        RandomGen& operator=(const RandomGen&) = delete;

        static uint64_t MakeSeed() {
            // Combine random_device + clock for robustness
            // (random_device is broken/constant on some MinGW builds)
            uint64_t rd_seed = std::random_device{}();
            uint64_t time_seed = static_cast<uint64_t>(
                std::chrono::steady_clock::now().time_since_epoch().count()
                );
            return rd_seed ^ (time_seed << 1);
        }

        std::mt19937 rng;
        std::uniform_int_distribution<int> distribution{
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max()
        };
    };

} // namespace spn

#endif // SPNRNG_H