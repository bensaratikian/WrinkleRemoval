//
//  Patch.h
//  Training
//
//  Created by Ben on 5/25/19.
//  Copyright © 2019 Ben Saratikian. All rights reserved.
//

#ifndef Patch_h
#define Patch_h

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>


enum class InpaintType {
    Left,
    Right,
    Top,
    Bottom
};

using uint = unsigned int;
using namespace cv;
using namespace std;

void makeBlack(Mat& tile) {
    for(uint y = 0; y < tile.rows; ++y)
        for(uint x = 0; x < tile.cols; ++x) {
            tile.at<Vec3b>(y, x)[0] =
            tile.at<Vec3b>(y, x)[1] =
            tile.at<Vec3b>(y, x)[2] = 0;
        }
}

class Patch final {
public:
    Patch(const Mat& _patch, uint _x, uint _y, bool _containsGap = false)
    : patch(new Mat(_patch))
    , x(_x)
    , y(_y)
    , containsGap(_containsGap)
    , lenght((uint)_patch.rows) {}
    
    ~Patch() {
        delete patch;
    }
    
    std::pair<uint, uint> get_coord() const {
        return { x, y };
    }
    
    const Mat& get_patch() const {
        return *patch;
    }
    
    bool contains_gap() const {
        return containsGap;
    }
    
    uint get_lenght() const {
        return lenght;
    }
    
    uint get_x() const {
        return x;
    }
    
    uint get_y() const {
        return y;
    }
    
    void gap_setter() {
        containsGap = false;
    }
    
private:
    Mat* patch;
    uint x;
    uint y;
    bool containsGap;
    uint lenght;
    //std::set<OverlapType> s;
    //Mat overlap;
};

#endif /* Patch_h */
