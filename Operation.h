//
// Created by Augustin Pan on 4/15/20.
//

#ifndef LIB_OPERATION_H
#define LIB_OPERATION_H

#include <random>
#include <chrono>
#include <utility>
#include <iostream>


namespace augmentorLib {

    const double LOWER_BOUND_PROB = 0.0;
    const double UPPER_BOUND_PROB = 1.0;
    const unsigned NULL_SEED = 0;


    template <typename DataType>
    class UniformDistributionGenerator {
    private:
        std::default_random_engine generator;
        std::uniform_real_distribution<DataType> distribution;

        static unsigned init_seed(unsigned seed) {
            if (seed == NULL_SEED) {
                return std::chrono::system_clock::now().time_since_epoch().count();
            }
            return seed;
        }

    public:
        UniformDistributionGenerator(): UniformDistributionGenerator<DataType>(NULL_SEED) {};

        ~UniformDistributionGenerator() = default;

        explicit UniformDistributionGenerator(unsigned seed):
                generator{init_seed(seed)},
                distribution{LOWER_BOUND_PROB, UPPER_BOUND_PROB} {}

        inline DataType operator()() {
            return distribution(generator);
        }
    };

    //TODO: use concept to constrain the value type to images
    template<typename Image>
    class Operation {
    private:
        double probability;
        UniformDistributionGenerator<double> generator;

    protected:
        inline bool operate_this_time() {
            return generator() <= probability;
        }

    public:

        Operation(): Operation{UPPER_BOUND_PROB} {};

        ~Operation() = default;

        explicit Operation(double prob, unsigned seed = NULL_SEED): probability{prob}, generator{seed} {}

        template <typename Container>
        Container&& perform(Container&&);

        // use pointer here, because we can use nullptr to indicate the operation did not occur.
        virtual Image* perform(Image* image) = 0;
    };


    template<typename Image>
    class StdoutOperation: Operation<Image> {
    private:
        std::string str;
    public:
        StdoutOperation(): str{""}, Operation<Image>{} {};

        explicit StdoutOperation(std::string s, double prob = UPPER_BOUND_PROB, unsigned seed = NULL_SEED):
                Operation<Image>{prob, seed}, str{std::move(s)} {};

        Image * perform(Image* image) override;

    };


    template<typename Image>
    template<typename Container>
    Container&& Operation<Image>::perform(Container&& container) {
        auto results = Container();
        for (auto& image : container) {
            results.push(Operation<Image>::perform(image));
        }
        return results;
    }


    template<typename Image>
    Image * StdoutOperation<Image>::perform(Image* image) {
        if (!Operation<Image>::operate_this_time()) {
            return nullptr;
        }
        std::cout << "(Image*) Stdout Operation is called:" << std::endl << str << std::endl;
        return image;
    }
}



#endif //LIB_OPERATION_H


