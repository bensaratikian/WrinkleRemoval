//
//  Grid.h
//  Training
//
//  Created by Ben on 5/25/19.
//  Copyright © 2019 Ben Saratikian. All rights reserved.
//

#ifndef Grid_h
#define Grid_h
#include "Patch.h"

bool hasWhite(uint y, uint x, const Mat& mask) {
    uint error = 30;
    uint sum = mask.at<Vec3b>(y, x)[0];
    sum += mask.at<Vec3b>(y, x)[1];
    sum += mask.at<Vec3b>(y, x)[2];
    return sum < error;
}

class Grid final {
public:
    Grid(const Mat& mask, const Mat& img, uint N = 10)
    : Xmin(-1)
    , Xmax(0)
    , Ymin(-1)
    , Ymax(0)
    , GRID_SIZE(N) {
        for(uint y = 0; y < mask.rows; ++y) {
            for(uint x = 0; x < mask.cols; ++x) {
                if(!hasWhite(y, x, mask)) {
                    if(x < Xmin)
                        Xmin = x;
                    if(x > Xmax)
                        Xmax = x;
                    if(y < Ymin)
                        Ymin = y;
                    if(y > Ymax)
                        Ymax = y;
                }
            }
        }
        Rect rect(Xmin, Ymin, width(), height());
        
        grid = img(rect);
        const Mat& grid_mask = mask(rect);
        
        for (uint y = 0; y < grid.rows; y += N)
            for (uint x = 0; x < grid.cols; x += N)
            {
                const Mat& tile = grid(Range(y, min(y + N, (uint)grid.rows)),
                                    Range(x, min(x + N, (uint)grid.cols)));
                const Mat& tile_mask = grid_mask(Range(y, min(y + N, (uint)grid.rows)),
                                                 Range(x, min(x + N, (uint)grid.cols)));
                patches.emplace_back(new Patch(tile, x + Xmin, y + Ymin, white(tile_mask)));
            }
    }
    
    
    ~Grid() {
        for(uint i = 0; i < patches_size(); ++i)
            delete patches[i];
    }
    
    uint patches_size() const {
        return patches.size();
    }
    
    void showPatches() const {
        for(uint i = 0; i < patches_size(); ++i) {
            namedWindow("Patch Window", CV_WINDOW_NORMAL);
            const Patch* tile = patches[i];
            imshow("Patch Window", tile->get_patch());
            waitKey();
        }
    }
    
    //Some getters below
    std::pair<uint, uint> get_x() const {
        return { Xmin, Xmax };
    }
    std::pair<uint, uint> get_y() const {
        return { Ymin, Ymax };
    }
    const std::vector<const Patch*>& get_patches() const {
        return patches;
    }
    const Mat& get_image() const {
        return grid;
    }
    
    uint block_size() const {
        return GRID_SIZE;
    }
    
    uint width() const {
        return Xmax - Xmin;
    }
    
    uint height() const {
        return Ymax - Ymin;
    }
    
    Rect get_rect() const {
        return Rect(Xmin, Ymin, width(), height());
    }
    
private:
    bool white(const Mat& tile) const {
        for(uint y = 0; y < tile.rows; ++y)
            for(uint x = 0; x < tile.cols; ++x) {
                if(!hasWhite(y, x, tile))
                    return true;
            }
        return false;

    }
private:
    uint Xmin;
    uint Xmax;
    uint Ymin;
    uint Ymax;
    uint GRID_SIZE;
    std::vector<const Patch*> patches;
    Mat grid;
};

#endif /* Grid_h */
