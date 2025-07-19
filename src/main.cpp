#include <iostream>
#include "Image.h"

void makeMp4(const std::string& path, const int2 size) {
    std::string command = "ffmpeg -loglevel quiet -y -framerate 30 -i ";
    command += path;
    command += "frame%03d.png -vf scale=";
    command += std::to_string(size.x) + ':' + std::to_string(size.y) + ' ';
    command += "-c:v libx264 -pix_fmt yuv420p output.mp4";

    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "FFmpeg command failed with code: " << result << std::endl;
    } else {
        std::cout << "Video created successfully.\n";
    }
}
void createFrame(const std::string& path, const Image& image, const int frameNum) {
    std::string filename = path + "frame";
    const int zeros = 3 - int(std::to_string(frameNum).length());
    for (int j = 0; j < zeros; ++j) filename += '0';
    filename += std::to_string(frameNum);
    filename += ".png";
    image.makePng(filename);
}
Image readFrame(const std::string& path, const int frameNum) {
    std::string filename = path + "frame";
    const int zeros = 3 - int(std::to_string(frameNum).length());
    for (int j = 0; j < zeros; ++j) filename += '0';
    filename += std::to_string(frameNum);
    filename += ".png";
    Image image = Image(filename);
    return image;
}

float softThreshold(const float x, const float threshold) {
    const float knee = 510-2*threshold;
    if (x < threshold)
        return 0.0f;
    if (x < threshold + knee)
        return ((x - threshold) * (x - threshold)) / (2.0f * knee);
    return x - threshold - (knee / 2.0f);
}

int main() {
    if (false) {
        Image image(2560, 1440);
        const std::vector<float3> color = {{25.5,255,382.5}, {510, 255, 51}, {51, 510, 255}};
        const std::vector<float> radius = {100,50, 75};
        const std::vector<float2> pos = {{750, 500}, {1000, 1000}, {2000, 750}};
        for (int x = 0; x < image.getSize().x; ++x) {
            for (int y = 0; y < image.getSize().y; ++y) {
                for (int i = 0; i < color.size(); ++i) {
                    float2 p = pos[i];
                    const float r = radius[i];
                    const float dis2 = float(pow(float(x)-p.x,2)+pow(float(y)-p.y,2));
                    if (dis2 < float(r*r)) {
                        image.write(x,y,color[i]);
                        break;
                    }
                }
            }
        }
        for (int x = 0; x < image.getSize().x; ++x) {
            for (int y = 0; y < image.getSize().y; ++y) {
                if (x > 1270 and x < 1290 and y > 620 and y < 820) image.write(x,y,{510,51,510});
                else if (x > 1180 and x < 1380 and y > 710 and y < 730) image.write(x,y,{510,51,510});
            }
        }
    }
    Image image("text4.png");
    image.apply([](float3 x) {
        x.clamp(0, 8192);
        if (x.mag2() < 10000) return float3(0);
        return x*1.5f;
    });
    //image.upsample();
    //image.upsample();
    //image.upsample();

    bool bloomActive = true;
    const int numMipLevels = int(log2(float(image.getSize().y)))-2;
    float falloff = 0.5f;
    std::cout << numMipLevels << std::endl;
    const std::vector<float> kernel {
        0.00391f,  // -5
    0.01018f,  // -4
    0.02459f,  // -3
    0.04947f,  // -2
    0.08553f,  // -1
    0.12066f,  //  0 (center)
    0.08553f,  // +1
    0.04947f,  // +2
    0.02459f,  // +3
    0.01018f,  // +4
    0.00391f   // +5
    };

    auto bloom = image;
    bloom.apply([](const float x){return softThreshold(x, 127.5f);});
    bloom.clamp(0, 8192);
    image.makePng("noBloom.png");
    if (bloomActive) {
        bloom.downsample();

        //downsample
        std::vector<Image> mipLevels;
        mipLevels.push_back(bloom);
        bloom.makePng("downsample0.png");
        for (int i = 0; i < numMipLevels-1; i++) {
            bloom.downsample();
            bloom.blur(kernel);
            mipLevels.push_back(bloom);
            Image bloom2 = bloom;
            bloom2.makePng("downsample"+std::to_string(i+1)+".png");
        }

        //upsample
        float weight = 1;
        float total = 0;
        bloom *= float3(weight); // apply weight to lowest mip before any += happens
        weight *= falloff;
        total += weight;
        for (int i = 0; i < numMipLevels-1; i++) {
            bloom.upsample();
            const Image& currentLevel = mipLevels[numMipLevels-i-2];
            bloom.resize(currentLevel.getSize());
            bloom += currentLevel;
            currentLevel *= float3(weight);
            bloom += currentLevel;
            weight *= falloff;
            total += weight;
            bloom.makePng("upsample"+std::to_string(i+1)+".png");
        }
        bloom *= float3(1/total);

        //tonemap
        bloom *= float3(0.5f);
        bloom.aces();
        bloom.upsample();
        bloom.clamp(0, 255);
        bloom.makePng("finishedBloom.png");
    }
    image += bloom;
    image.linearize();

    image.makePng("image.png");
}