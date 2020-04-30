//
// Created by Goutham Reddy Kotapalle on 2020-04-29.
//

#include "gtest/gtest.h"
#include "Augmentor.h"
#include "jpeg.h"

class AugmentorTest : public ::testing::Test {

protected:
    augmentorLib::Augmentor* aug;
    Image* img;

    AugmentorTest() {

    }

    virtual ~AugmentorTest() {
    }

    virtual void SetUp() {
         aug = new augmentorLib::Augmentor("/Users/gouthamreddykotapalle/Desktop/photos/", "/Users/gouthamreddykotapalle/Desktop/results/");
         img = new Image("/Users/gouthamreddykotapalle/Desktop/photos/input_0.jpg");
    }

    virtual void TearDown() {
        delete aug;
        delete img;
    }

};


TEST_F(AugmentorTest, resize0)
{
    size_t output_height = 200;
    size_t output_width = 200;
    aug->resize(output_height,output_width,1).sample(1);
    Image image_res = Image("/Users/gouthamreddykotapalle/Desktop/results/output_0.jpg");

    EXPECT_TRUE(output_height==image_res.getHeight() && output_width==image_res.getWidth());
}


TEST_F(AugmentorTest, invert0)
{
    aug->invert(1).sample(1);
    Image image_res = Image("/Users/gouthamreddykotapalle/Desktop/results/output_0.jpg");

    std::vector<uint8_t> temp =  image_res.getPixel(0,0);
    for(auto &p:temp)
    {
        p=~p;
    }
    EXPECT_TRUE(img->getPixel(0,0)==temp);
}

TEST_F(AugmentorTest, zoom0)
{
    aug->zoom(2, 2, 1).sample(1);
    Image image_res = Image("/Users/gouthamreddykotapalle/Desktop/results/output_0.jpg");
    std::vector<uint8_t> temp =  image_res.getPixel(50,50);
    EXPECT_TRUE(img->getPixel(100,100)!=temp);
}


TEST_F(AugmentorTest, crop0)
{
    size_t output_height = 300;
    size_t output_width = 300;
    aug->crop(output_height, output_width, 1).sample(1);
    Image image_res = Image("/Users/gouthamreddykotapalle/Desktop/results/output_0.jpg");
    EXPECT_TRUE(output_height==image_res.getHeight() && output_width==image_res.getWidth());
}



TEST_F(AugmentorTest, rotate0)
{

    aug->rotate(45,90,1).sample(1);
    Image image_res = Image("/Users/gouthamreddykotapalle/Desktop/results/output_0.jpg");

    EXPECT_TRUE(img->getHeight()==image_res.getHeight() && img->getWidth()==image_res.getWidth());
}





int main(int argc, char **argv) { ::testing::InitGoogleTest(&argc, argv); return RUN_ALL_TESTS(); }


