//
// Created by Augustin Pan on 4/15/20.
//

#ifndef LIB_OPERATION_H
#define LIB_OPERATION_H

#include <random>
#include <chrono>
#include <utility>
#include <iostream>


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
template<typename ImageContainer>
class Operation {
private:
    typedef typename ImageContainer::value_type Image;

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

    virtual ImageContainer* perform(ImageContainer* container) = 0;
};


template<typename ImageContainer>
class StdoutOperation: Operation<ImageContainer> {
private:
    std::string str;
public:
    StdoutOperation(): str{""}, Operation<ImageContainer>{} {};

    explicit StdoutOperation(std::string s, double prob = UPPER_BOUND_PROB, unsigned seed = NULL_SEED):
        Operation<ImageContainer>{prob, seed}, str{std::move(s)} {};

    // use pointer here, because we can use nullptr to indicate the operation did not occur.
    ImageContainer* perform(ImageContainer* container) override {
        if (!Operation<ImageContainer>::operate_this_time()) {
            return nullptr;
        }
        std::cout << "Stdout Operation is called:" << std::endl << str << std::endl;
        return container;
    }
};

#endif //LIB_OPERATION_H
