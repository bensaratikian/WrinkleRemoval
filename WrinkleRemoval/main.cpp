//
//  main.cpp
//  Training
//
//  Created by Ben Saratikian on 4/29/19.
//  Copyright © 2019 Ben Saratikian. All rights reserved.
//

#include "Grid.h"

static uint counter = 0;

uint random(uint min, uint max)
{
    static bool first = true;
    if (first)
    {
        srand(time(NULL));
        first = false;
    }
    return min + rand() % (max - min + 1);
}

double dist(const Patch* p1, const Patch* p2) {
    return std::pow(p1->get_x() - p2->get_x(), 2) + std::pow(p1->get_y() - p2->get_y(), 2);
}

void inpaint_patch(const Mat& src, Mat& dst) {
    for(uint y = 0; y < src.rows; ++y)
        for(uint x = 0; x < src.cols; ++x) {
            dst.at<Vec3b>(y, x)[0] = src.at<Vec3b>(y, x)[0];
            dst.at<Vec3b>(y, x)[1] = src.at<Vec3b>(y, x)[1];
            dst.at<Vec3b>(y, x)[2] = src.at<Vec3b>(y, x)[2];
        }
}

void Horizontalstich2(Mat& img1, Mat& img2, uint overlapS = 20) {
    if(img1.cols!=img2.cols) {
        std::cerr << "Cols are not equal\n";
        return;
    }
    std::vector<std::vector<double>> E;
    E.resize(overlapS);
    for (uint i = 0; i < overlapS; ++i) {
        E[i].resize(img1.cols);
        for (uint j = 0; j < img1.cols; ++j) {
//            E[i][j] = std::sqrt(errorCalc(img1.rows - i - overlapS, j, i, j, img1, img2));
        }
    }
    std::vector<std::vector<double>> Ec(E);
    std::vector<uint> T(img1.cols-1);
    uint j = 0;
    for (uint i = 1; i < overlapS; ++i)
        if(E[i][img1.cols - 1] < E[j][img1.cols - 1])
        j = i;
    for(uint x = img1.cols-2; x >= 0; --x) {
        double min = Ec[j][x+1];
        uint yM = j;
        if (j > 0 && Ec[j - 1][x+1] < min) {
            min = Ec[j - 1][x+1];
            yM = j - 1;
        }
        if (j < overlapS - 1 && Ec[j + 1][x+1] < min) {
            min = Ec[j + 1][x+1];
            yM = j + 1;
        }
        Ec[j][x] = E[j][x] + min;
        T[x] = yM;
        j = yM;
        
    }
    Mat result(Size(img2.cols, img2.rows + img1.rows-overlapS), img1.type());
    for (uint i = 0; i < img2.cols-2; ++i) {
        
        uint X = T[i+1];
        
        for (uint j = 0; j < img1.rows-X; ++j) {
            result.at<Vec3b>(j, i) = img1.at<Vec3b>(j,i);
        }
        int k = 0;
        for(uint j= img1.rows - X;j<img2.rows+img1.rows-overlapS;++j)
            result.at<Vec3b>(j, i) = img2.at<Vec3b>(k++,i);
    }
    imwrite("/Users/lm/Desktop/inPaint/inPaint/BlaV1.jpg",result);
}

void inpaint(const Mat& mask, const Mat& image, InpaintType type = InpaintType::Top) {
    Grid grid(mask, image);
    std::for_each(grid.get_patches().begin(), grid.get_patches().end(), [&](const Patch* it) {
        bool b = true;
        for(uint i = 0; i < grid.patches_size(); ++i) {
            const Patch* pptr = grid.get_patches()[i];
            if(!it->contains_gap()) {
                break;
            }
            //Finding nearest neighbors and using their color
               if(((pptr->get_x() == it->get_x() &&
                pptr->get_y() == it->get_y() - it->get_lenght()) ||
                (pptr->get_x() == it->get_x() &&
                pptr->get_y() == it->get_y() + it->get_lenght()) ||
                (pptr->get_y() == it->get_y() &&
                pptr->get_x() == it->get_x() - it->get_lenght()) ||
                (pptr->get_y() == it->get_y() &&
                pptr->get_x() == it->get_x() - it->get_lenght()) ||
                (pptr->get_x() == it->get_x() - it->get_lenght() &&
                pptr->get_y() == it->get_y() - it->get_lenght()) ||
                (pptr->get_x() == it->get_x() + it->get_lenght() &&
                pptr->get_y() == it->get_y() + it->get_lenght()) ||
                (pptr->get_x() == it->get_x() + it->get_lenght() &&
                pptr->get_y() == it->get_y() - it->get_lenght()) ||
                (pptr->get_x() == it->get_x() - it->get_lenght() &&
                pptr->get_y() == it->get_y() + it->get_lenght()) ||
                (pptr->get_x() == it->get_x() - 2*it->get_lenght() &&
                pptr->get_y() == it->get_y()) ||
                (pptr->get_x() == it->get_x() + 2*it->get_lenght() &&
                pptr->get_y() == it->get_y())  ||
                (pptr->get_y() == it->get_y() - 2*it->get_lenght() &&
                pptr->get_x() == it->get_x()) ||
                (pptr->get_y() == it->get_y() + 2*it->get_lenght() &&
                pptr->get_x() == it->get_x())) &&
                !pptr->contains_gap()) {
                
                Rect rect1(it->get_x(), it->get_y(), it->get_lenght(), it->get_lenght());
                const Mat& patch_img = image(rect1);
                Rect rect2(grid.get_patches()[i]->get_x(), grid.get_patches()[i]->get_y(), it->get_lenght(), it->get_lenght());
                const Mat& neighbor_patch = image(rect2);
                inpaint_patch(neighbor_patch, const_cast<Mat&>(patch_img));
                   b = false;
                break;
                }
            }
        if(b) {
            ++counter;
            std::pair<uint, uint> p1 = {grid.get_x().first - grid.block_size(), it->get_y()};
            std::pair<uint, uint> p2 = {it->get_x(), grid.get_y().first - grid.block_size()};
            std::pair<uint, uint> p3 = {grid.get_x().first + grid.width(), it->get_y()};
            std::pair<uint, uint> p4 = {it->get_x(), grid.get_y().first + grid.height()};
            std::pair<uint, uint> p;

            switch(type) {
                case InpaintType::Top:
                    p = p2;
                    break;
                case InpaintType::Right:
                    p = p3;
                    break;
                case InpaintType::Bottom:
                    p = p4;
                    break;
                case InpaintType::Left:
                    p = p1;
            }
            Rect rect(p.first, p.second, it->get_lenght(), it->get_lenght());
            const Mat& patch_img = image(Rect(it->get_x(), it->get_y(), it->get_lenght(), it->get_lenght()));
            const Mat& neighbor_patch = image(rect);
            inpaint_patch(neighbor_patch, const_cast<Mat&>(patch_img));
        }
    });
}

void removeGap(const Mat& mask, const Mat& image) {
    Grid grid(mask, image);
    std::for_each(grid.get_patches().begin(), grid.get_patches().end(), [&image](const Patch* it) {
        Rect rect(it->get_x(), it->get_y(), it->get_lenght(), it->get_lenght());
        const Mat& tile = image(rect);
        if(!it->contains_gap())
            makeBlack(const_cast<Mat&>(tile));
    });
}

int main(int argc, const char * argv[]) {
   Mat image = imread("/Users/bs/Downloads/ben.jpg", CV_LOAD_IMAGE_COLOR);
   Mat mask = imread("/Users/bs/Downloads/ben1.jpg", CV_LOAD_IMAGE_COLOR);

    if(!image.data && !mask.data) // Check for invalid input
    {
        std::cerr <<  "Could not open or find the image" << std::endl;
        return -1;
    }
    inpaint(mask, image, InpaintType::Top);
    imwrite("/Users/bs/Downloads/inpainted5.jpg", image);
    imshow("Blacked", image);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
