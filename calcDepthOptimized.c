// CS 61C Fall 2014 Project 3

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

// unrolling the inside x loop is known to increase by 0.5 Gflop/s

float displacement(int dx, int dy) {
    float squaredDisplacement = dx * dx + dy * dy;
    float displacement = sqrt(squaredDisplacement);
    return displacement;
}

void calcDepth(float *depth,
               float *left,
               float *right,
               int imageWidth,
               int imageHeight,
               int featureWidth,
               int featureHeight,
               int maximumDisplacement,
               size_t* floatOps) {
    if (floatOps != NULL) {
        *floatOps = 0;
    }
    
    for (int x = 0; x < imageWidth; x++) {
        for (int y = 0; y < imageHeight; y++) {
            if ((y < featureHeight) ||
                (y >= imageHeight - featureHeight) ||
                (x < featureWidth) ||
                (x >= imageWidth - featureWidth)) {
                depth[y * imageWidth + x] = 0;
            } else {
                float minimumSquaredDifference = -1;
                int minimumDy = 0;
                int minimumDx = 0;
                
                for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++) {
                    for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++) {
                        if (y + dy - featureHeight < 0 ||
                            y + dy + featureHeight >= imageHeight ||
                            x + dx - featureWidth < 0 ||
                            x + dx + featureWidth >= imageWidth) {
                            continue;
                        }
                        
                        float squaredDifference = 0;
                        
                        for (int boxY = -featureHeight; boxY <= featureHeight; boxY++) {
                            for (int boxX = -featureWidth; boxX <= featureWidth; boxX ++) {
                                int leftX = x + boxX;
                                int leftY = y + boxY;
                                int rightX = leftX + dx;
                                int rightY = leftY + dy;
                                
                                float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
                                squaredDifference += difference * difference;
                                
                                if (floatOps != NULL) {
                                    *floatOps += 3;
                                }
                            }
                        }
                        
                        if ((minimumSquaredDifference == -1) ||
                            ((minimumSquaredDifference == squaredDifference) &&
                             (displacement(dx, dy) < displacement(minimumDx, minimumDy))) ||
                            (minimumSquaredDifference > squaredDifference)) {
                            minimumSquaredDifference = squaredDifference;
                            minimumDx = dx;
                            minimumDy = dy;
                        }
                    }
                }
                
                if (minimumSquaredDifference != -1) {
                    if (maximumDisplacement == 0) {
                        depth[y * imageWidth + x] = 0;
                    } else {
                        depth[y * imageWidth + x] = displacement(minimumDx, minimumDy);
                    }
                } else {
                    depth[y * imageWidth + x] = 0;
                }
            }
        }
    }
}

void calcDepthOptimized(float *depth,
                        float *left,
                        float *right,
                        int imageWidth,
                        int imageHeight,
                        int featureWidth,
                        int featureHeight,
                        int maximumDisplacement) {
    calcDepth(depth, left, right, imageWidth, imageHeight,
              featureWidth, featureHeight, maximumDisplacement, NULL);
}
