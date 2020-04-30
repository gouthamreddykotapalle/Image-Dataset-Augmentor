//
// Created by Augustin Pan on 4/24/20.
//

#ifndef LIB_GAUSSIAN_FILTER_H
#define LIB_GAUSSIAN_FILTER_H

#include <vector>
#include <cmath>
#include <numeric>

namespace augmentorLib {

    template<unsigned N=0, bool Static = (N > 0) >
    class gaussian_blur_filter_1D;

    /// Gaussian blur filter class
    ///
    /// True Version
    /// \tparam N Denotes the Kernel value for the blur operation
    template<unsigned N>
    class gaussian_blur_filter_1D<N, true> {

        double array[N];

    public:
        gaussian_blur_filter_1D() = delete;

        explicit gaussian_blur_filter_1D(const double sigma):array{} {
            // make sure the filter has N as odd number
            static_assert(N % 2, "Kernel Size (N) must be odd number for Static Filter");
            assert(sigma > 0);

            auto base = - 1 / (sigma * sigma);
            int x = - (N / 2);
            for (unsigned i = 0; i < N; ++i, ++x) {
                array[i] = std::exp(x * x * base);
            }

            // normalize to unity
            auto norm_factor = std::accumulate(array, array + N, 0.0);
            for (unsigned i = 0; i < N; ++i, ++x) {
                array[i] /= norm_factor;
            }
        }

        inline double operator[](size_t i){
            return array[i];
        }

        inline size_t size() {
            return N;
        }
    };

    /// Gaussian blur filter class
    ///
    /// False Version
    /// \tparam N Denotes the Kernel value for the blur operation
    template<unsigned N>
    class gaussian_blur_filter_1D<N, false> {

        std::vector<double> vector;

    public:
        gaussian_blur_filter_1D() = delete;

        explicit gaussian_blur_filter_1D(double sigma, size_t n): vector(n) {
            static_assert(N == 0, "Kernel Size (N) must be zero for Dynamic Filter");
            assert(n % 2);
            assert(sigma > 0);

            auto base = - 1 / (sigma * sigma);
            int x = -((int) n / 2);
            for (size_t i = 0; i < n; ++i, ++x) {
                vector[i] = std::exp(x * x * base);
            }

            // normalize to unity
            auto norm_factor = std::accumulate(vector.begin(), vector.end(), 0.0);
            for (size_t i = 0; i < n; ++i, ++x) {
                vector[i] /= norm_factor;
            }
        }

        inline double operator[](size_t i){
            return vector[i];
        }

        inline size_t size() {
            return vector.size();
        }
    };
}



#endif //LIB_GAUSSIAN_FILTER_H
